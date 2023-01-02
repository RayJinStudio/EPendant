#include "irremote_gui.h"
#include "irremote.h"

// #include "driver/lv_port_indev.h"
#include "lvgl.h"

#include "stdio.h"

#include "../../sys/wifi_controller.h"

LV_FONT_DECLARE(song_font_10);

// lv_obj_t *irremote_gui = NULL;

lv_obj_t * mainScreen;
lv_obj_t * addScreen;
lv_obj_t * categoryScreen;

lv_obj_t * categoryRoller;
lv_obj_t * categoryList;



static void categoryRoller_handler(lv_event_t* event)
{
    lv_obj_t * obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        char buf[32];
        int id = lv_roller_get_selected(obj);
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        printf("Selected: %d %s\n",id, buf);
    }
}

static void categoryNext_event(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if(code == LV_EVENT_CLICKED)
    {
        printf("hi2\n");
    }
}

void setcategoryRoller(char *str)
{
    printf("%s\n",str);
    // lv_roller_set_options(categoryRoller,str,LV_ROLLER_MODE_INFINITE);
    if(!categoryList) printf("hi2\n");
    lv_obj_t * list_btn;
	list_btn = lv_list_add_btn(categoryList, NULL, str);
    printf("hi\n");
    //lv_obj_add_event_cb(list_btn, categoryNext_event, LV_EVENT_CLICKED, NULL);
}

void irremote_category_init(void)
{ 
    categoryScreen = lv_obj_create(NULL);
    // lv_obj_clear_flag(categoryScreen, LV_OBJ_FLAG_SCROLLABLE);



    categoryList = lv_list_create(categoryScreen);
    lv_obj_align(categoryList, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(categoryList,200,240);

    lv_obj_t *list_btn;
    list_btn = lv_list_add_btn(categoryList, LV_SYMBOL_FILE, "hi");
    // lv_obj_set_style_text_font(categoryList, &song_font_10, LV_STATE_DEFAULT);
    // lv_obj_t *nextBtn = lv_btn_create(categoryScreen);
    // // lv_obj_remove_style_all(nextbtn);
    // lv_obj_align(nextBtn, LV_ALIGN_RIGHT_MID, -15, 0);
    // lv_obj_set_size(nextBtn,60,60);

    // categoryRoller = lv_roller_create(categoryScreen);
    // lv_obj_set_style_text_font(categoryScreen, &song_font_10, 0);
    // lv_roller_set_visible_row_count(categoryRoller, 6);
    // lv_obj_align(categoryRoller, LV_ALIGN_LEFT_MID, 15, 0);
    // lv_obj_add_event_cb(categoryRoller, categoryRoller_handler, LV_EVENT_ALL, NULL);

    
    // lv_obj_add_event_cb(nextBtn, categoryNext_event, LV_EVENT_ALL, NULL);
     //lv_obj_set_style_bg_color(next,  LV_COLOR_MAKE(0x00,0x00,0xff), LV_STATE_DEFAULT);
    //lv_obj_set_style_radius(nextBtn, 40, LV_STATE_DEFAULT);

    // lv_obj_t *next_label = lv_label_create(nextBtn);
    // lv_obj_align(next_label,LV_ALIGN_CENTER,0,0);
    // // lv_obj_set_size(next_label,10,10);
    // lv_label_set_text(next_label, ">");
    // lv_obj_set_style_text_color(next_label, LV_COLOR_MAKE(0xff,0xff,0xff), LV_STATE_DEFAULT);

    lv_disp_load_scr(categoryScreen);
}


static void add_power_event(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_CLICKED)
    {
        send();
    }
}

