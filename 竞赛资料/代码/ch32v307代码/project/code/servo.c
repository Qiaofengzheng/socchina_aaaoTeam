#include "zf_common_headfile.h"


// 初始化所有舵机PWM
void Servo_PWM_Init_All(void) {
    // 50Hz PWM (周期20ms), 初始占空比1.5ms (中位)
    pwm_init(SERVO_X_PWM, 50, 1500 * PWM_DUTY_MAX / 20000);
    pwm_init(SERVO_Y_PWM, 50, 1500 * PWM_DUTY_MAX / 20000);
    pwm_init(SERVO_Z_PWM, 50, 1500 * PWM_DUTY_MAX / 20000);

    printf("[PWM] Initialized (50Hz)\n");
}

// 设置舵机角度 (0-180°)
void Servo_Set_Angle(Servo_Channel ch, uint8_t angle) {
    angle = angle > 180 ? 180 : angle; // 安全限制

    uint32_t pulse_us = 500 + angle * 2000 / 180;  // 0.5ms-2.5ms
    uint32_t duty = pulse_us * PWM_DUTY_MAX / 20000;

    switch(ch) {
        case SERVO_X: pwm_set_duty(SERVO_X_PWM, duty); break;
        case SERVO_Y: pwm_set_duty(SERVO_Y_PWM, duty); break;
        case SERVO_Z: pwm_set_duty(SERVO_Z_PWM, duty); break;
    }

}
