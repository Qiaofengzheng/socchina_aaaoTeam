#include "zf_common_headfile.h"

void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_Init(GPIOB, &ampjb;GPIO_InitStructure);
}
void led()
{
    GPIO_Toggle_INIT();

    system_delay_ms(250);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13,SET );
    GPIO_WriteBit(GPIOB, GPIO_Pin_10,RESET );
    system_delay_ms(250);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13,RESET );
    GPIO_WriteBit(GPIOB, GPIO_Pin_10,SET );
}

