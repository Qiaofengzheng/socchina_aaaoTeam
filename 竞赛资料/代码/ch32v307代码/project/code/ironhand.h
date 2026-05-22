#ifndef IRONHAND_H_
#define IRONHAND_H_

#include "zf_common_headfile.h"

#define KEY1                    (E0 )                                           // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用
#define KEY2                    (E1 )                                           // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用
#define KEY3                    (E2 )                                           // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用
#define KEY4                    (E3 )                                           // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用

#define KEY1_EXTI               (EXTI0_IRQn)                                    // 对应外部中断的中断编号 在 mm32f3277gx.h 头文件中查看 IRQn_Type 枚举体
#define KEY2_EXTI               (EXTI1_IRQn)                                    // 对应外部中断的中断编号 在 mm32f3277gx.h 头文件中查看 IRQn_Type 枚举体
#define KEY3_EXTI               (EXTI2_IRQn)                                    // 对应外部中断的中断编号 在 mm32f3277gx.h 头文件中查看 IRQn_Type 枚举体
#define KEY4_EXTI               (EXTI3_IRQn)                                    // 对应外部中断的中断编号 在 mm32f3277gx.h 头文件中查看 IRQn_Type 枚举体

extern int yuyin;

int m16r10(char *strr);
void ironhand_init();
void oled_working();
void uaart();
void uaaart();
void uaaaart();
void uaaaart1();
void LD3320_uart7_interrupt_handler();
extern int uart_data1,uart_data2,uart_data222,uart_data3,uart_data4,uart_data5,uart_data6,uart_data7,uart_data8,flag_mode1,key1,key2;
extern char uart_Rx_BUF[15];
extern char x[4],y[4],sss;
extern int x1,x2,x3,x4,yy1,yy2,yy3,yy4,flag_mode;
extern int ws,uart_fh1,uart_fh2,uart_fh3,uart_fh4;
extern uint8_t received_data,received_data1;
extern uint8_t data_ready;
extern uint8_t cmd_buf[];
extern uint8_t cmd_index ;
extern int send5;
void working();
void heixian();
void key1_exti_handler (void);
void key2_exti_handler (void);
void key3_exti_handler (void);
void key4_exti_handler (void);
void send_voice_command();

#endif
