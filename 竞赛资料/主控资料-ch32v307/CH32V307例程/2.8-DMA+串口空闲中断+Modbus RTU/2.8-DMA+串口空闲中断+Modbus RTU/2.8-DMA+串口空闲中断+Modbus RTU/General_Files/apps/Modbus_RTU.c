#include "Modbus_RTU.h"
#include "debug.h"
#include "../General_Files/drivers/CRC_SOFT.h"

static void Modbus_RTU_Set_Direction(unsigned char dir);
static void Modbus_RTU_WriteREG(Modbus_RTU_Buff *modbus_RTU_buff);
static void Modbus_RTU_ReadREG(Modbus_RTU_Buff *modbus_RTU_buff);
static void Modbus_RTU_SendArray(unsigned char length);

Modbus_RTU_Buff modbus_RTU_data_buff = {0, 0};

void Modbus_RTU_Init(unsigned int baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE);
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = RS485_RX_PIN | RS485_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(RS485_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = RS485_DE_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(RS485_DE_PORT, &GPIO_InitStructure);

    //USART2串口接收DMA
    DMA_DeInit(DMA1_Channel6);                                         //恢复为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&RS485_UART->DATAR);        //设置外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)modbus_RTU_data_buff.rec_buff_p; //设置存储器地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      //外设为源头
    DMA_InitStructure.DMA_BufferSize = RX_BUFF_LENGTH;                      //缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //禁用外设增量
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //使能存储器增量
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         //存储器数据长度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           //非循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 //优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            //禁用M2M
    DMA_Init(DMA1_Channel6, &DMA_InitStructure );
    DMA_Cmd(DMA1_Channel6, ENABLE);

    //USART2串口发送DMA
    DMA_DeInit(DMA1_Channel7);                                         //恢复为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&RS485_UART->DATAR);        //设置外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)modbus_RTU_data_buff.send_buff_p; //设置存储器地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      //外设为目标
    DMA_InitStructure.DMA_BufferSize = TX_BUFF_LENGTH;                      //缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        //禁用外设增量
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 //使能存储器增量
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         //存储器数据长度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           //非循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 //优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            //禁用M2M
    DMA_Init(DMA1_Channel7, &DMA_InitStructure );
//    DMA_Cmd(DMA1_Channel7, ENABLE);

    USART_InitStructure.USART_BaudRate = baudrate;             //设置USART2波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据长度8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     //停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;        //无校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                //全双工模式
    USART_Init(RS485_UART, &USART_InitStructure);
    USART_ITConfig(RS485_UART, USART_IT_IDLE, ENABLE);    //配置为串口空闲中断

    USART_DMACmd(RS485_UART, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);    //打开USART2 DMA
    USART_Cmd(RS485_UART, ENABLE);

    //初始化USART2 NVIC，设置中断优先级分组
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;             //USART2中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //设置抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;           //设置响应优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

static void Modbus_RTU_Set_Direction(unsigned char dir)
{
    GPIO_WriteBit(RS485_DE_PORT, RS485_DE_PIN, dir);
    Delay_Ms(5);    //让硬件有反应的时间
}

static void Modbus_RTU_SendArray(unsigned char length)
{
    USART_ITConfig(RS485_UART, USART_IT_IDLE, DISABLE);//暂时关闭串口空闲中断
    Modbus_RTU_Set_Direction(RS485_TX);//设置为发送模式
    DMA_SetCurrDataCounter(DMA1_Channel7, length);  //设置DMA_Channel7发送长度
    USART_DMACmd(RS485_UART, USART_DMAReq_Tx, ENABLE);//重新打开USART2 DMA发送
    DMA_Cmd(DMA1_Channel7, ENABLE);                 //打开DMA_Channel7
    while(DMA_GetFlagStatus(DMA1_FLAG_TC7) != SET); //检查DMA_Channel7发送是否完成
    DMA_ClearFlag(DMA1_FLAG_TC7);                   //清除标志位
    DMA_Cmd(DMA1_Channel7, DISABLE);                //关闭DMA_Channel7
    Delay_Ms(5);                                    //让硬件完成发送
    Modbus_RTU_Set_Direction(RS485_RX);//发送完成后设置为接收模式
    USART_ITConfig(RS485_UART, USART_IT_IDLE, ENABLE);//重新启用串口空闲中断
}

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(RS485_UART, USART_IT_IDLE) == SET)//检测到串口空闲中断
    {
        DMA_Cmd(DMA1_Channel6, DISABLE);//关闭DMA_Channel6
        DMA_SetCurrDataCounter(DMA1_Channel6, RX_BUFF_LENGTH);//设置DMA_Channel6接收长度
        Modbus_RTU_Protocol_Analysis(&modbus_RTU_data_buff);//协议解析
        USART_DMACmd(RS485_UART, USART_DMAReq_Rx, ENABLE);//重新打开USART2 DMA接收
        DMA_Cmd(DMA1_Channel6, ENABLE);//打开DMA_Channel6
    }
    USART_ReceiveData(RS485_UART);//清除中断标志位
}

