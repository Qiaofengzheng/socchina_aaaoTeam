#ifndef ADC_H_
#define ADC_H_
#include "zf_common_headfile.h"

#define CHANNEL_NUMBER          (6)

#define ADC_CHANNEL1            (ADC1_IN8_B0)
#define ADC_CHANNEL2            (ADC1_IN9_B1)
#define ADC_CHANNEL3            (ADC1_IN10_C0)
#define ADC_CHANNEL4            (ADC1_IN11_C1)
#define ADC_CHANNEL5            (ADC1_IN12_C2)
#define ADC_CHANNEL6            (ADC1_IN13_C3)

extern uint8 adc_data[5];

void adc_collect_init();
void adc_collect();


#endif
