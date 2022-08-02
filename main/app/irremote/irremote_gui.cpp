#include "irremote_gui.h"
#include "irremote.h"

// #include "driver/lv_port_indev.h"
#include "lvgl.h"

#include "stdio.h"

#include "../../sys/wifi_controller.h"

// lv_obj_t *irremote_gui = NULL;

lv_obj_t * ui_Screen1;
lv_obj_t * addScreen;
lv_obj_t * ui_TextArea1;
lv_obj_t * ui_Keyboard2;
lv_obj_t * list1;
lv_group_t *group;
lv_group_t *group2;
extern lv_indev_t* indev_key;

static void add_power_event(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_KEY)
    {
        printf("hi\n");
        send();
    }
}

void irrmote_addScreen_init(void)
{
    group  = lv_group_create();
    lv_indev_set_group(indev_key, group);

    addScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(addScreen, LV_OBJ_FLAG_SCROLLABLE);


    lv_obj_t *power = lv_btn_create(addScreen);
    // lv_obj_remove_style_all(power);
    lv_obj_align(power, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_size(power,60,60);
    lv_obj_add_event_cb(power, add_power_event, LV_EVENT_KEY, NULL);
    lv_obj_set_style_bg_color(power,  LV_COLOR_MAKE(0x00,0x00,0xff), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(power,30, LV_STATE_DEFAULT);
    lv_group_add_obj(group ,power);

    lv_obj_t *power_label = lv_label_create(power);
    lv_obj_align(power_label,LV_ALIGN_CENTER,0,0);
    lv_obj_set_size(power_label,40,40);
    lv_label_set_text(power_label, "power");


    lv_obj_t *wrong = lv_btn_create(addScreen);
    lv_obj_align(wrong, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_set_size(wrong,40,40);
    lv_obj_add_event_cb(wrong, add_power_event, LV_EVENT_KEY, NULL);
    lv_obj_set_style_bg_color(wrong, lv_palette_lighten(LV_PALETTE_RED, 3), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(wrong,20, LV_STATE_DEFAULT);
    lv_group_add_obj(group ,wrong);

    lv_obj_t *wrong_label = lv_label_create(wrong);
    lv_obj_align(wrong_label,LV_ALIGN_CENTER,0,0);
    //lv_obj_set_size(wrong_label,30,30);
    lv_label_set_text(wrong_label, "X");



    lv_obj_t *correct = lv_btn_create(addScreen);
    lv_obj_align(correct, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_set_size(correct,40,40);
    lv_obj_add_event_cb(correct, add_power_event, LV_EVENT_KEY, NULL);
    lv_obj_set_style_bg_color(correct, lv_palette_lighten(LV_PALETTE_RED, 3), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(correct,20, LV_STATE_DEFAULT);
    lv_group_add_obj(group ,correct);

    lv_obj_t *correct_label = lv_label_create(correct);
    lv_obj_align(correct_label,LV_ALIGN_CENTER,0,0);
    //lv_obj_set_size(correct_label,30,30);
    lv_label_set_text(correct_label, "Y");

    lv_disp_load_scr(addScreen);

}

void irremote_gui_init(void)
{ 
     // lv_style_init(&default_style);
    // lv_disp_t * dispp = lv_disp_get_default();
    // lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
    //                                            false, LV_FONT_DEFAULT);
    // lv_disp_set_theme(dispp, theme);

    // ui_Screen1 = lv_obj_create(NULL);
    // lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

    // lv_obj_t *label = lv_label_create(ui_Screen1);
    // lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 5);
    // lv_obj_set_size(label, 60, 20);
    // lv_label_set_text(label, "hello lvgl");

    // lv_disp_load_scr(ui_Screen1);
    
    irrmote_addScreen_init();
}

/*
 * 其他函数请根据需要添加
 */

void display_irremote(const char *file_name, lv_scr_load_anim_t anim_type)
{
}

void irremote_gui_del(void)
{
    if (NULL != ui_Screen1)
    {
        lv_obj_clean(ui_Screen1);
        ui_Screen1 = NULL;
    }
    if (NULL != addScreen)
    {
        lv_obj_clean(addScreen);
        addScreen = NULL;
    }
    
    // 手动清除样式，防止内存泄漏
    // lv_style_reset(&default_style);
}