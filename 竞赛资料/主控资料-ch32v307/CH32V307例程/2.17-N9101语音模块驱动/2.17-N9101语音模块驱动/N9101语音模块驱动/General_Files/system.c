#include "system.h"
#include "debug.h"
#include  "../General_Files/drivers/N9101_drv.h"

void KEY_Init();
void KEY_Scan();

void System_Init()
{
    KEY_Init();
    N9101_OneWire_Init();
    N9101_OneWire_SendByte(N9101_CMD_Disk_TF);//从TF卡播放
    N9101_OneWire_SendByte(10);//播放第10首
    N9101_OneWire_SendByte(N9101_CMD_VOL_Base + 30);//音量为15
}

void System_Loop()
{
    static u8 i = 0;
    if(++i == 200)
    {
        GPIO_TogglePin(GPIOE, GPIO_Pin_0);
        i = 0;
    }
    KEY_Scan();
    Delay_Ms(5);
}

void KEY_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOE, GPIO_Pin_0 | GPIO_Pin_5 | GPIO_Pin_6, Bit_SET);
}

void KEY_Scan()
{
    static u8 N9101_play_status = N9101_CMD_Music_Pause;

    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == RESET)
    {
        N9101_OneWire_SendByte(N9101_CMD_Music_Last);//上一首
        GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_RESET);
        while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == RESET);
        GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_SET);
    }
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == RESET)
    {
        N9101_OneWire_SendByte(N9101_CMD_Music_Next);//下一首
        GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_RESET);
        while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == RESET);
        GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_SET);
    }
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == RESET)//暂停/播放
    {
        N9101_OneWire_SendByte(N9101_play_status);
        if(N9101_play_status == N9101_CMD_Music_Play)N9101_play_status = N9101_CMD_Music_Pause;
        else if(N9101_play_status == N9101_CMD_Music_Pause) N9101_play_status = N9101_CMD_Music_Play;
        GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_RESET);
        while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == RESET);
        GPIO_WriteBit(GPIOE, GPIO_Pin_5, Bit_SET);
    }
}
