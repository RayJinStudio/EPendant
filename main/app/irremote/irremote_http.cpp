#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_tls.h"
#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#include "esp_http_client.h"

#define TAG "IRREMOTE"

int resplen;
char respbuf[1024];

char login[] = "{\"appKey\":\"0990c9b2da8b086fd5e980ba45b2d596\",\"appSecret\":\"7398b933041fd8334ffd7b930d7fa034\",\"appType\":\"2\"}";

void Get_CurrentData(char *data)
{
    strcpy(data,login);
}

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client))
            {
                resplen = evt->data_len;
                strcpy(respbuf, (char*)evt->data);
                respbuf[resplen] = '\0';
                // printf("%.*s\n", evt->data_len, (char*)evt->data);
            }
            else
            {
                resplen += evt->data_len;
                strcat(respbuf, (char*)evt->data);
                respbuf[resplen] = '\0';
                //printf("%.*s\n", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            // printf("%d %s\n",resplen, respbuf);
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}


static void httpPost()
{
    resplen = 0;
    memset(respbuf, 0, sizeof(respbuf));

    esp_http_client_config_t config =
    {
        .url = "http://irext.net",
        .event_handler = _http_event_handle,
        .buffer_size_tx = 1024,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    char post_data1[1024] = {0};
    Get_CurrentData(post_data1);
    //printf("%s\n",post_data1);

    esp_http_client_set_url(client, "http://irext.net/irext-server/app/app_login");
    esp_http_client_set_header(client,"Content-Type","application/json");
    esp_http_client_set_method(client, HTTP_METHOD_POST);    
    esp_http_client_set_post_field(client, post_data1, strlen(post_data1));

    esp_err_t err  = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI("http", "HTTP POST Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
        // int len =  esp_http_client_get_content_length(client);
        // int read_len = 0;
        // char buf[1024] = {0};
        // read_len = esp_http_client_read(client, buf, sizeof(buf));
        // printf("\nrecv data len:%d %d %s\n",read_len,len,buf);
    }
    else
    {
        ESP_LOGE("http", "HTTP POST request failed: %s", esp_err_to_name(err));
    }
}

void httpPostTask(void *pvParameters)
{
    ESP_LOGI("http", "Connected to AP, begin http example");
    httpPost();
    vTaskDelete(NULL);
}
