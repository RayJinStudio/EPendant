#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "driver/gpio.h"
#include "stdio.h"

static gpio_num_t KeyIO = (gpio_num_t)0;
static gpio_num_t KeyIO8 = (gpio_num_t)8;
static gpio_num_t KeyIO18 = (gpio_num_t)18;



void key_init()
{
    // gpio_reset_pin(KeyIO);
    // gpio_reset_pin(KeyIO4);
    // gpio_set_direction(KeyIO, GPIO_MODE_INPUT);
    // gpio_set_direction(KeyIO4, GPIO_MODE_INPUT);
}

uint32_t key_get_key(void)
{
    if(gpio_get_level(KeyIO)==0)
    {
        return 1;
    }
    else if(gpio_get_level(KeyIO8)==0)
    {
        return 2;
    }
    else if(gpio_get_level(KeyIO18)==0)
    {
        return 3;
    }
    return 0;
}


void key_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    uint32_t act_key = key_get_key();
    //printf("%d\n",act_key);
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
        //printf("%d\n",act_key);
        case 1:
            act_key = LV_KEY_ENTER;
            break;
        case 2:
            act_key = LV_KEY_RIGHT;
            break;
        case 3:
            act_key = LV_KEY_NEXT;
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

//something wrong
void lv_key_indev_init(lv_indev_t* &indev_key)
{
    key_init();	//初始化操作，对应你的平台 io 或 adc 检测
    lv_indev_drv_t indev_drv;
    /*Register a keypad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = key_read ;
    indev_key = lv_indev_drv_register(&indev_drv);
}


