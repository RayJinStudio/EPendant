#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_controller.h"


#include "esp_log.h"
#include <cstring>

#define TAG "RWIFI"

TaskHandle_t wifi_scan_handle;	

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        ESP_LOGI(TAG,"WIFI started");
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        // if (s_retry_num < IOT_CORE_ESP_MAXIMUM_RETRY) 
        // {
        //     esp_wifi_connect();
        //     s_retry_num++;
        //     ESP_LOGI(TAG, "retry to connect to the AP");
        // } 
        // else 
        // {
        //     xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        // }
        ESP_LOGI(TAG,"connect to the AP fail");
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        // s_retry_num = 0;
        // xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


esp_err_t initWifiSTA()
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = 
    {
        .sta = {
	        .threshold = {
                .authmode = WIFI_AUTH_WPA2_PSK,
            },
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    strcpy((char*)(wifi_config.sta.ssid),"Letv");
    strcpy((char*)(wifi_config.sta.password),"jjs550600");
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    esp_err_t err = esp_wifi_start();
    //esp_err_t err2 = esp_wifi_connect(); 
    ESP_ERROR_CHECK(err);

    // ESP_ERROR_CHECK(err2);
    return err;
}

void task_wifi_scan(void* arg)
{
    //ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    wifi_ap_record_t* records = new wifi_ap_record_t[8];

    uint16_t num;

    esp_wifi_scan_start(NULL, true);
    esp_wifi_scan_get_ap_num(&num);
    if(num > 8)
    {
        num = 8;	// if 的目的是如果扫描到了8个以上的WiFi，则只保留前8个
    }
    esp_wifi_scan_get_ap_records(&num, records);
    // if(num > 8)
    // {
    //     num = 8;	// if 的目的同上
    // }

    void (*wifi_scan_cb)(wifi_ap_record_t*) = (void (*)(wifi_ap_record_t*))arg;
    wifi_scan_cb(records);
    vTaskDelete(NULL);
}



void wifi_event(void (*wifi_scan_cb)(wifi_ap_record_t *records))
{
    printf("hi1\n");
    xTaskCreate(task_wifi_scan,"wifi",4096,(void*) wifi_scan_cb,2,&wifi_scan_handle);
    //xTaskNotifyGive(wifi_scan_handle);	
}
