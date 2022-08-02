#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_tls.h"
#include "irremote_http.hpp"

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#include "esp_http_client.h"

#define TAG "IRREMOTE"

int resplen;
char respbuf[1024];

void (*postCb) (int, char*);

void initPostEntitry(HttpPostEntity* entity)
{
    entity->url = (char*)malloc(1024 * sizeof(char));
    entity->body = (char*)malloc(1024 * sizeof(char));
    entity->postCb = NULL;
}

void setHttpPostURL(HttpPostEntity* entity, char* s)
{
    strcpy(entity->url,s);
}

void setHttpPostBody(HttpPostEntity* entity, char* s)
{
    strcpy(entity->body,s);
}

void setHttpPostCb(HttpPostEntity* entity, void (*cb) (int, char*))
{
    postCb = cb;
    entity->postCb = cb;
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
            }
            else
            {
                resplen += evt->data_len;
                strcat(respbuf, (char*)evt->data);
                respbuf[resplen] = '\0';
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


static void httpPost(HttpPostEntity* entity)
{
    resplen = 0;
    memset(respbuf, 0, sizeof(respbuf));

    printf("%s %s\n",entity->url,entity->body);

    esp_http_client_config_t config =
    {
        .url = entity->url,
        .event_handler = _http_event_handle,
        .buffer_size_tx = 1024,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client,"Content-Type","application/json");
    esp_http_client_set_method(client, HTTP_METHOD_POST);    
    esp_http_client_set_post_field(client, entity->body, strlen(entity->body));

    esp_err_t err  = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI("http", "HTTP POST Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
        if(postCb==entity->postCb) entity->postCb(resplen, respbuf);
        else
        {
            entity->postCb = postCb;
            printf("error\n");
            entity->postCb(resplen, respbuf);
        }
    }
    else
    {
        ESP_LOGE("http", "HTTP POST request failed: %s", esp_err_to_name(err));
    }
}

void httpPostTask(void *entity)
{
    ESP_LOGI("http", "Connected to AP, begin http example");
    httpPost((HttpPostEntity*)entity);
    vTaskDelete(NULL);
}