void Modbus_RTU_Protocol_Analysis(Modbus_RTU_Buff *modbus_RTU_buff)
{
//=======================【协议解析】=====================================
//modbus_RTU_buff：读写缓存结构体
//======================================================================
    uint8_t i, index = 0;
    //过滤干扰数据
    for(i = 0; i < RX_BUFF_LENGTH - 8; i++)
    {
        if(index == 0)//如果没有检测到地址，则进行下一次循环
            if(*(modbus_RTU_buff->rec_buff_p + i) != Modbus_RTU_Addr)continue;
        *(modbus_RTU_buff->rec_buff_p + index) = *(modbus_RTU_buff->rec_buff_p + i);
        if(index == 7)break;//已读取到1帧数据后结束
        index++;
    }
    //CRC-16校验
    CRC_SOFT.CRC16(modbus_RTU_buff->rec_buff_p, 6); //计算CRC16
    //兼容高位在前和低位在前
    if((*(modbus_RTU_buff->rec_buff_p + 6) == CRC_SOFT.CRC16_L && *(modbus_RTU_buff->rec_buff_p + 7) == CRC_SOFT.CRC16_H)
            ||
       (*(modbus_RTU_buff->rec_buff_p + 6) == CRC_SOFT.CRC16_H && *(modbus_RTU_buff->rec_buff_p + 7) == CRC_SOFT.CRC16_L))
    {
        if (*(modbus_RTU_buff->rec_buff_p + 0) == Modbus_RTU_Addr)//如果第一byte与地址相同
        {
            if(*(modbus_RTU_buff->rec_buff_p + 1) == Modbus_RTU_CMD_RD_REG)//如果第二byte是读寄存器
                Modbus_RTU_ReadREG(modbus_RTU_buff);
            if(*(modbus_RTU_buff->rec_buff_p + 1) == Modbus_RTU_CMD_WR_REG)//如果第二byte是写寄存器
                Modbus_RTU_WriteREG(modbus_RTU_buff);
        }
    }
    for(i = 0; i < RX_BUFF_LENGTH; i++)//清除接收缓存
        *(modbus_RTU_buff->rec_buff_p + i) = 0x00;
}

static void Modbus_RTU_WriteREG(Modbus_RTU_Buff *modbus_RTU_buff)
{
//=====================【写寄存器】=======================================
//modbus_RTU_buff：读写缓存结构体
//======================================================================
    unsigned char var;
    //如果寄存器地址是LED1的地址
    if ((*(modbus_RTU_buff->rec_buff_p + 2) << 8 | *(modbus_RTU_buff->rec_buff_p + 3)) == Modbus_RTU_DeviceREG_LED1)
    {
        if (*(modbus_RTU_buff->rec_buff_p + 5) == 0x01)GPIO_ResetBits(GPIOE, GPIO_Pin_0);//如果数据是0x01
        else GPIO_SetBits(GPIOE, GPIO_Pin_0);
    }
    //如果寄存器地址是LED2的地址
    if ((*(modbus_RTU_buff->rec_buff_p + 2) << 8 | *(modbus_RTU_buff->rec_buff_p + 3)) == Modbus_RTU_DeviceREG_LED2)
    {
        if (*(modbus_RTU_buff->rec_buff_p + 5) == 0x01)GPIO_ResetBits(GPIOE, GPIO_Pin_6);//如果数据是0x01
        else GPIO_SetBits(GPIOE, GPIO_Pin_6);
    }
    //如果寄存器地址是LED3的地址
    if ((*(modbus_RTU_buff->rec_buff_p + 2) << 8 | *(modbus_RTU_buff->rec_buff_p + 3)) == Modbus_RTU_DeviceREG_LED3)
    {
        if (*(modbus_RTU_buff->rec_buff_p + 5) == 0x01)GPIO_ResetBits(GPIOE, GPIO_Pin_5);//如果数据是0x01
        else GPIO_SetBits(GPIOE, GPIO_Pin_5);
    }
    for (var = 0; var < 8; ++var)
    {
        *(modbus_RTU_buff->send_buff_p + var) = *(modbus_RTU_buff->rec_buff_p + var);
    }
    Modbus_RTU_SendArray(8);//把数据发回去
}

static void Modbus_RTU_ReadREG(Modbus_RTU_Buff *modbus_RTU_buff)
{
//=====================【读寄存器】=======================================
//modbus_RTU_buff：读写缓存结构体
//固定返回4个寄存器数据，即8Byte
//======================================================================
    *(modbus_RTU_buff->send_buff_p + 0) = Modbus_RTU_Addr;
    *(modbus_RTU_buff->send_buff_p + 1) = Modbus_RTU_CMD_RD_REG;
    *(modbus_RTU_buff->send_buff_p + 2) = 8;//数据长度
    *(modbus_RTU_buff->send_buff_p + 3) = 0;
    *(modbus_RTU_buff->send_buff_p + 4) = GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_0);
    *(modbus_RTU_buff->send_buff_p + 5) = 0;
    *(modbus_RTU_buff->send_buff_p + 6) = GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_6);
    *(modbus_RTU_buff->send_buff_p + 7) = 0;
    *(modbus_RTU_buff->send_buff_p + 8) = GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_5);
    extern float temperature;
    temperature = (temperature * 100) + 5500;//-55.00℃x100 = -5500 + 5500 ->0
    *(modbus_RTU_buff->send_buff_p + 9) = (unsigned int)temperature >> 8;
    *(modbus_RTU_buff->send_buff_p + 10) = (unsigned char)temperature;
    //CRC-16校验
    CRC_SOFT.CRC16(modbus_RTU_buff->send_buff_p, 11); //计算CRC16
    *(modbus_RTU_buff->send_buff_p + 11) = CRC_SOFT.CRC16_H;
    *(modbus_RTU_buff->send_buff_p + 12) = CRC_SOFT.CRC16_L;
    Modbus_RTU_SendArray(13);
}
