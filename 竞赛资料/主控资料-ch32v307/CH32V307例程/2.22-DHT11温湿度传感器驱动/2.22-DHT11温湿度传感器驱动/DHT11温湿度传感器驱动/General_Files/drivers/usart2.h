#ifndef _USART2_H_
#define _USART2_H_

#include "ch32v30x.h"
#include "debug.h"
#include "stdio.h"
#include "string.h"
typedef struct{

    char buff[1024];
    u16 length;
}NB_Struct;

extern NB_Struct NB_msg;

void USART2_config(void);
u16 USART2_ReadData(void);
void NB_SendString(char *str);
void USART2_IT_Config(void);
void USART2_SendData(u8 ch);
#endif

