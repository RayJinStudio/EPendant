#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "iostream"
#include "driver/gpio.h"

#include "esp_log.h"

#include "mpu6050.hpp"
#include "kalmanfilter.hpp"
#include <cmath>

using namespace std;

static gpio_num_t i2c_gpio_sda = (gpio_num_t)4;
static gpio_num_t i2c_gpio_scl = (gpio_num_t)5;

#define I2C_NUM I2C_NUM_0

float ax,ay,az,gx,gy,gz;
float pitch, roll;
float fpitch, froll;

MPU6050 mpu(i2c_gpio_scl, i2c_gpio_sda, I2C_NUM);

void mpu6050_init()
{
    // gpio_reset_pin(KeyIO);
    // gpio_reset_pin(KeyIO4);
    // gpio_set_direction(KeyIO, GPIO_MODE_INPUT);
    // gpio_set_direction(KeyIO4, GPIO_MODE_INPUT);
    if(!mpu.init())
    {
	    ESP_LOGE("mpu6050", "init failed!");
        vTaskDelete(0);
    }
	ESP_LOGI("mpu6050", "init success!");
}

uint32_t mpu6050_get_key(void)
{
    ax = -mpu.getAccX();
    ay = -mpu.getAccY();
    az = -mpu.getAccZ();
    gx = mpu.getGyroX();
    gy = mpu.getGyroY();
    gz = mpu.getGyroZ();
    pitch = atan(ax/az)*57.2958;
    roll = atan(ay/az)*57.2958;

    if(pitch>30) return 1;
    else if(roll>45) return 2;
    else if(roll<-45) return 3;
    return 0;
}



void mpu6050_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    uint32_t act_key = mpu6050_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) 
        {
        case 1:
            act_key = LV_KEY_ENTER;
            break;
        case 2:
            act_key = LV_KEY_RIGHT;
            break;
        case 3:
            act_key = LV_KEY_LEFT;
            break;
        case 4:
            act_key = LV_KEY_RIGHT;
            break;
        case 5:
            act_key = LV_KEY_ENTER;
            break;
      	case 6:
		    act_key = LV_KEY_HOME;
            break;
        }

        last_key = act_key;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;
}

/*Get the currently being pressed key.  0 if no key is pressed*/

//移植中添加keypad相关的初始化以及正确填充 indev_drv 这个结构体。
//好像有问题
void lv_mpu6050_indev_init()
{
    extern lv_indev_t* indev_mpu6050;
   	 /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    mpu6050_init();	//初始化操作，对应你的平台 io 或 adc 检测
    lv_indev_drv_t indev_drv;
    /*Register a keypad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = mpu6050_read;
    indev_mpu6050 = lv_indev_drv_register(&indev_drv);
}
