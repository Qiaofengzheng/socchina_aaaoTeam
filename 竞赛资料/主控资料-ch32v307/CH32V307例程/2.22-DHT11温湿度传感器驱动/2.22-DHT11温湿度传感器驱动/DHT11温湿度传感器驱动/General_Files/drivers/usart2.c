#include "usart2.h"
//串口2配置
NB_Struct NB_msg={0};
void USART2_config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    GPIO_InitTypeDef GPIO_Config={0};
    GPIO_Config.GPIO_Pin = GPIO_Pin_2; //TX 发送
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_Config);

    GPIO_Config.GPIO_Pin = GPIO_Pin_3; //RX 接收
    GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA,&GPIO_Config);

    USART_InitTypeDef USART2_Config={0};
    USART2_Config.USART_BaudRate = 9600; //波特率
    USART2_Config.USART_Parity = USART_Parity_No; //奇偶校验
    USART2_Config.USART_WordLength =USART_WordLength_8b;//数据长度
    USART2_Config.USART_StopBits = USART_StopBits_1;//停止位
    USART2_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件控制
    USART2_Config.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; //模式

    USART_Init(USART2,&USART2_Config);//初始化串口2
    USART2_IT_Config();
    USART_Cmd(USART2,ENABLE); //发送 == 写数据  接收 ==读数据  使能立即工作  使能或失能串口外设

}

//中断配置
void USART2_IT_Config(void)
{
    USART_ITConfig(USART2,USART_IT_RXNE, ENABLE); //使能指定的中断 2：待使能或使能的串口中断源
    USART_ITConfig(USART2,USART_IT_IDLE, ENABLE); //使能空闲总线中断

    NVIC_InitTypeDef USART2_struct;
    USART2_struct.NVIC_IRQChannel = USART2_IRQn ; //使能或失能指定的IRQn通道源
    USART2_struct.NVIC_IRQChannelCmd = ENABLE; //指定了在NVIC_IRQChannel中定义的IRQ通道是否使能
    USART2_struct.NVIC_IRQChannelPreemptionPriority = 1; //占先优先级
    USART2_struct.NVIC_IRQChannelSubPriority = 3; //从级优先级

    NVIC_Init(&USART2_struct); //初始化外设NVIC寄存器

}
//串口2的中断服务函数
/*
PE FE NE ORE and IDLE pending bits are cleared by software sequence:
a <read> operation to <USART_SR register> followed by a <read> operation to <USART_DR register>
*/
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)//中断接收标志位被置1
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除串口中断待处理位 不然下一次中断进不来
        u8 data = USART2->DATAR;
        USART1->DATAR = data;//将串口2的数据传给串口1 然后在串口助手中显示NB中的数据  回显

        NB_msg.buff[NB_msg.length++]=data;//将WiFi内的数据传给一个数组
        if(NB_msg.length>=1024)
        {
            NB_msg.length=0;
        }
    }
    //空闲总线中断
    if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET)
    {
        u8 data;
        data= USART2->STATR;
        data = USART2->DATAR;//清空闲中断总线的步骤（IDLE）
    }
}

void USART2_SendData(u8 ch)
{
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)== RESET);//发送完成中断
    USART_SendData(USART2,ch);
}

//在主程序中发送指令给串口2
void NB_SendString(char *str)
{
    while((*str)!= '\0')
        USART2_SendData(*str++);
}

u16 USART2_ReadData(void)
{
    while(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)== RESET);//接收中断
    return USART_ReceiveData(USART2);
}








