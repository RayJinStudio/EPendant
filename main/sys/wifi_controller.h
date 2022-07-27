#ifndef WIFI_CONTROLLER_H
#define WIFI_CONTROLLER_H

#include "esp_err.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct WIFI_OBJ
    {
        char ssid[33];
        int strong;
        wifi_auth_mode_t authmode;
        bool isnull;
    };

    esp_err_t initWifiSTA();
    void task_wifi_scan(void *arg);
    void wifi_event(void (*wifi_scan_cb)(wifi_ap_record_t *records));

#ifdef __cplusplus
}
#endif

#endif