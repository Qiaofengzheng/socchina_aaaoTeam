#include "zf_common_headfile.h"
int flag_mode1=0;
int mode = 0;
int i1=0;
char uart_Rx_BUF[15]={0};
int uart_data1,uart_data2,uart_data222,uart_data3,uart_data4,uart_data5,uart_data6,uart_data7,uart_data8;
int uart_fh1,uart_fh2,uart_fh3,uart_fh4=0;
int mode_uart = 0;
char sss;
int key1,key2=0;
int flag_mode;

int i2=0,i3=0,i4=0,i5=0,i6=0,i7=0;
int i = 0;
int Yuyin_RX_BUF[10]={0};
int Yuyin_data1;

uint8_t received_data = 0,received_data1 = 0;
uint8_t data_ready = 0;

#define CMD_BUF_SIZE 8
uint8_t cmd_buf[CMD_BUF_SIZE];
uint8_t cmd_index = 0;

int send5=0;
//char* uart_data222;
void ironhand_init()
{

//    exti_init(KEY1, EXTI_TRIGGER_RISING);                                       // 初始化 KEY1 为外部中断输入 上升沿触发
//    exti_init(KEY2, EXTI_TRIGGER_RISING);                                      // 初始化 KEY2 为外部中断输入 下降沿触发
//    exti_init(KEY3, EXTI_TRIGGER_RISING);                                         // 初始化 KEY3 为外部中断输入 双边沿触发
//    exti_init(KEY4, EXTI_TRIGGER_RISING);                                         // 初始化 KEY4 为外部中断输入 双边沿触发
//
//    interrupt_set_priority(KEY1_EXTI, 0);                                       // 设置 KEY1 对应外部中断的中断有先级为 0
//    interrupt_set_priority(KEY2_EXTI, 0);                                       // 设置 KEY2 对应外部中断的中断有先级为 1
//    interrupt_set_priority(KEY3_EXTI, 0);                                       // 设置 KEY3 对应外部中断的中断有先级为 2
//    interrupt_set_priority(KEY4_EXTI, 0);
}

int m16r10(char *strr)
{
    char *s=strr;
    int sum=0;
    for (int i=2;i>0;i--)
    {
        switch(s[i-1])
        {
            case'0':sum=sum+0;break;
            case'1':sum=sum+1;break;
            case'2':sum=sum+2;break;
            case'3':sum=sum+3;break;
            case'4':sum=sum+4;break;
            case'5':sum=sum+5;break;
            case'6':sum=sum+6;break;
            case'7':sum=sum+7;break;
            case'8':sum=sum+8;break;
            case'9':sum=sum+9;break;
            case'a':sum=sum+10;break;
            case'b':sum=sum+11;break;
            case'c':sum=sum+12;break;
            case'd':sum=sum+13;break;
            case'e':sum=sum+14;break;
            case'f':sum=sum+15;break;
        }
    }
    return sum;
}


void uaart()
{
    uart_data1=uart_read_byte(UART_8);


}
void uaaart()
{

    char c = USART_ReceiveData(UART6);

                if(c == '\n' || rx_index >= BUF_SIZE-1)
                {
                    rx_buf[rx_index] = '\0';
                    process_coordinates(rx_buf);

                    rx_index = 0;
                }
                else
                {
                    rx_buf[rx_index++] = c;
                }
              //  printf("RX: %c\n", c);  // 通过调试串口输出


}
void uaaaart()
{
    received_data = uart_read_byte(UART_5);
    if(cmd_index < CMD_BUF_SIZE-1)
    {
        cmd_buf[cmd_index++] = received_data ;

        if(received_data == '\n' || received_data == '\r')
        {
            cmd_buf[cmd_index] = '\0'; // 字符串结束符

            if(cmd_buf[0] == '5')
            {
                send_voice_command();
//
//                Beep_Off();
//                system_delay_ms(100);
//                Beep_On();
//                led();
            }
            cmd_index = 0;
        }
    }
    else
    {
               cmd_index = 0; // 防止溢出
          }
    data_ready = 1;  // 设置数据就绪标志
}
void send_voice_command()
{
    uart_write_byte(UART_4, '5');
   // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    uart_write_byte(UART_4, '\n');  // 添加换行符作为终止符

}

void uaaaart1()
{
    received_data1 = uart_read_byte(UART_1);
    if(cmd_index < CMD_BUF_SIZE-1)
    {
        cmd_buf[cmd_index++] = received_data1 ;

        if(received_data1 == '\n' || received_data1 == '\r')
        {
            cmd_buf[cmd_index] = '\0'; // 字符串结束符

            if(cmd_buf[0] == '3')
            {
                if(avg_temp<=25)
                   {
                        uart_write_byte(UART_1, '1');
                   }
                   else
                   {
                         uart_write_byte(UART_1, '2');
                   }
            }
            cmd_index = 0;
        }
    }
    else
    {
               cmd_index = 0; // 防止溢出
          }
    data_ready = 1;  // 设置数据就绪标志
}
///*接收语音数据*/
//void LD3320_uart7_interrupt_handler()
//{
//        Yuyin_RX_BUF[i1]=uart_read_byte(UART_6);
//        i1++;
//     if(Yuyin_RX_BUF[0]!=0x0E)//帧头·
//
//            i1=0;
//
//     if((i1==2)&&(Yuyin_RX_BUF[1]!=0x0F))//判断帧头·
//
//            i1=0;
//     if(i1>=3)//代表一帧数据完毕
//    {
//        Yuyin_data1=Yuyin_RX_BUF[2];
//        i1 = 0;
//    }
//}


//void working()
//{
//
//
//           if(flag_mode1==1)
//           {
//               tft180_show_int(20,20,mq7,4);
//           }
//           else if(flag_mode1==2)
//           {
//
//               tft180_show_int(0,30,mq7,5);
//            }
//           else if(flag_mode1==3)
//            {
//
//
//               tft180_show_int(0,45,mq4,5);
//             }
//           else if(flag_mode1==4)
//             {
//
//               tft180_show_int(0,60,mq2,5);
//
//              }
//           else if(flag_mode1==5)
//           {
//
//
//               tft180_show_int(30,75,status ,5);
//
//           }
//           else if(flag_mode1==6)
//            {
//               DHT11_Read();
//               //tft180_show_chinese(0,  16*1,   16, chinese_test[], 4, RGB565_BLUE);           // 显示汉字
//               tft180_show_int(0,60,DHT11.humidity,5);
//               tft180_show_int(0,75,DHT11.temperature,5);
//            }
//
//}
void key1_exti_handler (void)
{
    flag_mode1=1;   // 外部中断触发 标志位置位

}
void key2_exti_handler (void)
{
                                                     // 外部中断触发 标志位置位
}
void key3_exti_handler (void)
{
                                                       // 外部中断触发 标志位置位
}
void key4_exti_handler (void)
{
                                                        // 外部中断触发 标志位置位
}


