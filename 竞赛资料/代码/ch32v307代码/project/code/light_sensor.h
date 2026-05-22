#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#include "zf_common_headfile.h"

// 光敏电阻参数（需根据实际测量校准）
#define LIGHT_MIN_ADC     500    // 黑暗时的ADC值（需实测）
#define LIGHT_MAX_ADC     3000   // 强光时的ADC值（需实测）
#define LIGHT_MAX_LUX     2000   // 最大光照强度（单位：Lux）

void LightSensor_Init(void);              // 初始化ADC
uint16_t Read_LightSensor_ADC(void);      // 读取ADC原始值
uint16_t Read_LightSensor_Lux(void);      // 转换为光照强度（Lux）
uint16_t Read_LightSensor_Lux_Average(uint8_t samples); // 多次采样取平均

#endif
