#include "zf_common_headfile.h"

// PID控制器实例
PID_Controller pid_x, pid_y, pid_z;

// 初始化PID参数
void PID_Init_All(void) {
    // X轴（快速响应）
    pid_x.Kp = 1.2f; pid_x.Ki = 0.04f; pid_x.Kd = 0.1f;
    pid_x.integral_limit = 30.0f; pid_x.output_limit = 10.0f;

    // Y轴（平滑移动）
    pid_y.Kp = 0.8f; pid_y.Ki = 0.03f; pid_y.Kd =0.2f;
    pid_y.integral_limit = 40.0f; pid_y.output_limit = 8.0f;

    // Z轴（精确控制）
    pid_z.Kp = 0.8f; pid_z.Ki = 0.05f; pid_z.Kd = 0.2f;
    pid_z.integral_limit = 20.0f; pid_z.output_limit = 5.0f;

    printf("[PID] Parameters initialized\n");
}
// 约束函数：确保值在 min 和 max 之间
float constrain(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
// PID计算（带抗饱和）
float PID_Calculate(PID_Controller* pid, float error) {
    // 比例项
    float P = pid->Kp * error;

    // 积分项（带抗饱和）
    if(fabs(error) < 10) { // 仅在小误差时积分
        pid->integral += error;
        pid->integral = constrain(pid->integral, -pid->integral_limit, pid->integral_limit);
    } else {
        pid->integral = 0; // 大误差时重置积分
    }
    float I = pid->Ki * pid->integral;

    // 微分项
    float D = pid->Kd * (error - pid->prev_error);
    pid->prev_error = error;

    // 总和输出
    float output = P + I + D;
    return constrain(output, -pid->output_limit, pid->output_limit);
}
