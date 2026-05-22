#include "DHT11.h"
#include "debug.h"

DHT11_data DHT11 = {0};

void DHT11_Init()
{
//===============================【初始化】================================
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //使能APB2 GPIO外设时钟

    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;         //设置为开漏输出模式并外置上拉电阻
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);

    DHT11_SET(1);   //释放总线
    Delay_Ms(1500); //越过不稳定状态
}

unsigned char DHT11_Read()
{
//===============================【读取温湿度】================================
//返回值：错误代码，0表示成功
//===========================================================================

    uint8_t i, j;
    uint8_t data[5] = {0x00};
//步骤二
    DHT11_SET(0);
    Delay_Ms(20);
    DHT11_SET(1);   //释放总线
//步骤三
    Delay_Us(30);
    if(DHT11_READ() == Bit_SET)return 1;
    while(DHT11_READ() != Bit_SET);//等待高电平
    Delay_Us(90);

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 8; j++)
        {
            data[i] <<= 1;
            while(DHT11_READ() == Bit_SET);//如果是低电平则下一步
            while(DHT11_READ() != Bit_SET);//等待54us的低电平过去
            Delay_Us(40);
            if (DHT11_READ() == Bit_SET)data[i] |= 0x01;
        }
    }

    if((data[0] + data[1] + data[2] + data[3]) != data[4])return 2;//校验

    DHT11.humidity = data[0] + data[1]/10.0f;

    if(data[3] & 0x80)DHT11.temperature = -(data[2] + (data[3] & 0x7f)/10.0f);//负温度处理
    else DHT11.temperature = data[2] + data[3]/10.0f;

    while(!DHT11_READ());//等待结束信号
    return 0;//成功
}