void irrmote_addScreen_init(void)
{
    addScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(addScreen, LV_OBJ_FLAG_SCROLLABLE);


    lv_obj_t *power = lv_btn_create(addScreen);
    // lv_obj_remove_style_all(power);
    lv_obj_align(power, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_size(power,60,60);
    lv_obj_add_event_cb(power, add_power_event, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(power,  LV_COLOR_MAKE(0x00,0x00,0xff), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(power,30, LV_STATE_DEFAULT);

    lv_obj_t *power_label = lv_label_create(power);
    lv_obj_align(power_label,LV_ALIGN_CENTER,0,0);
    lv_obj_set_size(power_label,40,40);
    lv_label_set_text(power_label, "power");


    lv_obj_t *wrong = lv_btn_create(addScreen);
    lv_obj_align(wrong, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_set_size(wrong,40,40);
    //lv_obj_add_event_cb(wrong, add_power_event, LV_EVENT_KEY, NULL);
    lv_obj_set_style_bg_color(wrong, lv_palette_lighten(LV_PALETTE_RED, 3), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(wrong,20, LV_STATE_DEFAULT);

    lv_obj_t *wrong_label = lv_label_create(wrong);
    lv_obj_align(wrong_label,LV_ALIGN_CENTER,0,0);
    //lv_obj_set_size(wrong_label,30,30);
    lv_label_set_text(wrong_label, "X");



    lv_obj_t *correct = lv_btn_create(addScreen);
    lv_obj_align(correct, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_set_size(correct,40,40);
    //lv_obj_add_event_cb(correct, add_power_event, LV_EVENT_KEY, NULL);
    lv_obj_set_style_bg_color(correct, lv_palette_lighten(LV_PALETTE_RED, 3), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(correct,20, LV_STATE_DEFAULT);

    lv_obj_t *correct_label = lv_label_create(correct);
    lv_obj_align(correct_label,LV_ALIGN_CENTER,0,0);
    //lv_obj_set_size(correct_label,30,30);
    lv_label_set_text(correct_label, "Y");

    lv_disp_load_scr(addScreen);

}

static void add_event(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_CLICKED)
    {
        getToken();
        if (NULL != mainScreen)
        {
            lv_obj_clean(mainScreen);
            mainScreen = NULL;
        }
        irremote_category_init();
    }
}


void irremote_gui_init(void)
{ 
    mainScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(mainScreen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *add = lv_btn_create(mainScreen);
    // lv_obj_remove_style_all(power);
    lv_obj_align(add, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_size(add,200,80);
    lv_obj_add_event_cb(add, add_event, LV_EVENT_ALL, NULL);
    //lv_obj_set_style_bg_color(power,  LV_COLOR_MAKE(0x00,0x00,0xff), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(add,15, LV_STATE_DEFAULT);

    lv_obj_t *add_label = lv_label_create(add);
    lv_obj_align(add_label,LV_ALIGN_CENTER,0,0);
    lv_obj_set_size(add_label,40,40);
    lv_label_set_text(add_label, "Add");

    lv_obj_t *my = lv_btn_create(mainScreen);
    // lv_obj_remove_style_all(power);
    lv_obj_align(my, LV_ALIGN_TOP_MID, 0, 140);
    lv_obj_set_size(my,200,80);
    //lv_obj_add_event_cb(my, add_power_event, LV_EVENT_KEY, NULL);
    //lv_obj_set_style_bg_color(power,  LV_COLOR_MAKE(0x00,0x00,0xff), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(my,15, LV_STATE_DEFAULT);

    lv_obj_t *my_label = lv_label_create(my);
    lv_obj_align(my_label,LV_ALIGN_CENTER,0,0);
    lv_obj_set_size(my_label,40,40);
    lv_label_set_text(my_label, "My");


    lv_disp_load_scr(mainScreen);
}

/*
 * 其他函数请根据需要添加
 */

void display_irremote(const char *file_name, lv_scr_load_anim_t anim_type)
{
}

void irremote_gui_del(void)
{
    if (NULL != mainScreen)
    {
        lv_obj_clean(mainScreen);
        mainScreen = NULL;
    }
    if (NULL != addScreen)
    {
        lv_obj_clean(addScreen);
        addScreen = NULL;
    }
    
    // 手动清除样式，防止内存泄漏
    // lv_style_reset(&default_style);
}