#include "irremote.h"
#include "irremote_gui.h"
#include "irremote_http.hpp"

#include "../../sys/app_controller.h"
#include "../../sys/wifi_controller.h"

#include "stdio.h"

#include "driver/rmt.h"
#include "cJSON.h"

#define IRREMOTE_APP_NAME "IRREMOTE"

struct IRRemoteAppRunData
{
};

struct IRRemoteAppForeverData
{
    unsigned int val1;
};

static IRRemoteAppRunData *run_data = NULL;

static IRRemoteAppForeverData forever_data;

int irremote_init(AppController *sys)
{
    xTaskCreate(&httpPostTask, "http_post_task", 8192*2, NULL, 5, NULL);

    irremote_gui_init();
    run_data = (IRRemoteAppRunData *)calloc(1, sizeof(IRRemoteAppRunData));
      

    return 0;
}

uint32_t decoded[] = {9000,4500,560,1690,560,565,560,565,560,565,560,565,560,565,560,565,560,565,560,565,560,1690,560,1690,560,1690,560,1690,560,1690,560,1690,560,1690,560,1690,560,1690,560,565,560,565,560,565,560,565,560,565,560,565,560,565,560,565,560,1690,560,1690,560,1690,560,1690,560,1690,560,1690,560,1690,560,1690,560,565,560,565,560,565,560,565,560,565,560,565,560,565,560,565,560,1690,560,1690,560,1690,560,1690,560,1690,560,1690,560,0}; 

/*
 * @brief 填充item的电平和电平时间 需要将时间转换成计数器的计数值 /10*RMT_TICK_10_US
 */
static inline void nec_fill_item_level(rmt_item32_t* item, int high_us, int low_us)
{
    const int RMT_CLK_DIV = 80;
	const int RMT_TICK_10_US = (80000000 / RMT_CLK_DIV / 100000);
    item->level0 = 1;
    item->duration0 = (high_us / 10 * RMT_TICK_10_US);
    item->level1 = 0;
    item->duration1 = (low_us / 10 * RMT_TICK_10_US);
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

void send()
{
    rmt_item32_t *item;
    size_t size = 0;
    int item_num = 0;

    item_num = 50;   //此处为70
    size = (sizeof(rmt_item32_t) * item_num); //计算70个item所需的字节空间
    item = (rmt_item32_t *)malloc(size);	//记得free
    memset((void *)item, 0, size);
    irext_build(item, item_num); //根据要发射的数据，填充item
    
    rmt_write_items(RMT_CHANNEL_0, item, item_num, true); //将item写入通道对应的RAM并进入阻塞

    rmt_wait_tx_done(RMT_CHANNEL_0, portMAX_DELAY); //等待发送完成 进入阻
    free(item);
}

static void irremote_process(AppController *sys,
                            const UserAction *act_info)
{
    extern lv_group_t *group2;
    // if (RETURN == act_info->active)
    // {
    //     sys->app_exit(); // 退出APP
    //     return;
    // }
    if (GO_FORWORD == act_info->active)
    {
        lv_group_send_data(group2, LV_KEY_ENTER);
    }
    else if (TURN_LEFT == act_info->active)
    {
        lv_group_send_data(group2, LV_KEY_LEFT);
    }
    else if (TURN_RIGHT == act_info->active)
    {
        lv_group_send_data(group2, LV_KEY_RIGHT);
    }
    // 发送请求。如果是wifi相关的消息，当请求完成后自动会调用 irremote_message_handle 函数
    // sys->send_to(IRREMOTE_APP_NAME, CTRL_NAME,
    //              APP_MESSAGE_WIFI_CONN, (void *)run_data->val1, NULL);

    // 程序需要时可以适当加延时
    // delay(300);
}

static void irremote_background_task(AppController *sys,
                                    const UserAction *act_info)
{
    // 本函数为后台任务，主控制器会间隔一分钟调用此函数
    // 本函数尽量只调用"常驻数据",其他变量可能会因为生命周期的缘故已经释放

    // 发送请求。如果是wifi相关的消息，当请求完成后自动会调用 irremote_message_handle 函数
    // sys->send_to(IRREMOTE_APP_NAME, CTRL_NAME,
    //              APP_MESSAGE_WIFI_CONN, (void *)run_data->val1, NULL);

    // 也可以移除自身的后台任务，放在本APP可控的地方最合适
    // sys->remove_backgroud_task();

    // 程序需要时可以适当加延时
    // delay(300);
}

static int irremote_exit_callback(void *param)
{
    irremote_gui_del();

    if (NULL != run_data)
    {
        free(run_data);
        run_data = NULL;
    }

    return 0;
}

static void irremote_message_handle(const char *from, const char *to,
                                   APP_MESSAGE_TYPE type, void *message,
                                   void *ext_info)
{
    // 目前主要是wifi开关类事件（用于功耗控制）
    // switch (type)
    // {
    // case APP_MESSAGE_WIFI_CONN:
    // {
    //     // todo
    // }
    // break;
    // case APP_MESSAGE_WIFI_AP:
    // {
    //     // todo
    // }
    // break;
    // case APP_MESSAGE_WIFI_ALIVE:
    // {
    //     // wifi心跳维持的响应 可以不做任何处理
    // }
    // break;
    // case APP_MESSAGE_GET_PARAM:
    // {
    //     char *param_key = (char *)message;
    // }
    // break;
    // case APP_MESSAGE_SET_PARAM:
    // {
    //     char *param_key = (char *)message;
    //     char *param_val = (char *)ext_info;
    // }
    // break;
    // default:
    //     break;
    // }
}

APP_OBJ irremote_app = {IRREMOTE_APP_NAME, &app_irremote, "Author HQ\nVersion 2.0.0\n",
                       irremote_init, irremote_process, irremote_background_task,
                       irremote_exit_callback, irremote_message_handle};
