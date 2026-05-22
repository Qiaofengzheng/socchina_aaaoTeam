//#include "sys.h"
#include "oled.h"
#include "usart.h"
#include "openmv.h"


//如果使用ucos,则包括下面的头文件即可.
//#if SYSTEM_SUPPORT_OS
//#include "includes.h"                   //ucos 使用
//#endif


//#define RING_BUFFER_LEN     (1024u)
//
//// The length of a single buffer used by DMA
//#define RX_BUFFER_LEN       (128u)
//
//#define USART_RX_CH         DMA1_Channel5
//
//#define USART_REC_LEN           200
//#define EN_USART1_RX            1
//******************************************************************

#define RX_BUFFER_LEN       (2048u)

uint8_t          Rx_Buffer[RX_BUFFER_LEN];
uint8_t          Tx_Buffer[RX_BUFFER_LEN];
int rcv_uart2_flag = 0; //数据接收标志
int rcv_uart2_datalen = 0;//数据长度

//#if EN_USART1_RX
//u8 USART_RX_BUF[USART_REC_LEN];
//u16 USART_RX_STA=0;

//struct
//{
//    volatile uint8_t DMA_USE_BUFFER;
//    uint8_t          Rx_Buffer[2][RX_BUFFER_LEN];
//
//} USART_DMA_CTRL = {
//    .DMA_USE_BUFFER = 0,
//    .Rx_Buffer      = {0},
//};

//struct
//{
//    uint8_t           buffer[RING_BUFFER_LEN];
//    volatile uint16_t RecvPos;  //
//    volatile uint16_t SendPos;  //
//    volatile uint16_t RemainCount;
//
//} ring_buffer = {{0}, 0, 0, 0};

/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART1 peripheral.
 *
 * @return  none
 */
void USART1_CFG(uint32_t baudrate)
{
//    GPIO_InitTypeDef  GPIO_InitStructure  = {0};
//    USART_InitTypeDef USART_InitStructure = {0};
//    NVIC_InitTypeDef  NVIC_InitStructure  = {0};
//
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_USART1, ENABLE);
//
//    /* USART1 TX-->A.9   RX-->A.10 */
//    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//    USART_InitStructure.USART_BaudRate            = baudrate;
//    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
//    USART_InitStructure.USART_Parity              = USART_Parity_No;
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
//
//    USART_Init(USART1, &USART_InitStructure);
//    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
//
//    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//
//    USART_Cmd(USART1, ENABLE);
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
        /* USART2 TX-->A.2   RX-->A.3 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
        USART_Init(USART2, &USART_InitStructure);

        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;      //抢占优先级为1
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;           //子优先级为1
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //IRQ通道使能
        NVIC_Init(&NVIC_InitStructure);                              //中断优先级初始化

        USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
        USART_Cmd(USART2,ENABLE);
//        current_baudrate = baudrate;
        printf("usart1_init_success\r\n");
}

void USART1_IRQHandler(void)
{
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
        {
//            DMA_RcvData_Handle();
        Res =USART_ReceiveData(USART2);// 接收数据、处理数据
        Openmv_Receive_Data(Res);
        Openmv_Data();
        OLED_ShowNum(56,2,data2,3);
        }
//    u8 Res;
//#if SYSTEM_SUPPORT_OS       //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//    OSIntEnter();
//#endif
//    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//        {
//            Res =USART_ReceiveData(USART1);
//            Openmv_Receive_Data(Res);
//            Openmv_Data();
////            OLED_ShowNum(56,2,Res,3);
////            if((USART_RX_STA&0x8000)==0)//接收未完成
////                    {
////                        if(USART_RX_STA&0x4000)//接收到了0x0d
////                        {
////                            if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
////                            else USART_RX_STA|=0x8000;  //接收完成了
////                        }
////                        else //还没收到0X0D
////                        {
////                            if(Res==0x0d)USART_RX_STA|=0x4000;
////                            else
////                            {
////                                USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
////                                USART_RX_STA++;
////                                if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收
////                            }
////                        }
////                    }
//        }
//
//
//#if SYSTEM_SUPPORT_OS   //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//    OSIntExit();
//#endif
}
//#endif




