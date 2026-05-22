#ifndef __SERVO_H
#define __SERVO_H


#include "zf_common_headfile.h"

// 舵机通道定义
typedef enum {
    SERVO_X = 0,  // X轴
    SERVO_Y,      // Y轴
    SERVO_Z       // Z轴
} Servo_Channel;

// PWM通道配置（根据实际硬件连接修改）
#define SERVO_X_PWM   TIM2_PWM_MAP0_CH1_A0
#define SERVO_Y_PWM   TIM2_PWM_MAP0_CH2_A1
#define SERVO_Z_PWM   TIM2_PWM_MAP0_CH3_A2

// 函数声明
void Servo_PWM_Init_All(void);
void Servo_Set_Angle(Servo_Channel ch, uint8_t angle);

#endif
