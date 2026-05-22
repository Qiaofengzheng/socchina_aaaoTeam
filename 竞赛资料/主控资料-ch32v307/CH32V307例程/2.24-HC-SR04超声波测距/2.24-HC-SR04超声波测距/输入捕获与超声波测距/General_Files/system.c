#include "system.h"
#include "debug.h"
#include "../General_Files/drivers/ST7789.h"
#include "../General_Files/drivers/Ultrasonic.h"

#define PWM_PORT GPIOE
#define PWM_PIN GPIO_Pin_4

uint16_t i;

void Music_Init();
void TIM6_Init();

void System_Init()
{
    Ultrasonic_Init();
    Music_Init();
    TIM6_Init();
    ST7789_Init();
    ST7789_ShowString(0, 10, "Distance:", color_WHITE, color_BLACK, SIZE_ASCII_16x32);
    ST7789_ShowNumf(144, 10, 0.00, 5, color_WHITE, color_BLACK, SIZE_ASCII_16x32);//初始值
    ST7789_ShowString(256, 10, "cm",color_WHITE, color_BLACK, SIZE_ASCII_16x32);
}

void System_Loop()
{
    float temp = Ultrasonic_GetDistance();
    ST7789_ShowNumf(144, 10, temp, 5, color_WHITE, color_BLACK, SIZE_ASCII_16x32);
    i = (uint16_t)temp;
    Delay_Ms(200);
}

void Music_Init()
{
/*========================================【Music初始化】=======================================*/
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_TIM10 | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_FullRemap_TIM10, ENABLE);                  //TIM10完全映射
    GPIO_InitStructure.GPIO_Pin = PWM_PIN;                              //TIM10 CH2N
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                     //设置为复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PWM_PORT, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;    //定时器向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = (1000000 / 1000) - 1;           //自动重装载寄存器，1MHz/1000Hz(1ms)
    TIM_TimeBaseInitStructure.TIM_Prescaler = (SystemCoreClock / 1000000 - 1);//定时器预分频器设置 = 1MHz
    TIM_TimeBaseInit(TIM10, &TIM_TimeBaseInitStructure);                 //初始化TIM10

    /*********************************************************************************
    TIM_OCMode_PWM1：TIM_CounterMode_Up时，当定时器CNT<CCR值时通道1为有效电平，否则为无效电平。
    TIM_OCMode_PWM2：TIM_CounterMode_Up时，当定时器CNT>CCR值时通道1为有效电平，否则为无效电平。
    *********************************************************************************/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;                   //输出比较模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;      //禁用P通道输出
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;     //使能N通道输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;           //输出比较P通道有效电平为Low
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;         //输出比较N通道有效电平为Low
    TIM_OCInitStructure.TIM_Pulse = (uint16_t)(TIM10->ATRLR * 0.5);    //CCR值为ARR的一半，即占空比50％
    TIM_OC2Init( TIM10, &TIM_OCInitStructure);                         //初始化输出比较通道2

    TIM_Cmd(TIM10, ENABLE);

    Delay_Ms(500);
}

void TIM6_Init()
{
    TIM_TimeBaseInitTypeDef TIM_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC, ENABLE);

    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;    //定时器向上计数模式
    TIM_InitStructure.TIM_Period = (1000000 / 1000);           //自动重载寄存器，1MHz/1000Hz(1ms)
    TIM_InitStructure.TIM_Prescaler = (SystemCoreClock / 1000000 - 1);//定时器预分频器设置 = 1MHz
    TIM_TimeBaseInit(TIM6, &TIM_InitStructure);                 //初始化TIM6
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);                  //启用定时器6更新中断
    TIM_Cmd(TIM6, ENABLE);                                      //TIM6使能

    //初始化TIM NVIC，设置中断优先级分组
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;             //TIM6中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //设置抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;           //设置响应优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //使能通道1中断
    NVIC_Init(&NVIC_InitStructure);                              //初始化NVIC
}

void TIM6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM6_IRQHandler(void)
{
    static unsigned int time = 0;
    static unsigned char beep_flag = DISABLE;
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) == SET) //检查TIM6中断是否发生。
    {
        if(i < 5)
        {
            TIM_CtrlPWMOutputs(TIM10, ENABLE);
            TIM_ClearITPendingBit(TIM6,TIM_IT_Update);    //清除TIM6的中断挂起位。
            return;
        }
        if(i > 50)
        {
            TIM_CtrlPWMOutputs(TIM10, DISABLE);
            TIM_ClearITPendingBit(TIM6,TIM_IT_Update);    //清除TIM6的中断挂起位。
            return;
        }
        if(++time > i*10)
        {
          time = 0;
          TIM_CtrlPWMOutputs(TIM10, !beep_flag);
          beep_flag = !beep_flag;
        }
        TIM_ClearITPendingBit(TIM6,TIM_IT_Update);    //清除TIM6的中断挂起位。
    }
}
