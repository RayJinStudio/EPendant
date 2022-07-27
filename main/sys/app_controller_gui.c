#include "app_controller_gui.h"
#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #include "lvgl.h"

// 必须定义为全局或者静态
static lv_obj_t *app_scr = NULL;
static lv_obj_t *pre_app_image = NULL;
static lv_obj_t *pre_app_name = NULL;
static lv_obj_t *now_app_image = NULL;
static lv_obj_t *now_app_name = NULL;
const void *pre_img_path = NULL;

static lv_style_t default_style;
static lv_style_t app_name_style;

LV_FONT_DECLARE(lv_font_montserrat_24);

void app_control_gui_init(void)
{
    if (NULL != app_scr)
    {
        lv_obj_clean(app_scr);
        app_scr = NULL;
    }

    lv_style_init(&default_style);
    lv_style_set_bg_color(&default_style, lv_color_hex(0xffffff));
    // lv_style_set_bg_color(&default_style, lv_color_hex(0x505050));
    // lv_style_set_bg_color(&default_style, lv_color_hex(0x0));
    // lv_style_set_bg_color(&default_style, lv_color_hex(0xf88));

    lv_style_init(&app_name_style);
    lv_style_set_text_opa(&app_name_style, LV_OPA_COVER);
    // lv_style_set_text_color(&app_name_style, lv_color_hex(0xffffff));
    // lv_style_set_text_font(&app_name_style, &lv_font_montserrat_24);
    
    // APP图标页
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    app_scr = lv_obj_create(NULL);
    lv_obj_add_style(app_scr, &default_style, LV_PART_MAIN);
    
}

void app_control_gui_release(void)
{
    if (NULL != app_scr)
    {
        lv_obj_clean(app_scr);
        app_scr = NULL;
    }
}

void display_app_scr_init(const void *src_img_path, const char *app_name)
{
    lv_obj_t *act_obj = lv_scr_act(); // 获取当前活动页
    if (act_obj == app_scr)
    {
        // 防止一些不适用lvgl的APP退出 造成画面在无其他动作情况下无法绘制更新
        lv_scr_load_anim(app_scr, LV_SCR_LOAD_ANIM_NONE, 300, 300, false);
        return;
    }
    if(act_obj!=NULL) lv_obj_clean(act_obj); // 清空此前页面
    pre_app_image = lv_img_create(app_scr);
    lv_img_set_src(pre_app_image, src_img_path);
    lv_obj_set_size(pre_app_image, IMGSIZE, IMGSIZE); 
    pre_img_path = src_img_path;

    lv_obj_align(pre_app_image, LV_ALIGN_CENTER, 0, -20);

    // 添加APP的名字
    pre_app_name = lv_label_create(app_scr);
    lv_obj_add_style(pre_app_name, &app_name_style, LV_PART_MAIN);
    lv_label_set_recolor(pre_app_name, true); //先得使能文本重绘色功能
    lv_label_set_text(pre_app_name, app_name);
    lv_obj_align(pre_app_name, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_scr_load_anim(app_scr, LV_SCR_LOAD_ANIM_NONE, 300, 300, false);
    //ANIEND
    if(app_scr!=NULL)
    {
        lv_scr_load(app_scr);
    }
}

void app_control_display_scr(const void *src_img, const char *app_name, lv_scr_load_anim_t anim_type, bool force)
{
    // force为是否强制刷新页面 true为强制刷新
    if (true == force)
    {
        display_app_scr_init(src_img, app_name);
        return;
    }

    if (src_img == pre_img_path)
    {
        return;
    }

    pre_img_path = src_img;
    int now_start_x;
    int now_end_x;
    int old_start_x;
    int old_end_x;
    if (LV_SCR_LOAD_ANIM_MOVE_LEFT == anim_type)
    {
        now_start_x = -IMGSIZE;
        now_end_x = IMGSTART;
        old_start_x = IMGSTART;
        old_end_x = LV_HOR_RES_MAX;
    }
    else
    {
        now_start_x = LV_HOR_RES_MAX;
        now_end_x = IMGSTART;
        old_start_x = IMGSTART;
        old_end_x = -IMGSIZE;
    }

    now_app_image = lv_img_create(app_scr);
    lv_img_set_src(now_app_image, src_img);

    lv_obj_set_size(now_app_image, IMGSIZE, IMGSIZE); 

    lv_obj_align(now_app_image, LV_ALIGN_LEFT_MID, 0, -20);
    // 添加APP的名字
    now_app_name = lv_label_create(app_scr);
    lv_obj_add_style(now_app_name, &app_name_style, LV_PART_MAIN);
    lv_label_set_recolor(now_app_name, true); //先得使能文本重绘色功能
    lv_label_set_text(now_app_name, app_name);
    // 删除原先的APP name
    lv_obj_del(pre_app_name);
    pre_app_name = now_app_name;
    lv_obj_align(now_app_name, LV_ALIGN_BOTTOM_MID, 0, -10);

    // lv_anim_path_cb_t path;
    // lv_anim_path_step(&path);
    // /*
    // lv_anim_path_linear lv_anim_path_bounce
    // lv_anim_path_overshoot lv_anim_path_ease_out
    // lv_anim_path_step
    // */
    // path = lv_anim_path_ease_out;
    // /*Optional for custom functions*/
    // // lv_anim_path_set_user_data(&path, &foo);

    lv_anim_t now_app;
    lv_anim_init(&now_app);
    lv_anim_set_exec_cb(&now_app, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&now_app, now_app_image);
    lv_anim_set_values(&now_app, now_start_x, now_end_x);
    uint32_t duration = lv_anim_speed_to_time(300, now_start_x, now_end_x); // 计算时间
    lv_anim_set_time(&now_app, duration);
    lv_anim_set_path_cb(&now_app, lv_anim_path_ease_out); // Default is linear

    lv_anim_t pre_app;
    lv_anim_init(&pre_app);
    lv_anim_set_exec_cb(&pre_app, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&pre_app, pre_app_image);
    lv_anim_set_values(&pre_app, old_start_x, old_end_x);
    duration = lv_anim_speed_to_time(300, old_start_x, old_end_x); // 计算时间
    lv_anim_set_time(&pre_app, duration);
    lv_anim_set_path_cb(&pre_app, lv_anim_path_ease_out); // Default is linear
    lv_anim_start(&now_app);
    lv_anim_start(&pre_app);
    // ANIEND
    vTaskDelay(300/portTICK_RATE_MS);

    lv_obj_del(pre_app_image); // 删除原先的图像
    pre_app_image = now_app_image;
}