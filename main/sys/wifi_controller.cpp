#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_controller.h"


#include "esp_log.h"

TaskHandle_t wifi_scan_handle;	

esp_err_t initWifiSTA()
{
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_err_t err = esp_wifi_start();
    ESP_ERROR_CHECK(err);
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
