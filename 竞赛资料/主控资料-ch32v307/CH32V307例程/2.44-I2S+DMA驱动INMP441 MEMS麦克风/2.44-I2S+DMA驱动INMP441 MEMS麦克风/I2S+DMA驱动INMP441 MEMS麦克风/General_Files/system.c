#include "system.h"
#include "debug.h"
#include "string.h"
#include "../General_Files/drivers/INMP441.h"
#include "../General_Files/drivers/dac_drv.h"

void System_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    INMP441_Init();
    DAC_User_Init();
}

__attribute__((section(".highcode")))
void System_Loop()
{
//	//24bit
//	uint32_t temp;
//	temp = ((Audio.audio[0]<<8) | (Audio.audio[1]>>8));
//	if(temp & 0x800000)temp |= 0xFF000000;

	//24bit->12bit
	int16_t temp;
	temp = ((Audio.audio[0]<<8) | (Audio.audio[1]>>8)) >> 8;//Ë¥¼õ24dB

	//Ë«ÏòÏÞ·ù0-4095
	if((s16)temp > 2047)
	{
		temp = 2047;
		GPIO_ResetBits(GPIOE, GPIO_Pin_0);
	}
	else if((s16)temp < -2048)
	{
		temp = -2048;
		GPIO_ResetBits(GPIOE, GPIO_Pin_0);
	}
	else
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_0);
	}

	DAC_SetChannel2Data(DAC_Align_12b_R, (s16)temp + 2048);
//	printf("%d\r\n", (s16)tempL);
//	Delay_Ms(100);
}
