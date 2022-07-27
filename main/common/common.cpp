#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common.h"

void delay(int ms)
{
    vTaskDelay(ms/portTICK_RATE_MS);
}