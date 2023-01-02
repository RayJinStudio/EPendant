#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "iostream"
#include "driver/gpio.h"

#include "esp_log.h"

#include "CST816T.h"
#include "../common/common.h"
#include "../sys/app_controller.h"
#include <cmath>

using namespace std;

static gpio_num_t i2c_gpio_sda = (gpio_num_t)4;
static gpio_num_t i2c_gpio_scl = (gpio_num_t)5;

#define I2C_NUM I2C_NUM_0

CST816T touch(i2c_gpio_scl, i2c_gpio_sda);

void touch_init()
{
    touch.begin();
}

extern AppController* appctrl;
bool pretouch = false;
uint16_t startx;
uint16_t starty;
uint16_t endx;
uint16_t endy;

const uint16_t deta = 50;

extern UserAction ua;

bool touch_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    TouchInfos tf = touch.GetTouchInfo();
    data->point.x = tf.x;
    data->point.y = tf.y;
    data->state = (tf.touching == false ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR);

    TouchInfos ntf;
    ntf.isValid = false;
    ntf.gesture = None;
    if(!pretouch&&tf.touching)
    {
        pretouch = 1;
        startx = tf.x;
        starty = tf.y;
    }
    else if(pretouch&&!tf.touching)
    {
        pretouch = 0;
        endx = tf.x;
        endy = tf.y;

        short disx = startx-endx;
        short disy = starty-endy;
        if(disx >= deta && disy <= deta && disy >= -deta)
        {
            ntf.isValid = true;
            ntf.gesture = SlideLeft;
        }
        else if(disx <= -deta && disy <= deta && disy >= -deta)
        {
            ntf.isValid = true;
            ntf.gesture = SlideRight;
        }
        else if(disy <= -deta && disx <= deta && disx >= -deta)
        {
            ntf.isValid = true;
            ntf.gesture = SlideDown;
        }
        else if(disy >= deta && disx <= deta && disx >= -deta)
        {
            ntf.isValid = true;
            ntf.gesture = SlideUp;
        }   
    }

    if(appctrl&&ntf.isValid)
    {
        // ua.isValid = 0;
        switch (tf.gesture)
        {
            case SlideDown:
                ua.active = DOWN;
                ua.isValid = 1;
                break;
            case SlideUp:
                ua.active = GO_FORWORD;
                ua.isValid = 1;
                break;
            case SlideRight:
                ua.active = TURN_RIGHT;
                ua.isValid = 1;
                break;
            case SlideLeft:
                ua.active = TURN_LEFT;
                ua.isValid = 1;
                break;
            default:
                break;
        }
    }

    return false;
}

