#include "debug.h"
#include "N9101_drv.h"

void N9101_OneWire_Init()
{
//=============================【初始化】=================================
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = N9101_ONE_WIRE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(N9101_ONE_WIRE_PORT, &GPIO_InitStructure);
}

void N9101_OneWire_SendByte(N9101_CMD_t N9101_CMD)
{
//=============================【初始化】=================================
//参数：
//N9101_CMD:要发送的命令
//=======================================================================
    u8 var;

    N9101_SDA_CTRL(0);
    Delay_Ms(4);
    for(var = 0; var < 8; var++)
    {
        N9101_SDA_CTRL(1);
        if(N9101_CMD & 0x01)
        {
            Delay_Us(1800);
            N9101_SDA_CTRL(0);
            Delay_Us(600);
        }
        else
        {
            Delay_Us(600);
            N9101_SDA_CTRL(0);
            Delay_Us(1800);
        }
        N9101_CMD >>= 1;
    }
    N9101_SDA_CTRL(1);
    Delay_Us(2000);
}
