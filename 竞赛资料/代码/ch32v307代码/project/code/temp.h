#ifndef __TEMP_H
#define __TEMP_H

#include "zf_common_headfile.h"

// 温度传感器参数（需根据实际校准调整）
#define V25         1.43f    // 25℃时的电压（单位：V）
#define AVG_SLOPE   0.0043f  // 温度系数（单位：V/℃）

// 函数声明
void ADC_TemperatureSensor_Init(void);    // 初始化ADC和温度传感器
float Read_Temperature(void);             // 读取当前温度（℃）
float Read_Temperature_Average(uint8_t samples); // 多次采样取平均值

#endif
