#include "zf_common_headfile.h"
uint8 PCA_r_IIC_SDA_1 = 0;

//内部数据定义
uint16 simiic_delay_time=10;                                        // 模拟IIC的延时时间，ICM等传感器应设置为100

void PCA_IIC_Init(void)
{
     gpio_init(PCA_IIC_SCL_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
     gpio_init(PCA_IIC_SDA_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

void SDA_OUT(void)  //设置SDA为输出模式  GPIO_Mode_Out_PP-推挽输出
{
     gpio_init(PCA_IIC_SDA_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

void SDA_IN(void)  //设置SDA为输入模式   GPI_PULL_UP——上拉输入
{
     gpio_init(PCA_IIC_SDA_PIN, GPO, GPIO_HIGH, GPI_PULL_UP);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC起始信号
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
void IIC_Start(void)
{
    SDA_OUT();
    PCA_IIC_SDA1();
    PCA_IIC_SCL1();
    system_delay_us(simiic_delay_time);
    PCA_IIC_SDA0();
    system_delay_us(simiic_delay_time);
    PCA_IIC_SCL0();
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC停止信号
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加simiic_delay_time的值
//-------------------------------------------------------------------------------------------------------------------
void IIC_Stop(void)
{
      SDA_OUT();
      PCA_IIC_SDA0();
    PCA_IIC_SCL0();
    system_delay_us(simiic_delay_time);
      PCA_IIC_SCL1();
    PCA_IIC_SDA1();
    system_delay_us(simiic_delay_time);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC等待应答
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心 如果IIC通讯失败可以尝试增加simiic_delay_time的值
//   返回1--应答出错
//   放回0--应答正确  针对与OLED，可以忽略掉这部分ACK，减少程序运行时间，但针对与mpu6050来说，则需要这部分应答！！！！
//-------------------------------------------------------------------------------------------------------------------
uint8 IIC_Wait_Ack(void)
{
    uint8 ucErrTime = 0;
    SDA_IN();
    PCA_IIC_SDA1();system_delay_us(1);
    PCA_IIC_SCL1();system_delay_us(1);
    while(PCA_r_IIC_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    PCA_IIC_SCL0();
    return 0;
}
//产生ACK应答
void IIC_Ack(void)
{
    PCA_IIC_SCL0();
    SDA_OUT();
    PCA_IIC_SDA0();
    system_delay_us(2);
    PCA_IIC_SCL1();
    system_delay_us(2);
    PCA_IIC_SCL0();
}
//不产生ACK应答
void IIC_NAck(void)
{
    PCA_IIC_SCL0();
    SDA_OUT();
    PCA_IIC_SDA1();
    system_delay_us(2);
    PCA_IIC_SCL1();
    system_delay_us(2);
    PCA_IIC_SCL0();
}



//-------------------------------------------------------------------------------------------------------------------
// @brief       模拟IIC 字节发送 送完后接收从应答 不考虑从应答位
// @param       c               发送c(可以是数据也可是地址)
// @return      void
// Sample usage:
// @note        内部调用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
void IIC_Send_Byte(uint8 txd)
{
    uint8 t;
    SDA_OUT();
    PCA_IIC_SCL0(); //拉低IIC总线时钟开始数据传输
    for(t=0;t<8;t++)
    {
        if(txd & 0x80)
        {
            PCA_IIC_SDA1();
        }
        else
        {
            PCA_IIC_SDA0();
        }
        txd<<=1;
        system_delay_us(2);
        PCA_IIC_SCL1();
        system_delay_us(2);
        PCA_IIC_SCL0();
        system_delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();
    for(i=0;i<8;i++)
    {
        PCA_IIC_SCL0();
        system_delay_us(2);
        PCA_IIC_SCL1();
        receive<<=1;
        PCA_r_IIC_SDA_1 = PCA_r_IIC_SDA;
        if(PCA_r_IIC_SDA_1)
            receive++;
        system_delay_us(1);
    }
    if(!ack)
        IIC_NAck();//发送NACK
    else
        IIC_Ack();//发送ACK
    return receive;
}



