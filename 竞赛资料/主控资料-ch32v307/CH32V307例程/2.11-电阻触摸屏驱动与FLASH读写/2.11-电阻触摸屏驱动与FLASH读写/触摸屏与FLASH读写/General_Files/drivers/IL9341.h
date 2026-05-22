#ifndef __IL9341_H
#define __IL9341_H

#include "debug.h"

//=================【背光控制】==================================================
#define  TFT_BL_PORT GPIOC
#define  TFT_BL_PIN GPIO_Pin_0
#define	 TFT_BL_Ctrl(n)	 GPIO_WriteBit(TFT_BL_PORT, TFT_BL_PIN, n);

/*=============================================================================
 * 硬件连接：
 * LCD_CS<--->FSMC_NE1  LCD_RS<--->FSMC_A16  LCD_WR<--->FSMC_NWE   LCD_RD<--->FSMC_NOE
 * DB[15:0]<--->FSMC_D[15:0]
 *1x16MB:块1_NOR/PSRAM:0x6000 0000 ~ 0x60FF FFFF 16bit模式
 *地址线有A0-A23共24根，数据线有D0-D15共16根，HADDR[23:1]与 FSMC_A[22:0]对应相连，HADDR[0]未接
 *命令地址 = 0x6000 0000 = 0110 0000 0000 0000 0000 0000 0000 0000
 *数据地址 = 0x6002 0000 = 0110 0000 0000 0010 0000 0000 0000 0000(命令地址+(1<<(16+1)))
 *===========================================================================*/
#define FSMC_CMD_ADDR   (u32)0x60000000  //FSMC_Bank1_NOR/PSRAM CMD
#define FSMC_DATA_ADDR  (u32)0x60020000  //FSMC_Bank1_NOR/PSRAM Data A16(<<1)
//FSMC读写
//#define FSMC_WR_REG(CMD)      *((vu16 *)FSMC_CMD_ADDR)  = CMD
//#define FSMC_WR_DATA(DATA)    *((vu16 *)FSMC_DATA_ADDR) = DATA
//#define FSMC_RD_DATA()        *((vu16 *)FSMC_DATA_ADDR)

/* LCD addr */
typedef struct
{
    vu16 LCD_REG;
    vu16 LCD_RAM;
} LCD_TypeDef;
/*=============================================================================
 * 0x0001 FFFE + 0x02 = 0010 0000 0000 0000 0000，1在17bit
 ============================================================================*/
#define LCD_BASE        ((u32)(0x60000000 | 0x0001FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//FSMC读写
#define FSMC_WR_REG(CMD)    LCD->LCD_REG = CMD
#define FSMC_WR_DATA(DATA)  LCD->LCD_RAM = DATA
#define FSMC_RD_DATA()      LCD->LCD_RAM

//显示方向
//#define LCD_DIR   1  //0°
#define LCD_DIR   2  //90°
//#define LCD_DIR   3  //180°
//#define LCD_DIR   4  //270°

#if (LCD_DIR == 1)||(LCD_DIR == 3)
    #define LCD_WIDTH               240
    #define LCD_HEIGHT              320
#else
    #define LCD_WIDTH               320
    #define LCD_HEIGHT              240
#endif

//LCD命令
#define LCD_CMD_SETxOrgin	0x2A
#define LCD_CMD_SETyOrgin   0x2B
#define LCD_CMD_WRgram      0x2C
#define LCD_CMD_RDgram      0x2E

//字体大小
#define IL9341_SIZE_ASCII_16x8  16
#define IL9341_SIZE_ASCII_24x12 24
#define IL9341_SIZE_CHN_24x24   24

//颜色
typedef enum
{
    color_BLACK   = 0x0000,	   //黑色
	color_WHITE   = 0xFFFF,	   //白色
	color_RED     = 0xF800,	   //红色
	color_GREEN   = 0x07E0,	   //绿色
	color_BLUE    = 0x001F,	   //蓝色
	color_YELLOW  = 0xFFE0,    //黄色
	color_GRAY    =	0X8430,    //灰色
    color_MAGENTA = 0xF81F,    //洋红色
    color_CYAN    = 0x07FF,    //青色
    color_BROWN   = 0xA145,    //棕色
    color_LIGHTBLUE = 0x841F,  //浅绿
}color_t;//RGB565

void IL9341_Init(void);
void IL9341_FillColor(unsigned int x, unsigned int y, unsigned int width, unsigned int height, color_t color);
void IL9341_DrawPic(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char *_data);
void IL9341_DMA_DrawPic(const unsigned char *_data);
void IL9341_ShowChar(unsigned int x, unsigned int y, unsigned char chr, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE);
void IL9341_ShowNum(unsigned int x, unsigned int y, signed long num, unsigned char len, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE);
void IL9341_ShowNumFormat(unsigned int x, unsigned int y, unsigned long num, char *str, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE);
void IL9341_ShowNumf(unsigned int x, unsigned int y, float num, unsigned char len, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE);
void IL9341_ShowString(unsigned int x, unsigned int y, const char *chr, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE);
void IL9341_ShowCHN(unsigned int x, unsigned int y,unsigned char *chr, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE);
unsigned int IL9341_GetPiexlColor(unsigned int x, unsigned int y);
void IL9341_DrawDot(unsigned int x, unsigned int y, color_t color, unsigned char bold);
void IL9341_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, color_t color, unsigned char bold);
void IL9341_DrawRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, color_t color, unsigned char bold, FunctionalState NewState);
void IL9341_DrawCircle(unsigned int x, unsigned int y, unsigned int radius, color_t color, unsigned char bold, FunctionalState fill);
uint32_t IL9341_ReadID(void);

void IL9341_ToGrayscale(const unsigned char *data);
void IL9341_ToBinarization(const unsigned char *data, unsigned char level);
void IL9341_Scale1_2(const unsigned char *data);

#endif/*__IL9341_H*/
