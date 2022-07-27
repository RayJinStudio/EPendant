#ifndef COMMON_H
#define COMMON_H

void delay(int ms);

enum ACTIVE_TYPE
{
    TURN_RIGHT = 0,
    RETURN,
    TURN_LEFT,
    UP,
    DOWN,
    GO_FORWORD,
    SHAKE,
    UNKNOWN
};


struct UserAction
{
    volatile ACTIVE_TYPE active;
    bool isValid;
    // boolean long_time;
    // int16_t v_ax; // v表示虚拟参数（用于调整6050的初始方位）
    // int16_t v_ay;
    // int16_t v_az;
    // int16_t v_gx;
    // int16_t v_gy;
    // int16_t v_gz;
};

#endif