#include "zf_common_headfile.h"
#include "math.h"

void PCA9685_Init(float hz,uint8 angle)
{
    uint32 off = 0;
    PCA_IIC_Init();
    PCA9685_Write(PCA_Model,0x00);
    PCA9685_setFreq(hz);
    off = (uint32)(118+angle*2.2);
    PCA9685_setPWM(0,0,off);
    PCA9685_setPWM(1,0,off);
    PCA9685_setPWM(2,0,off);
    PCA9685_setPWM(3,0,off);
    PCA9685_setPWM(4,0,off);
    PCA9685_setPWM(5,0,off);
    PCA9685_setPWM(6,0,off);
    PCA9685_setPWM(7,0,off);
    PCA9685_setPWM(8,0,off);
    PCA9685_setPWM(9,0,off);
    PCA9685_setPWM(10,0,off);
    PCA9685_setPWM(11,0,426);
    system_delay_ms(100);

}

void PCA9685_Write(uint8 addr,uint8 data)
{
    IIC_Start();

    IIC_Send_Byte(PCA_Addr);
    IIC_NAck();

    IIC_Send_Byte(addr);
    IIC_NAck();

    IIC_Send_Byte(data);
    IIC_NAck();

    IIC_Stop();


}

uint8 PCA9685_Read(uint8 addr)
{
    uint8 data;

    IIC_Start();

    IIC_Send_Byte(PCA_Addr);
    IIC_NAck();

    IIC_Send_Byte(addr);
    IIC_NAck();

    IIC_Stop();

    system_delay_us(10);


    IIC_Start();

    IIC_Send_Byte(PCA_Addr|0x01);
    IIC_NAck();

    data = IIC_Read_Byte(0);

    IIC_Stop();

    return data;

}

void PCA9685_setPWM(uint8 num,uint32 on,uint32 off)
{
    IIC_Start();

    IIC_Send_Byte(PCA_Addr);
    IIC_Wait_Ack();

    IIC_Send_Byte(LED0_ON_L+4*num);
    IIC_Wait_Ack();

    IIC_Send_Byte(on&0xFF);
    IIC_Wait_Ack();

    IIC_Send_Byte(on>>8);
    IIC_Wait_Ack();

    IIC_Send_Byte(off&0xFF);
    IIC_Wait_Ack();

    IIC_Send_Byte(off>>8);
    IIC_Wait_Ack();

    IIC_Stop();
}

void PCA9685_setFreq(float freq)
{
    uint8 prescale,oldmode,newmode;

    double prescaleval;

    //freq *= 0.92;
    prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    prescale = floor(prescaleval+0.5f);
    oldmode = PCA9685_Read(PCA_Model);

    newmode = (oldmode&0x7F)|0x10;
    PCA9685_Write(PCA_Model,newmode);
    PCA9685_Write(PCA_Pre,prescale);
    PCA9685_Write(PCA_Model,oldmode);
    system_delay_ms(5);
    PCA9685_Write(PCA_Model,oldmode|0xa1);

}



void setAngle(uint8 num,uint8 angle)
{
    uint32 off = 0;
    off = (uint32)(118+angle*2.2);//158
    PCA9685_setPWM(num,0,off);
}
