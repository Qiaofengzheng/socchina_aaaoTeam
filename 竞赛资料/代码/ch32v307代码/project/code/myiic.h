#ifndef MYIIC_H_
#define MYIIC_H_
#include "zf_common_headfile.h"


//----宏定义OLED引脚----

#define PCA_IIC_SCL_PIN         B6
#define PCA_IIC_SDA_PIN         B7

//#define  PCA_IIC_SCL         gpio_dir(PCA_IIC_SCL_PIN,GPO);  //设置C11为输出模式
//#define  PCA_IIC_SDA         gpio_dir(PCA_IIC_SDA_PIN,GPO);  //设置C12为输出模式


#define PCA_IIC_SCL0()          gpio_low(PCA_IIC_SCL_PIN)                                        // IO口输出低电平
#define PCA_IIC_SCL1()          gpio_high(PCA_IIC_SCL_PIN)                              // IO口输出高电平
#define PCA_IIC_SDA0()          gpio_low(PCA_IIC_SDA_PIN)                                            // IO口输出低电平
#define PCA_IIC_SDA1()          gpio_high(PCA_IIC_SDA_PIN)                                  // IO口输出高电平

#define PCA_r_IIC_SDA           gpio_get_level(B7)                             // IO口输出低电平


void PCA_IIC_Init(void);//软模拟IIC引脚初始化
void SDA_OUT(void);
void SDA_IN(void);
void IIC_Start(void);//IIC开始信号
void IIC_Stop(void);//IIC结束信号
uint8 IIC_Wait_Ack(void);//等待应答信号ACK
void IIC_Ack(void);//产生ACK应答
void IIC_NAck(void);//产生非ACK应答
void IIC_Send_Byte(uint8 txd);//IIC发送一个字节
uint8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节




#endif
