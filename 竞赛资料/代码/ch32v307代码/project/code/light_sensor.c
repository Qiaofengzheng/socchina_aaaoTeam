#include "zf_common_headfile.h"
/**
  * @brief  初始化ADC（通道0，PA0引脚）
  */
void LightSensor_Init(void) {
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // 1. 使能ADC和GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 2. 配置PA0为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. ADC基础配置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 4. 使能ADC并校准
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

/**
  * @brief  读取光敏电阻ADC原始值
  * @return ADC值（0~4095）
  */
uint16_t Read_LightSensor_ADC(void) {
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

/**
  * @brief  将ADC值转换为光照强度（Lux）
  * @return 光照强度（0~LIGHT_MAX_LUX）
  */
uint16_t Read_LightSensor_Lux(void) {
    uint16_t adc_value = Read_LightSensor_ADC();
    // 线性映射：ADC值 → Lux
    if (adc_value <= LIGHT_MIN_ADC) return 0;
    if (adc_value >= LIGHT_MAX_ADC) return LIGHT_MAX_LUX;
    return (uint16_t)((adc_value - LIGHT_MIN_ADC) * LIGHT_MAX_LUX / (LIGHT_MAX_ADC - LIGHT_MIN_ADC));
}

/**
  * @brief  多次采样取平均值
  * @param  samples: 采样次数
  * @return 平均光照强度（Lux）
  */
uint16_t Read_LightSensor_Lux_Average(uint8_t samples) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += Read_LightSensor_Lux();
        system_delay_ms(10);  // 间隔10ms减少噪声
    }
    return (uint16_t)(sum / samples);
}
