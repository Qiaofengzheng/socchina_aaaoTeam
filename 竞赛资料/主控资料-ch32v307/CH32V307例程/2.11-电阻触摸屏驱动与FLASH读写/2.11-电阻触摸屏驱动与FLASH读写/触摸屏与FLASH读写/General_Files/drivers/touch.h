#ifndef __TOUCH_H
#define __TOUCH_H

//SPI管脚&触摸屏管脚
#define SPI_PORT GPIOB
#define SPI_SCLK_PIN GPIO_Pin_3
#define SPI_MOSI_PIN GPIO_Pin_5
#define SPI_MISO_PIN GPIO_Pin_4

#define SPI_CS_PORT GPIOC
#define SPI_CS_PIN GPIO_Pin_2

#define TOUCH_PEN_PORT GPIOB
#define TOUCH_PEN_PIN GPIO_Pin_15

#define  SPI_SET_CS() 	GPIO_WriteBit(SPI_CS_PORT, SPI_CS_PIN, Bit_SET)
#define  SPI_RESET_CS() GPIO_WriteBit(SPI_CS_PORT, SPI_CS_PIN, Bit_RESET)
#define  GPIO_READ_PEN_PIN() GPIO_ReadInputDataBit(TOUCH_PEN_PORT, TOUCH_PEN_PIN)
#define XPT2046_X_CMD        0xD0  //读取X轴命令,1 101 0 0 00
#define XPT2046_Y_CMD        0x90  //读取Y轴命令,1 001 0 0 00

#define  TOUCH_READ_TIMES   3     //一次读取触摸值的次数
#define  TOUCH_ERROR        15    //允许误差

#define  TOUCH_X_MAX        3700  //X轴最大值
#define  TOUCH_X_MIN        160   //X轴最小值
#define  TOUCH_Y_MAX        3800  //Y轴最大值
#define  TOUCH_Y_MIN        160   //Y轴最小值

#define TOUCH_SAVE_FLASH_ADDR 0x803E800//保存到0x803E800 – 0x803E8FF(250k - 250k+256)，共256字节

typedef struct 
{
    unsigned char calibrate_flag;   //校准标志位
    char x_offset;         //x轴偏移量，可能为负值
    char y_offset;         //y轴偏移量，可能为负值
	float   x_factor;               //x轴比例因子
	float   y_factor;               //y轴比例因子
}touch_para_t;

typedef struct
{
    int ADC_X;                      //x轴ADC采样值
    int ADC_Y;                      //y轴ADC采样值
    int X;                          //屏幕触摸x轴坐标
    int Y;                          //屏幕触摸y轴坐标
}touch_Axias_t;

void TFT_Touch_Init();
unsigned char Touch_Calibrate();
unsigned char Touch_Get_XY();
void Touch_Para_Save();
void Touch_Para_Restore();

extern touch_para_t touch_para;
extern touch_Axias_t touch_Axias;

#endif
