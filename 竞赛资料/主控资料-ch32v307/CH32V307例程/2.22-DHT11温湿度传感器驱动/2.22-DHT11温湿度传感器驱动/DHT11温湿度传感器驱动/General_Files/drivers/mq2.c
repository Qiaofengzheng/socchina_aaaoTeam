#include "mq2.h"
//mq2 mq7 mq135的采集
//=============================================================================
//变量名称：
//DMA_buf[0]  MQ2    烟雾
//DMA_buf[1]  MQ7    一氧化碳
//DMA_buf[2]  MQ135  空气质量
//=============================================================================
u16 arr_count[2]={0};
void ADC1_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitTypeDef GPIO_Struct = {0};
    GPIO_Struct.GPIO_Mode = GPIO_Mode_AIN;                       //模拟输入
    GPIO_Struct.GPIO_Pin = GPIO_Pin_1;//|GPIO_Pin_6|GPIO_Pin_7
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_Struct);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

    ADC_InitTypeDef ADC_Struct = {0};
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                            //设置ADC时钟 72/6<14
    ADC_Struct.ADC_Mode = ADC_Mode_Independent;                  //独立工作模式
    ADC_Struct.ADC_ContinuousConvMode = ENABLE;                  //连续模式
    ADC_Struct.ADC_ScanConvMode = ENABLE;                        //多通道模式
    ADC_Struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件触发启动
    ADC_Struct.ADC_DataAlign = ADC_DataAlign_Right;              //右对齐
    ADC_Struct.ADC_NbrOfChannel = 3;                             //规定了顺序进行规则转换的 ADC 通道的数目
    ADC_Init(ADC1,&ADC_Struct);

    //设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
    ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);//MQ2
//    ADC_RegularChannelConfig(ADC1,ADC_Channel_6,2,ADC_SampleTime_239Cycles5);//MQ7
//    ADC_RegularChannelConfig(ADC1,ADC_Channel_7,3,ADC_SampleTime_239Cycles5);//MQ135

    ADC_DMACmd(ADC1,ENABLE);
//    DMA_Config();
    ADC_Cmd(ADC1,ENABLE);

    //校准:减小误差
    ADC_ResetCalibration(ADC1);                            //重置寄存器
    while(ADC_GetResetCalibrationStatus(ADC1)==SET)        //等待重置完成
    {}
    ADC_StartCalibration(ADC1);                            //启动校准，用校准寄存器 校准 ADC1
    while(ADC_GetCalibrationStatus(ADC1)==SET)             //等待校准完成
    {}
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);                 //启动转换 使能或者失能指定的 ADC 的软件转换启动功能
}

u16 DMA_buf[3]={0};
void DMA_Config(void)
{
    DMA_InitTypeDef DMA_Struct={0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);               //开启DMA时钟

    DMA_Struct.DMA_PeripheralBaseAddr =(u32) &ADC1->RDATAR;             //定义DMA外设基地址
    DMA_Struct.DMA_DIR = DMA_DIR_PeripheralSRC;                     //外设作为数据传输的来源
    DMA_Struct.DMA_BufferSize = 3;                                  //地址递增3次（单位为字宽）
    DMA_Struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //外设地址寄存器不变
    DMA_Struct.DMA_MemoryInc = DMA_MemoryInc_Enable;                //内存地址寄存器递增
    DMA_Struct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;    //数据宽度为16位
    DMA_Struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
    DMA_Struct.DMA_Mode = DMA_Mode_Circular;                        //工作在循环缓存模式
    DMA_Struct.DMA_Priority = DMA_Priority_High;                    //优先级
    DMA_Struct.DMA_M2M = DMA_M2M_Disable;                           //DMA通道没有设置为内存到内存传输
    DMA_Struct.DMA_MemoryBaseAddr = (u32)&DMA_buf[0];               //内存基地址

    DMA_Init(DMA1_Channel1,&DMA_Struct);
    DMA_Cmd(DMA1_Channel1,ENABLE);
}

//不采用DMA的多通道采集方法
u16 ADC_Result(u8 ADC_Channel_x)
{
  u16 ADC_val = ADC_GetConversionValue(ADC1); //返回最近一次 ADCx 规则组的转换结果
  ADC_RegularChannelConfig(ADC1,ADC_Channel_x,1,ADC_SampleTime_239Cycles5);
  ADC_SoftwareStartConvCmd(ADC1,ENABLE); //启动转换    使能或者失能指定的 ADC 的软件转换启动功能
  while((ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC))==RESET);

  //float ADC_Cha = (ADC_val*3.3/4096);
  //printf("ADC_val == %d\r\n",ADC_val);
  return ADC_val;
}



