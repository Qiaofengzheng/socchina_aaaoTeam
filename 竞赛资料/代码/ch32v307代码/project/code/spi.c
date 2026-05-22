#include "zf_common_headfile.h"


void Software_SPI_Init(void)
{
    /*CS*/
    gpio_init(A4, GPO, GPIO_HIGH, GPO_PUSH_PULL);//B10
    /*SCK*/
    gpio_init(B13, GPO, GPIO_HIGH, GPO_PUSH_PULL);//B12
    /*MOSI*/
    gpio_init(B15, GPO, GPIO_HIGH, GPO_PUSH_PULL);//B13

    SPI_CS_1();
    SPI_SCK_1();
    SPI_MOSI_1();
}

#if SPI_Mode_0
/*
Mode_0
MSB先行
data :待写入的数据
*/
void Software_SPI_Write(int8 data)
{
    uint8 i;
    for(i=0;i<8;i++)
    {
        SPI_SCK_0();
        system_delay_us(1);
        if(data & 0x80)
        {
            SPI_MOSI_1();
        }
        else
        {
            SPI_MOSI_0();
        }
        data <<= 1;
        SPI_SCK_1();
        system_delay_us(1);
    }
}


#elif SPI_Mode_3
/*

Mode_3
MSB先行
data :待写入的数据
*/

void Software_SPI_Write_Read(u8 data)
{
    u8 i;
    SPI_SCK_1();
    systick_delay_us(1);
    for(i=0;i<8;i++)
    {
        SPI_SCK_0();
        systick_delay_us(1);
        if(data & 0x80)
        {
            SPI_MOSI_1();
        }
        else
        {
            SPI_MOSI_0();
        }
        data <<= 1;
        SPI_SCK_1();
        systick_delay_us(1);
    }

}
#endif

