#include "usart1.h"

//串口1配置
vu8 REV_OVER = 0;
void USART1_config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //开启端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//开启串口1时钟

    GPIO_InitTypeDef GPIO_Config;
    GPIO_Config.GPIO_Pin = GPIO_Pin_9;
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_Config);

    GPIO_Config.GPIO_Pin = GPIO_Pin_10;
    GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOA,&GPIO_Config);

    USART_InitTypeDef USART_Config;
    USART_Config.USART_BaudRate = 9600; //波特率
    USART_Config.USART_Parity = USART_Parity_No; //奇偶校验
    USART_Config.USART_WordLength =USART_WordLength_8b;//数据长度
    USART_Config.USART_StopBits = USART_StopBits_1;//停止位
    USART_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件控制
    USART_Config.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;

    USART_Init(USART1,&USART_Config);

    USART1_IT_Config();

    USART_Cmd(USART1,ENABLE); //发送 == 写数据  接收 ==读数据  使能立即工作

}

void USART1_IT_Config(void)
{
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); //使能指定的中断 2：待使能或使能的串口中断源

    NVIC_InitTypeDef USART1_struct;
    USART1_struct.NVIC_IRQChannel = USART1_IRQn ; //使能或失能指定的IRQn通道源
    USART1_struct.NVIC_IRQChannelCmd = ENABLE; //指定了在NVIC_IRQChannel中定义的IRQ通道是否使能
    USART1_struct.NVIC_IRQChannelPreemptionPriority = 1; //占先优先级
    USART1_struct.NVIC_IRQChannelSubPriority = 2; //从级优先级

    NVIC_Init(&USART1_struct); //初始化外设NVIC寄存器

}

//中断接收，到来一个接受一个,只有数据发送完，才会触发空闲中断
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
    {
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
        USART2->DATAR= USART1->DATAR;
        USART3->DATAR = USART1->DATAR;
    }
    //空闲总线中断
    if(USART_GetITStatus(USART1,USART_IT_IDLE)==SET)
    {
        u8 data = USART1->STATR;
        data = USART1->DATAR;//清中断的步骤（IDLE）
        REV_OVER=1;
    }
}

int fputc(int ch,FILE*system)
{
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
    USART_SendData(USART1,ch);
    return ch;
}








