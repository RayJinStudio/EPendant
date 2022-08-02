#ifndef APP_CONTROLLER_GUI_H
#define APP_CONTROLLER_GUI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"
#include "lvgl_helpers.h"

#define IMGSIZE 80
#define IMGSTART (LV_HOR_RES_MAX-IMGSIZE)/2

#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler(); //等待动画完成

    void app_control_gui_init(void);
    void app_control_gui_release(void);
    void display_app_scr_release(void);
    void display_app_scr_init(const void *src_img, const char * app_name);
    void app_control_display_scr(const void *src_img, const char * app_name,
                                 lv_scr_load_anim_t anim_type, bool force);
    void memu_clean();

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"
    extern const lv_img_dsc_t app_loading;
    extern const lv_img_dsc_t app_loading1;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif