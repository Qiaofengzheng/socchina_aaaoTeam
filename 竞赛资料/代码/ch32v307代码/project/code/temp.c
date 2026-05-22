#include "zf_common_headfile.h"
// 定义温度传感器参数（需根据实际校准调整）
#define V25         1.43f    // 25℃时的电压（单位：V）
#define AVG_SLOPE   0.0043f  // 温度系数（单位：V/℃）

void ADC_TemperatureSensor_Init(void) {
    ADC_InitTypeDef ADC_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1时钟

    // ADC基础配置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;      // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;           // 单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;     // 单次转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                 // 1个通道
    ADC_Init(ADC1, &ADC_InitStructure);

    // 启用温度传感器（内部通道16）
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_Cmd(ADC1, ENABLE); // 使能ADC1

    // ADC校准（建议上电后执行一次）
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

float Read_Temperature(void) {
    uint16_t adc_value;
    float voltage, temperature;

    // 配置ADC通道（温度传感器 = 通道16）
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);      // 启动转换
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); // 等待转换完成
    adc_value = ADC_GetConversionValue(ADC1);    // 获取ADC值

    // 计算电压（假设Vdda = 3.3V）
    voltage = (float)adc_value * 3.3f / 4095.0f;

    // 计算温度（公式：T = (V - V25) / Avg_Slope + 25）
    temperature = (voltage - V25) / AVG_SLOPE + 25.0f;

    return temperature;
}

float Read_Temperature_Average(uint8_t samples) {
    float sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += Read_Temperature();
        system_delay_ms(20);
    }
    return sum / (float)samples;
}


