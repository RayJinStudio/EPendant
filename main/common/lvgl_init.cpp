#include "lvgl_init.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "../sys/app_controller.h"

#include "../Rdriver/lv_mpu6050_driver.hpp"
#include "../Rdriver/lv_key_driver.hpp"
#include "../Rdriver/touch_driver.h"

#include "ui.h"
LV_FONT_DECLARE(song_font_10);

#define CONFIG_LV_MPU6050_CONTROLLER 0
#define CONFIG_LV_KEY_CONTROLLER 1

#if CONFIG_LV_MPU6050_CONTROLLER
    lv_indev_t* indev_mpu6050;
#endif

#if CONFIG_LV_KEY_CONTROLLER
    lv_indev_t* indev_key;
#endif

lv_obj_t *label_1;
lv_obj_t *label_2;
lv_obj_t *label;
lv_obj_t *label3;
lv_obj_t *btn1;
lv_obj_t *btn2;

void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}
/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

/*********************** GUI_SHOW_CODE_START***********************/

int cnt1 = 0;

static void event_handler(lv_event_t *event)
{
    // cnt1 = (cnt1+1)%10;
    lv_label_set_text_fmt(label, "%d", cnt1);
}

int cnt2 = 0;

static void event_handler2(lv_event_t *event)
{
    extern AppController* appctrl;
    // // printf("%d\n",cnt1);
    // appctrl->app_start(cnt1);
}

void Show_State()
{
    lv_obj_t *scr = lv_scr_act(); //创建scr
    lv_group_t *group = lv_group_create();
	//lv_indev_set_group(indev_mpu6050, group);
    lv_obj_set_pos(scr, 0, 0);
    lv_scr_load(scr);

    label_1 = lv_label_create(scr);                         //创建label
    lv_label_set_recolor(label_1, 1);                       //颜色可变换
    lv_label_set_long_mode(label_1, LV_LABEL_LONG_DOT);     //设置滚动模式
    lv_obj_set_pos(label_1, 10, 10);                        //设置位置
    lv_obj_set_size(label_1, 160, 30);                      //设定大小
    lv_label_set_text(label_1, "This is a GUI thread yes"); //设定文本内容

    label_2 = lv_label_create(scr);                            //创建label
    lv_label_set_recolor(label_2, 1);                          //颜色可变换
    lv_label_set_long_mode(label_2, LV_LABEL_LONG_SCROLL);     //设置滚动模式
    lv_obj_set_pos(label_2, 10, 40);                           //设置位置
    lv_obj_set_size(label_2, 100, 40);                         //设定大小
    lv_label_set_text(label_2, "This is the Intetnet thread"); //设定文本内容

    btn1 = lv_btn_create(scr);
    lv_obj_set_size(btn1, 50, 40); // 设置对象大小,宽度和高度
    lv_obj_set_pos(btn1, 2, 90);
    label = lv_label_create(btn1);
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_KEY, NULL);
    lv_label_set_text(label, "按钮"); // 设置显示内容
    lv_obj_set_style_text_font(label, &song_font_10, 0);
    lv_obj_center(label);
    btn2 = lv_btn_create(scr);
    lv_obj_set_size(btn2, 50, 40); // 设置对象大小,宽度和高度
    lv_obj_set_pos(btn2, 73, 90);
    label3 = lv_label_create(btn2);
    lv_obj_add_event_cb(btn2, event_handler2, LV_EVENT_KEY, NULL);
    lv_label_set_text(label3, "Click me!"); // 设置显示内容
    lv_obj_center(label3);

    lv_group_add_obj(group ,btn1);
    lv_group_add_obj(group ,btn2);
}
/*********************** GUI_SHOW_CODE_END***********************/

void guiTask1(void *pvParameter)
{

    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();
    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    void *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    // memset(buf1,0x00ff,DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    void *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //memset(buf2,0x00ff,DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_draw_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820 || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if 1
    touch_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#elif CONFIG_LV_MPU6050_CONTROLLER
    mpu6050_init();

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = mpu6050_read;
    indev_mpu6050 = lv_indev_drv_register(&indev_drv);
#endif

#if CONFIG_LV_KEY_CONTROLLER
    key_init();

    lv_indev_drv_t indev_drv2;
    lv_indev_drv_init(&indev_drv2);
    indev_drv2.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv2.read_cb = key_read;
    indev_key = lv_indev_drv_register(&indev_drv2);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
    //ui_init();
    //Show_State();
    // lv_demo_music();
    // lv_demo_benchmark();
    // lv_demo_widgets();
    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
    vTaskDelete(NULL);
}
