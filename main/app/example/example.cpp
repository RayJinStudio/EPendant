#include "example.h"
#include "example_gui.h"
#include "../../sys/app_controller.h"
#include "../../sys/wifi_controller.h"

#include "stdio.h"

#define EXAMPLE_APP_NAME "Example"

// 动态数据，APP的生命周期结束也需要释放它
struct ExampleAppRunData
{
    unsigned int val1;
    unsigned int val2;
    unsigned int val3;
    WIFI_OBJ wifiList[8];
};

// 常驻数据，可以不随APP的生命周期而释放或删除
struct ExampleAppForeverData
{
    unsigned int val1;
    unsigned int val2;
    unsigned int val3;
};

// 保存APP运行时的参数信息，理论上关闭APP时推荐在 xxx_exit_callback 中释放掉
ExampleAppRunData *run_data = NULL;

// 当然你也可以添加恒定在内存中的少量变量（退出时不用释放，实现第二次启动时可以读取）
// 考虑到所有的APP公用内存，尽量减少 forever_data 的数据占用
static ExampleAppForeverData forever_data;

void wifiScanCallBack(wifi_ap_record_t *records)
{
    int num =8;
    for(int i = 0; i < num; i++)
    {
        uint8_t rssi_level = 0;
        switch (records[i].rssi)
        {
            case -100 ... -88:
                rssi_level = 1; break;
            case -87 ... -77:
                rssi_level = 2; break;
            case -76 ... -66:
                rssi_level = 3; break;
            case -65 ... -55:
                rssi_level = 4; break;
            default:
                if(records[i].rssi < -100)
                {
                    rssi_level = 0;
                }
                else
                {
                    rssi_level = 5;
                }
                break;
        }

	// 逐条打印扫描到的WiFi
        strcpy((run_data->wifiList[i]).ssid, (char*)records[i].ssid);
        (run_data->wifiList[i]).strong = 1;
        (run_data->wifiList[i]).isnull = 1;
        printf("—————【第 %2d 个WiFi】———————\n", i+1);
        printf("WiFi名称: %s\n", records[i].ssid);
        printf("信号强度: %d格\n", rssi_level);
        printf("WiFi: 安全类型: %d\n\n", records[i].authmode);
        (run_data->wifiList[i]).authmode = records[i].authmode;
        addItem((run_data->wifiList[i]).ssid);
    }
    delete[] records;
}

esp_err_t ConnectWifi(int id, char* pwd)
{
    wifi_config_t wifi_config;
    wifi_sta_config_t sta_config;
    strcpy((char*)(sta_config.ssid),(run_data->wifiList[id]).ssid);
    strcpy((char*)(sta_config.password),pwd);
    wifi_config.sta = sta_config;
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_err_t err = esp_wifi_connect();
    ESP_ERROR_CHECK(err);
    return err;
}

int example_init(AppController *sys)
{
    // 初始化运行时的参数
    example_gui_init();
    // 初始化运行时参数
    run_data = (ExampleAppRunData *)calloc(1, sizeof(ExampleAppRunData));
    run_data->val1 = 0;
    run_data->val2 = 0;
    run_data->val3 = 0;
    // 使用 forever_data 中的变量，任何函数都可以用
    // Serial.print(forever_data.val1);

    // 如果有需要持久化配置文件 可以调用此函数将数据存在flash中
    // 配置文件名最好以APP名为开头 以".cfg"结尾，以免多个APP读取混乱
    char info[128] = {0};
    
    
    // uint16_t size = g_flashCfg.readFile("/example.cfg", (uint8_t *)info);
    // // 解析数据
    // // 将配置数据保存在文件中（持久化）
    // g_flashCfg.writeFile("/example.cfg", "value1=100\nvalue2=200");

    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);

    if(mode == WIFI_MODE_STA)
    {
        wifi_ap_record_t* wifiList = new wifi_ap_record_t[8];
        wifi_event(&wifiScanCallBack);
    }

    return 0;
}

static void example_process(AppController *sys,
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
    // 发送请求。如果是wifi相关的消息，当请求完成后自动会调用 example_message_handle 函数
    // sys->send_to(EXAMPLE_APP_NAME, CTRL_NAME,
    //              APP_MESSAGE_WIFI_CONN, (void *)run_data->val1, NULL);

    // 程序需要时可以适当加延时
    // delay(300);
}

static void example_background_task(AppController *sys,
                                    const UserAction *act_info)
{
    // 本函数为后台任务，主控制器会间隔一分钟调用此函数
    // 本函数尽量只调用"常驻数据",其他变量可能会因为生命周期的缘故已经释放

    // 发送请求。如果是wifi相关的消息，当请求完成后自动会调用 example_message_handle 函数
    // sys->send_to(EXAMPLE_APP_NAME, CTRL_NAME,
    //              APP_MESSAGE_WIFI_CONN, (void *)run_data->val1, NULL);

    // 也可以移除自身的后台任务，放在本APP可控的地方最合适
    // sys->remove_backgroud_task();

    // 程序需要时可以适当加延时
    // delay(300);
}

static int example_exit_callback(void *param)
{
    example_gui_del();
    // 释放资源
    if (NULL != run_data)
    {
        free(run_data);
        run_data = NULL;
    }
    return 0;
}

static void example_message_handle(const char *from, const char *to,
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

APP_OBJ example_app = {EXAMPLE_APP_NAME, &app_example, "Author HQ\nVersion 2.0.0\n",
                       example_init, example_process, example_background_task,
                       example_exit_callback, example_message_handle};
