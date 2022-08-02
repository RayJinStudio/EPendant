set(srcs
        "common/ui.c"
        "common/ui_helpers.c"
        "common/song_font_10.c"
        "common/lvgl_init.cpp"
        "common/common.cpp"

        "Rdriver/lv_mpu6050_driver.cpp"
        "Rdriver/lv_key_driver.cpp"
        "Rdriver/mpu6050.cpp"
        "Rdriver/kalmanfilter.cpp"
        "Rdriver/i2c.cpp"

        "sys/app_controller.cpp"
        "sys/app_controller_gui.c"
        "sys/app_loading.c"
        "sys/wifi_controller.cpp"

        "app/example/example_gui.c"
        "app/example/example_ico.c"
        "app/example/example.cpp"

        "app/game_2048/game_2048_gui.c"
        "app/game_2048/game_2048_ico.c"
        "app/game_2048/game_2048.cpp"
        "app/game_2048/game2048_contorller.cpp"

        "app/irremote/irremote_gui.cpp"
        "app/irremote/irremote_ico.c"
        "app/irremote/irremote.cpp"
        "app/irremote/irremote_http.cpp"
        
        "main.cpp"
    )
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "")