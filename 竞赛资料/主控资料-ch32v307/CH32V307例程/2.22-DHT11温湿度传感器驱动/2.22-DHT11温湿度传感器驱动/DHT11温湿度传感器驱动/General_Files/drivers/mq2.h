#ifndef __MQ2_H
#define __MQ2_H
#include "ch32v30x.h"
#include "ch32v30x_gpio.h"
#include "debug.h"



extern u16 DMA_buf[3];
extern u16 arr_count[2];
void ADC1_Config(void);
u16 ADC_Result(u8 ADC_Channel_x);
void DMA_Config(void);

#endif
