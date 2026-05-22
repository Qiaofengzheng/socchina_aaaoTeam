#include "zf_common_headfile.h"
#include<stdio.h>
#include<math.h>
#include<string.h>
#include<stdlib.h>
int flag_num=1;
uint8 test=10;
int cmd1;
uint32_t last_pid_time = 0;
#define PID_INTERVAL 20 // 20ms控制周期


int main(void)
{
        clock_init(SYSTEM_CLOCK_144M);      // 务必保留，设置系统时钟。
//      debug_init();                       // 务必保留，本函数用于初始化MPU 时钟 调试串口
        Kang_init();
        uart_init(UART_8, 9600, UART8_MAP0_TX_C4, UART8_MAP0_RX_C5); //接收数字数据
        uart_init(UART_6, 115200, UART6_MAP3_TX_E10, UART6_MAP3_RX_E11);//接收坐标数据
        uart_init(UART_5, 9600, UART5_MAP0_TX_C12, UART5_MAP0_RX_D2);//接收语音数据
        uart_init(UART_4, 115200, UART4_MAP0_TX_C10, UART4_MAP0_RX_C11);//发送语音给python
        uart_init(UART_1, 19200, UART1_MAP0_TX_A9 , UART1_MAP0_RX_A10);//语音播报
        uart_rx_interrupt(UART_5, ENABLE);
         uart_rx_interrupt(UART_8, ENABLE);
        uart_rx_interrupt(UART_6, ENABLE);
        uart_rx_interrupt(UART_4, ENABLE);
        uart_rx_interrupt(UART_1, ENABLE);
        // 此处编写用户代码 例如外设初始化代码等

        // 替代 clock() 的调用
        tft180_show_chinese (2, 5, 16, tft_chinese[0],4, RGB565_RED);//欢迎来到
        tft180_show_chinese (10, 27, 16, tft_chinese[8],6, RGB565_RED);//稳像云台系统
        tft180_draw_line(5,50, 120,50,RGB565_RED);      // 坐标 0,0 到 10,10 画一条红色的线
        tft180_show_string(0,55, "mode:");
        Beep_Init();

        while(1)
        {

            uart_control();
            tft180_show_int(40,55,CMD,2);
            Process_Command(CMD);
            uaaart();
            //uaaaart();
            // 设置舵机
            Servo_Set_Angle(SERVO_X, current_angle[0]);
            Servo_Set_Angle(SERVO_Y, current_angle[1]);
            Servo_Set_Angle(SERVO_Z, current_angle[2]);

        }

   }


