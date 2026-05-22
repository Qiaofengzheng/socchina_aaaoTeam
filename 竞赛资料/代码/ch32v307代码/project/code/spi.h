#ifndef SPI_H_
#define SPI_H_

#include "zf_common_headfile.h"


/*使用什么模式就将对应宏定义改成1,1为真，0为假*/
#define                 SPI_Mode_0         1
#define                 SPI_Mode_3               0

#define         SPI_CS_0()      gpio_set_level(A4, 0);  //CS
#define         SPI_CS_1()      gpio_set_level(A4, 1);


#define         SPI_SCK_0()       gpio_set_level(B13, 0);   //D0
#define         SPI_SCK_1()       gpio_set_level(B13, 1);

#define         SPI_MOSI_0()      gpio_set_level(B15, 0);   //D1
#define         SPI_MOSI_1()      gpio_set_level(B15, 1);

void Software_SPI_Init(void);
void Software_SPI_Write(int8 data);


#endif
