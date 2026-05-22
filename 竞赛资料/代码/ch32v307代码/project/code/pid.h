#ifndef __PID_H
#define __PID_H


#include "zf_common_headfile.h"

// PID控制器结构体
typedef struct {
    float Kp, Ki, Kd;
    float integral;
    float prev_error;
    float integral_limit;
    float output_limit;
} PID_Controller;

// 全局PID实例
extern PID_Controller pid_x, pid_y, pid_z;

// 函数声明
void PID_Init_All(void);
float PID_Calculate(PID_Controller* pid, float error);
float constrain(float value, float min, float max) ;
#endif
