#include "zf_common_headfile.h"


uint8 channel_index = 0;
uint8 adc_data[5];
int n;

adc_channel_enum channel_list[CHANNEL_NUMBER] =
{
    ADC_CHANNEL1, ADC_CHANNEL2, ADC_CHANNEL3, ADC_CHANNEL4,
    ADC_CHANNEL5, ADC_CHANNEL6
};

void adc_collect_init()
{
    adc_init(ADC_CHANNEL1, ADC_8BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL2, ADC_8BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL3, ADC_8BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL4, ADC_8BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL5, ADC_8BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL6, ADC_8BIT);                                           // 初始化对应 ADC 通道为对应精度

}

void adc_collect()
{
    adc_data[0]=adc_convert(ADC_CHANNEL1);
    adc_data[5]=adc_convert(ADC_CHANNEL6);
    adc_data[1]=adc_convert(ADC_CHANNEL2);
    adc_data[2]=adc_convert(ADC_CHANNEL3);
    adc_data[3]=adc_convert(ADC_CHANNEL4);
    adc_data[4]=adc_convert(ADC_CHANNEL5);
//  for(n = 0;n<6; n++)
//  {
//      adc_data[n] = adc_data[n] - 100;
//
//  }
    for(n = 0;n<5; n++)
    {
        if(adc_data[n] > 255)
        {
            adc_data[n] = 255;
        }
    }
}

