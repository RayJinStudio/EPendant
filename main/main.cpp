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
#include "app/irremote/irremote.h"


static gpio_num_t BLINK_GPIO = (gpio_num_t)12;
static gpio_num_t LED_R_IO = (gpio_num_t)13;
static gpio_num_t KeyIO = (gpio_num_t)0;
static gpio_num_t KeyIO8 = (gpio_num_t)8;
static gpio_num_t KeyIO18 = (gpio_num_t)18;

AppController* appctrl = NULL;

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
    rmt_tx.tx_config.loop_en = false;   //关闭循环发射，只发射一次
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

esp_err_t ConnectWifi()
{
    wifi_config_t wifi_config;
    wifi_sta_config_t sta_config;
    strcpy((char*)(sta_config.ssid),"Letv");
    strcpy((char*)(sta_config.password),"jjs550600");
    wifi_config.sta = sta_config;
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_err_t err = esp_wifi_connect();
    if(err == ESP_OK) printf("success\n");
    else ESP_ERROR_CHECK(err);
    return err;
}

extern "C" void app_main(void)
{
    xTaskCreatePinnedToCore(guiTask1, "gui", 4096 * 2, NULL, 1, NULL, 1);
    vTaskDelay(100);
    appctrl = new AppController();
    appctrl->init();
    appctrl->app_install(&example_app);
    appctrl->app_install(&game_2048_app);
    appctrl->app_install(&irremote_app);
    
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

    nec_tx_init();
    
    initWifiSTA();

    // char ssid[]="Letv";
    // char pwd[]="jjs550600";

    //ConnectWifi();
}

