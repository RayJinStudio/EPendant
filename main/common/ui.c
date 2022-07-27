// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
lv_obj_t * ui_TextArea1;
lv_obj_t * ui_Keyboard2;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=1
    #error "#error LV_COLOR_16_SWAP should be 1 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{
    extern lv_indev_t* indev_mpu6050;
    lv_group_t *group = lv_group_create();
    lv_indev_set_group(indev_mpu6050, group);
    // ui_Screen1

    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

    // ui_TextArea1

    ui_TextArea1 = lv_textarea_create(ui_Screen1);

    lv_obj_set_width(ui_TextArea1, 99);
    lv_obj_set_height(ui_TextArea1, 70);

    lv_obj_set_x(ui_TextArea1, 2);
    lv_obj_set_y(ui_TextArea1, -38);

    lv_obj_set_align(ui_TextArea1, LV_ALIGN_CENTER);

    lv_textarea_set_accepted_chars(ui_TextArea1, NULL);
    //else lv_textarea_set_accepted_chars(ui_TextArea1, "");

    lv_textarea_set_text(ui_TextArea1, "");
    lv_textarea_set_placeholder_text(ui_TextArea1, "Placeholder...");

    // ui_Keyboard2

    ui_Keyboard2 = lv_keyboard_create(ui_Screen1);

    lv_obj_set_width(ui_Keyboard2, 126);
    lv_obj_set_height(ui_Keyboard2, 82);

    lv_obj_set_x(ui_Keyboard2, -1);
    lv_obj_set_y(ui_Keyboard2, 36);

    lv_obj_set_align(ui_Keyboard2, LV_ALIGN_CENTER);

    // POST CALLS
    lv_keyboard_set_textarea(ui_Keyboard2, ui_TextArea1);

    lv_group_add_obj(group ,ui_TextArea1);
    lv_group_add_obj(group , ui_Keyboard2);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    lv_disp_load_scr(ui_Screen1);
}

