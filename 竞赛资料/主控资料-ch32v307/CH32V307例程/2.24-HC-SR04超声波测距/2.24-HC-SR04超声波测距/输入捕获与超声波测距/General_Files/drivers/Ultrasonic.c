#include "debug.h"
#include "Ultrasonic.h"

volatile uint8_t IC_FLAG = 0x00;//bit7 = 高电平标志位，bit6 = 捕获成功标志位
volatile u16 time = 0,overflow_CNT;//计数次数与定时器溢出次数

void Ultrasonic_Init()
{
//=============================【初始化】=================================
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_InitStructure = {0};
    TIM_ICInitTypeDef TIM_ICInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = ULTRASONIC_TRIG_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            //TRIG设为推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ULTRASONIC_PORT, &GPIO_InitStructure);
    GPIO_WriteBit(ULTRASONIC_PORT, ULTRASONIC_TRIG_PIN, 0);     //设为低电平

    GPIO_InitStructure.GPIO_Pin = ULTRASONIC_ECHO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       //ECHO设为浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ULTRASONIC_PORT, &GPIO_InitStructure);

    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;             //定时器向上计数模式
    TIM_InitStructure.TIM_Period = 65535;
    TIM_InitStructure.TIM_Prescaler = (SystemCoreClock / 1000000 - 1);   //定时器预分频器设置 = 1MHz,即每1us计数一次
    TIM_TimeBaseInit(TIM5, &TIM_InitStructure);                         //初始化TIM5

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                   //选择输入通道
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;        //上升沿捕获
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;    //映射到 TI1 上
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;              //配置输入分频,不分频
    TIM_ICInitStructure.TIM_ICFilter = 0x00;                           //输入滤波器不滤波
    TIM_ICInit(TIM5, &TIM_ICInitStructure);

    TIM_ITConfig(TIM5, TIM_IT_CC2|TIM_IT_Update, ENABLE );//使能捕获/比较中断和定时器更新中断
    TIM_Cmd(TIM5, ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;             //TIM5中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //设置抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;          //设置响应优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //使能中断
    NVIC_Init(&NVIC_InitStructure);                             //初始化NVIC
}

float Ultrasonic_GetDistance()
{
//============================【获取距离】================================
//返回值：获取到的距离，单位cm
//======================================================================

    //产生20us脉冲信号
    GPIO_WriteBit(ULTRASONIC_PORT, ULTRASONIC_TRIG_PIN, 1);
    Delay_Us(20);
    GPIO_WriteBit(ULTRASONIC_PORT, ULTRASONIC_TRIG_PIN, 0);

    while((IC_FLAG&0x40) == RESET);//等待捕获完成
    IC_FLAG = 0x00;//清除标志位

    /*58.8的来源为：声速为340m/s,这里计数单位为cm和us,就是0.034cm/us,1cm就是29.4us,
     * 声波从发射到接收经历了2倍的距离，所以29.4*2 = 58.8
     */
    return (time + overflow_CNT * 65535)/58.8f;
}

void TIM5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM5_IRQHandler(void)
{
    if(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update) == SET)
    {
        if((IC_FLAG&0x80)==0x80)overflow_CNT++;//如果还未捕获产生溢出就计数
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update); //清除中断标志位
    }

    if(TIM_GetITStatus(TIM5, TIM_IT_CC2) == SET) //检查TIM5捕获中断是否发生。
    {
        if((IC_FLAG&0x80)==RESET && (IC_FLAG&0x40)==RESET) //高电平标志与捕获标志都为0，说明之前没有捕获到高电平
        {
            overflow_CNT = 0;
            TIM5->CNT = 0;
            TIM5->CH2CVR = 0;
            IC_FLAG |= 0x80;   //置位高电平标志
            TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Falling); //下降沿捕获
        }
        else if((IC_FLAG&0x80)==0x80 && (IC_FLAG&0x40)==RESET)
        {
            time = TIM_GetCapture2(TIM5);  //获取捕获到的高电平时间
            IC_FLAG &= ~0x80;   //复位高电平标志
            IC_FLAG |= 0x40;   //置位捕获标志
            TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); //上升沿捕获
        }
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC2); //清除中断标志位
    }
}
