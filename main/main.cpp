#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/rmt.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "common/lvgl_init.h"
#include "common/common.h"

#include "sys/app_controller.h"
#include "sys/wifi_controller.h"

#include "app/example/example.h"
#include "app/game_2048/game_2048.h"


static gpio_num_t BLINK_GPIO = (gpio_num_t)12;
static gpio_num_t LED_R_IO = (gpio_num_t)13;
static gpio_num_t KeyIO = (gpio_num_t)0;
static gpio_num_t KeyIO8 = (gpio_num_t)8;
static gpio_num_t KeyIO18 = (gpio_num_t)18;

AppController* appctrl;

extern void guiTask1(void *pvParameter);

int led_r_status = 0;

void key_read(void *pvParameters)
{
    UserAction ua;
    while(1)
    {
        ua.active = UNKNOWN;
        if(gpio_get_level(KeyIO8)==0)//按键按下
        {
            //等待松手，最傻的办法
            if (led_r_status==1)
            {
                led_r_status = 0;
                gpio_set_level(LED_R_IO, 1);//不亮
            }
            else
            {
                led_r_status = 1;
                gpio_set_level(LED_R_IO, 0);//亮
            }
            ua.active = TURN_RIGHT;
        }
        else if(gpio_get_level(KeyIO18)==0)
        {
            ua.active = TURN_LEFT;
        }
        else if(gpio_get_level(KeyIO)==0)
        {
            vTaskDelay(1000/portTICK_RATE_MS);
            if(gpio_get_level(KeyIO)==1)
                ua.active = GO_FORWORD;
            else
                ua.active = RETURN;
        }
        ua.isValid = 1;
        appctrl->main_process(&ua);
        vTaskDelay(10/portTICK_RATE_MS);
    }
}

void init_nvs() 
{
	/*尝试初始化一次nvs_flash*/
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

/*
 * @brief RMT transmitter initialization
 */
static void nec_tx_init()
{
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_CHANNEL_0;
    rmt_tx.gpio_num = (gpio_num_t)19;
    rmt_tx.mem_block_num = 2;   //由于格力红外有70个item，使用2个块，64×2=128个item
    rmt_tx.clk_div = 80;   
    rmt_tx.tx_config.loop_en = true;   //关闭循环发射，只发射一次
    rmt_tx.tx_config.carrier_duty_percent = 50; //载波占空比为50
    rmt_tx.tx_config.carrier_freq_hz = 38000;   //载波频率38khz 红外
    rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH; //载波高电平
    rmt_tx.tx_config.carrier_en = true;    //使能载波
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;    //空闲状态低电平
    rmt_tx.tx_config.idle_output_en = true; //输出使能
    rmt_tx.rmt_mode = RMT_MODE_TX;  //发射模式
    printf("[ 1.1 ] config rmt\n");
    rmt_config(&rmt_tx);
    printf("[ 1.2 ] install rmt driver\n");

    //安装红外发射通道 无需ringbuff
    rmt_driver_install(rmt_tx.channel, 0, 0);
}


uint32_t decoded[] = {9000,4500,560,565,560,565,560,1690,560,1690,560,565,560,565,560,1690,560,565,560,1690,560,565,560,1690,560,565,560,565,560,1690,560,1690,560,565,560,565,560,565,560,565,560,565,560,1690,560,565,560,565,560,565,560,1690,560,1690,560,1690,560,1690,560,565,560,1690,560,1690,560,1690,560,0}; 

/*
 * @brief 填充item的电平和电平时间 需要将时间转换成计数器的计数值 /10*RMT_TICK_10_US
 */
static inline void nec_fill_item_level(rmt_item32_t* item, int high_us, int low_us)
{
    const int RMT_CLK_DIV = 80;                                   // RMT计数器时钟分频器
	const int RMT_TICK_10_US = (80000000 / RMT_CLK_DIV / 100000);
    item->level0 = 1;
    item->duration0 = (high_us);
    item->level1 = 0;
    item->duration1 = (low_us);
}


/*
 * irext_build
 * brief：通过irext构建item 使用全局变量
 */
static void irext_build(rmt_item32_t* item,size_t item_num)
{
	int i = 0;

    nec_fill_item_level(item, decoded[0], decoded[1]);
    for (i = 1; i < item_num;i++)
	{
        item++;
		nec_fill_item_level(item, decoded[2*i], decoded[2*i + 1]);
		
	}
}


static const rmt_item32_t morse_esp[] = {

	{{{ 9000, 1, 4500, 0 }}}, 
	{{{ 560, 1, 565, 0 }}}, 

	{{{ 560, 1, 565, 0 }}}, 
	{{{ 560, 1, 565, 0 }}}, 
	{{{ 560, 1, 565, 0 }}}, 
	{{{ 560, 1, 1690, 0 }}},  

	{{{ 560, 1, 1690, 0 }}}, 
	{{{ 560, 1, 1690, 0 }}}, 
	{{{ 560, 1, 1690, 0 }}}, 
	{{{ 560, 1, 1690, 0 }}}, 
	{{{ 560, 1, 565, 0 }}},  
	{{{ 560, 1, 565, 0 }}}, 

	{{{ 560, 1, 565, 0 }}}, 
};


void send()
{
    rmt_item32_t *item;	//发射item
    size_t size = 0;
    int item_num = 0;

    item_num = 34;   //此处为70
    size = (sizeof(rmt_item32_t) * item_num); //计算70个item所需的字节空间
    item = (rmt_item32_t *)malloc(size);	//记得free
    memset((void *)item, 0, size);
    irext_build(item, item_num); //根据要发射的数据，填充item
    //printf("hhhhh\n");
    //printf("%d %d\n",item[0].duration0,item[0].duration1);

    // while(1)
    // {
        rmt_write_items(RMT_CHANNEL_0, item, item_num, true); //将item写入通道对应的RAM并进入阻塞

        //rmt_write_items(RMT_CHANNEL_0, morse_esp, sizeof(morse_esp) / sizeof(morse_esp[0]), true);
    //     vTaskDelay(5000 / portTICK_PERIOD_MS);
    // }
    rmt_wait_tx_done(RMT_CHANNEL_0, portMAX_DELAY); //等待发送完成 进入阻
    free(item);
}


extern "C" void app_main(void)
{
    xTaskCreatePinnedToCore(guiTask1, "gui", 4096 * 2, NULL, 1, NULL, 1);
    vTaskDelay(100);
    appctrl = new AppController();
    appctrl->init();
    appctrl->app_install(&example_app);
    appctrl->app_install(&game_2048_app);
    
    gpio_reset_pin(BLINK_GPIO);
    gpio_reset_pin(LED_R_IO);
    gpio_reset_pin(KeyIO);
    gpio_reset_pin(KeyIO8);
    gpio_reset_pin(KeyIO18);

    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_R_IO, GPIO_MODE_OUTPUT);
    gpio_set_direction(KeyIO, GPIO_MODE_INPUT);
    gpio_set_direction(KeyIO8, GPIO_MODE_INPUT);
    gpio_set_direction(KeyIO18, GPIO_MODE_INPUT);

    xTaskCreate(key_read,"key",4096*2,NULL,2,NULL);
    
    init_nvs();
    
    initWifiSTA();

    nec_tx_init();
    send();
    // while (1)
	// {
	// 	rmt_write_items(RMT_CHANNEL_0, morse_esp, sizeof(morse_esp) / sizeof(morse_esp[0]), true);
	// 	vTaskDelay(100 / portTICK_PERIOD_MS);

	// }
}

