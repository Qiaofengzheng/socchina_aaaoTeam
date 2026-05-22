#ifndef __ST7789_H
#define __ST7789_H			

#include "debug.h"
//============================【参数设置】=================================

//显示方向
#define ST7789_DIR   1  //0°
//#define ST7789_DIR   2  //90°
//#define ST7789_DIR   3  //180°
//#define ST7789_DIR   4  //270°
#if ST7789_DIR == 2 || ST7789_DIR == 4
	#define ST7789_WIDTH 172
	#define ST7789_HEIGHT 320
#else
	#define ST7789_WIDTH 320
	#define ST7789_HEIGHT 172
#endif

#define ST7789_CMD  0       //写命令
#define ST7789_DATA 1       //写数据

//============================【引脚定义】=================================
#define  ST7789_BL_PORT GPIOE
#define  ST7789_BL_PIN GPIO_Pin_3
#define  ST7789_BL_Ctrl(n)  GPIO_WriteBit(ST7789_BL_PORT, ST7789_BL_PIN, n);

#define SPI_PORT         GPIOB
#define SPI_SCLK_PIN     GPIO_Pin_3
#define SPI_MOSI_PIN     GPIO_Pin_5//sda
#define SPI_MISO_PIN     GPIO_Pin_4

#define I8080_DC_PORT    GPIOE
#define I8080_DC_PIN     GPIO_Pin_2

#define ST7789_CS_PORT        GPIOA
#define ST7789_CS_PIN         GPIO_Pin_15
#define GT30L32S4W_CS_PORT    GPIOE
#define GT30L32S4W_CS_PIN     GPIO_Pin_1

#define ST7789_SET_CS()        GPIO_WriteBit(ST7789_CS_PORT, ST7789_CS_PIN, Bit_SET)
#define ST7789_RESET_CS()      GPIO_WriteBit(ST7789_CS_PORT, ST7789_CS_PIN, Bit_RESET)
#define GT30L32S4W_SET_CS()    GPIO_WriteBit(GT30L32S4W_CS_PORT, GT30L32S4W_CS_PIN, Bit_SET)
#define GT30L32S4W_RESET_CS()  GPIO_WriteBit(GT30L32S4W_CS_PORT, GT30L32S4W_CS_PIN, Bit_RESET)

#define I8080_DC(n)         GPIO_WriteBit(I8080_DC_PORT, I8080_DC_PIN, n)

//颜色
typedef enum
{
    color_WHITE = 0xFFFF, //白色
    color_BLACK = 0x0000, //黑色
    color_BLUE = 0x001F, //蓝色
    color_BRED = 0XF81F, //紫色
    color_GRED = 0XFFE0, //黄绿色
    color_GBLUE = 0X07FF, //天蓝色
    color_RED = 0xF800, //红色
    color_GREEN = 0x07E0, //绿色
    color_CYAN = 0x7FFF, //天蓝色
    color_YELLOW = 0xFFE0, //黄色
    color_BROWN = 0XBC40, //棕色
    color_BRRED = 0XFC07, //棕红色
    color_GRAY = 0X8430, //灰色
    color_DARKBLUE = 0X01CF,//深蓝色
    color_LIGHTBLUE = 0X7D7C, //浅蓝色
    color_GRAYBLUE = 0X5458, //灰蓝色
    color_LIGHTGREEN = 0X841F, //浅绿色
    color_LGRAY = 0XC618, //浅灰色(PANNEL),窗体背景色
    color_LGRAYBLUE = 0XA651, //浅灰蓝色(中间层颜色)
    color_LBBLUE = 0X2B12, //浅棕蓝色(选择条目的反色)
}color_t;//RGB565


typedef enum
{
//GB2312
    SIZE_CHN_12x12 = 0x000000,
    SIZE_CHN_16x16 = 0x02C9D0,
    SIZE_CHN_24x24 = 0x068190,
    SIZE_CHN_32x32 = 0x0EDF00,

    SIZE_ASCII_5x7 = 0x1DDF80,
    SIZE_ASCII_7x8 = 0x1DE280,
    SIZE_ASCII_6x12 = 0x1DBE00,
    SIZE_ASCII_8x16 = 0x1DD780,
    SIZE_ASCII_12x24 = 0x1DFF00,
    SIZE_ASCII_16x32 = 0x1E5A50,
}GT30L32S4W_BaseADDR_t;

void ST7789_Init();
void ST7789_FillColor(unsigned int x, unsigned int y, unsigned int width, unsigned int height, color_t color);
void ST7789_DrawPic(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char *_data);
void ST7789_DrawDot(unsigned int x, unsigned int y, color_t color, unsigned char bold);
void ST7789_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, color_t color, unsigned char bold);
void ST7789_DrawRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, color_t color, unsigned char bold, FunctionalState NewState);
void ST7789_DrawCircle(unsigned int x, unsigned int y, unsigned int radius, color_t color, unsigned char bold, FunctionalState fill);
void ST7789_ShowChar(unsigned int x, unsigned int y, unsigned char chr, color_t color_background, color_t color_foreground, GT30L32S4W_BaseADDR_t SIZE);
void ST7789_ShowNum(unsigned int x, unsigned int y, signed long num, unsigned char len, color_t color_background, color_t color_foreground, GT30L32S4W_BaseADDR_t SIZE_);
void ST7789_ShowNumf(unsigned int x, unsigned int y, float num, unsigned char len, color_t color_background, color_t color_foreground, GT30L32S4W_BaseADDR_t SIZE);
void ST7789_ShowString(unsigned int x, unsigned int y, const char *chr, color_t color_background, color_t color_foreground, GT30L32S4W_BaseADDR_t SIZE);
void ST7789_ShowCHN(unsigned int x, unsigned int y, unsigned char *chr, color_t color_background, color_t color_foreground, GT30L32S4W_BaseADDR_t SIZE);

#endif
  
