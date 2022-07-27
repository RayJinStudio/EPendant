#include "example_gui.h"

// #include "driver/lv_port_indev.h"
#include "lvgl.h"

#include "stdio.h"

#include "../../sys/wifi_controller.h"

// lv_obj_t *example_gui = NULL;

lv_obj_t * ui_Screen1;
lv_obj_t * ui_Screen2;
lv_obj_t * ui_TextArea1;
lv_obj_t * ui_Keyboard2;
lv_obj_t * list1;
lv_group_t *group;
lv_group_t *group2;
char *connecting_ssid;

void del_connect_gui()
{
    extern lv_indev_t* indev_key;
    lv_indev_set_group(indev_key, NULL);
    // if (NULL != ui_Screen1)
    // {
    //     lv_obj_clean(ui_Screen1);
    //     ui_Screen1 = NULL;
    // }
    // if (NULL != group)
    // {
    //     lv_group_del(group);
    //     group = NULL;
    // }
}

esp_err_t ConnectWifi(char* pwd)
{
    wifi_config_t wifi_config;
    wifi_sta_config_t sta_config;
    strcpy((char*)(sta_config.ssid),connecting_ssid);
    strcpy((char*)(sta_config.password),pwd);
    wifi_config.sta = sta_config;
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_err_t err = esp_wifi_connect();
    ESP_ERROR_CHECK(err);
    return err;
}

static void kb_event_cb(lv_event_t* event)
{
    printf("hi\n");
    lv_keyboard_def_event_cb(event);
     lv_event_code_t code = lv_event_get_code(event);
   if(code ==  LV_EVENT_READY)
   {
        printf("hi\n");
        if(ConnectWifi(lv_textarea_get_text(ui_TextArea1))==ESP_OK)
            printf("success");


   }
}

void detail_gui_init(const char *ssid)
{
    connecting_ssid = ssid;
    ui_Screen2 = lv_obj_create(NULL);
    lv_group_t *group2 = lv_group_create();

    lv_obj_t *label = lv_label_create(ui_Screen2);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_size(label, 60, 20);
    lv_label_set_text(label, ssid);

    ui_TextArea1 = lv_textarea_create(ui_Screen2);
    lv_obj_set_width(ui_TextArea1, 100);
    lv_obj_set_height(ui_TextArea1, 30);
    lv_obj_align(ui_TextArea1, LV_ALIGN_TOP_MID, 0, 25);
    lv_textarea_set_accepted_chars(ui_TextArea1, NULL);
    lv_textarea_set_text(ui_TextArea1, "");
    lv_textarea_set_placeholder_text(ui_TextArea1, "Placeholder...");

    // ui_Keyboard2

    ui_Keyboard2 = lv_keyboard_create(ui_Screen2);

    lv_obj_set_width(ui_Keyboard2, 126);
    lv_obj_set_height(ui_Keyboard2, 82);

    lv_obj_set_x(ui_Keyboard2, -1);
    lv_obj_set_y(ui_Keyboard2, 36);
    lv_obj_set_align(ui_Keyboard2, LV_ALIGN_CENTER);

    // POST CALLS
    lv_keyboard_set_textarea(ui_Keyboard2, ui_TextArea1);
    lv_obj_add_event_cb(ui_Keyboard2, kb_event_cb, LV_EVENT_KEY, NULL);
    printf("hi1\n");

    lv_group_add_obj(group2 ,ui_Keyboard2);
     extern lv_indev_t* indev_key;
	lv_indev_set_group(indev_key, group2);
    
    lv_disp_load_scr(ui_Screen2);
}

void connect_gui()
{
    extern lv_indev_t* indev_key;
    group  = lv_group_create();
	lv_indev_set_group(indev_key, group);
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(ui_Screen1);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 5);
    lv_obj_set_size(label, 60, 20);
    lv_label_set_text(label, "WIFI");

    lv_obj_t *sw1 = lv_switch_create(ui_Screen1);
    lv_obj_align(sw1, LV_ALIGN_TOP_RIGHT, -10, 5);
    lv_obj_set_size(sw1, 50, 20);
    lv_group_add_obj(group ,sw1);

    list1 = lv_list_create(ui_Screen1);
    lv_obj_add_state(list1, LV_STATE_FOCUSED);
    lv_obj_set_pos(list1,5,30);
    lv_obj_set_size(list1, 118, 125);
    // lv_group_add_obj(group ,list1);
}

static void list_event_handler(lv_event_t* event)
{
    lv_obj_t * obj = lv_event_get_target(event);
	printf("Clicked: %s\n", lv_list_get_btn_text(list1,obj));
    del_connect_gui();
    detail_gui_init(lv_list_get_btn_text(list1,obj));
}

void addItem(char *ssid)
{
    lv_obj_t * list_btn;
	list_btn = lv_list_add_btn(list1, NULL, ssid);
    lv_obj_add_event_cb(list_btn, list_event_handler,LV_EVENT_KEY,NULL);
    lv_group_add_obj(group ,list_btn);
}


void example_gui_init(void)
{ 
     // lv_style_init(&default_style);
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    connect_gui();
    lv_disp_load_scr(ui_Screen1);
    
}

/*
 * 其他函数请根据需要添加
 */

void display_example(const char *file_name, lv_scr_load_anim_t anim_type)
{
}

void example_gui_del(void)
{
    if (NULL != ui_Screen1)
    {
        lv_obj_clean(ui_Screen1);
        ui_Screen1 = NULL;
    }

    
    // 手动清除样式，防止内存泄漏
    // lv_style_reset(&default_style);
}