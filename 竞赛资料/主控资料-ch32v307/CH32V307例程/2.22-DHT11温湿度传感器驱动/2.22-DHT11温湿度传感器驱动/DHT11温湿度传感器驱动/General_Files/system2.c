//=============================================================================
//文件名称：main.c
//功能概要：基于物联网的环境监测系统设计
//调试方式：J-Link OB ARM SW方式 5MHz
//=============================================================================
//头文件
#include "system2.h"
#include "debug.h"
#include "ch32v30x.h"
#include "OLED.h"
#include "usart1.h"
#include "usart2.h"
#include "mq2.h"
#include "ST7789.h"
//=============================================================================
//文件名称：main
//功能概要：主函数
//参数说明：无
//函数返回：int
//=============================================================================

//=============================================================================
//变量名称：
//DMA_buf[0]  MQ2    烟雾
//DMA_buf[1]  MQ7    一氧化碳
//DMA_buf[2]  MQ135  空气质量
//hum         湿度
//tmp         温度
//=============================================================================

//int main(void)
//{
//    init_Config();                               //系统初始化
//    while(1)
//    {
//        if(KEY_RunTim>2000)                      //时间片
//        {
//            system_Config();                     //OLED显示内容
//            uint16_t sendbuf[4]={tmp,hum,DMA_buf[0],DMA_buf[1]}; //向NBIOT发送的内容
//            NB_Send_IOT1(sendbuf);               //将发送内容转成MODBUS协议模式
//            if(hum>threshold)                    //湿度阀值
//            {
//                PCout(13)=0;                     //开灯
//                PBout(9)= 0;                     //开风扇
//                PAout(8)=0;                      //开蜂鸣器
//                OLED_ShowString(3, 11, "open "); //OLED显示
//            }
//            else
//            {
//                PCout(13)=1;                     //关灯
//                PBout(9)= 1;                     //关风扇
//                PAout(8)=1;                      //关蜂鸣器
//                OLED_ShowString(3, 11, "close"); //OLED显示
//            }
//            KEY_RunTim=0;                        //将时间片复位
//        }
//    }
//}

void init_Config(void)
{
    USART2_config();                             //串口2    收发NBIOT的消息
    USART1_config();                             //串口1    打印
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组
//    GPIO_Configuration();                        //LED
//    Beep_Config();                               //蜂鸣器
//    FAN_Config();                                //风扇
    OLED_Init();                                 //OLED
    ADC1_Config();                               //ADC 转换  烟雾传感器
//    KEY_Config();                                //按键
//    SysTick_Config(72000);                       //1ms 更新、溢出

}

void system_Config(void)
{

        OLED_ShowString(2, 1, "q2:");
        OLED_ShowNum(2, 4, DMA_buf[0], 4);       //mq2
        OLED_ShowString(2, 10, "q7:");
        OLED_ShowNum(2, 13, DMA_buf[1], 4);      //mq7
        Delay_Ms(2000);
//        OLED_ShowString(3, 1, "q135:");
//        OLED_ShowNum(3, 6, DMA_buf[2], 4);       //mq135
//        OLED_ShowString(4, 1, "thred:");
//        OLED_ShowNum(4, 7, threshold, 2);
        ST7789_ShowString(3*32+7*16, 32, "q2:", color_WHITE, color_BLUE, SIZE_ASCII_16x32);
        ST7789_ShowNumf(96, 32,  DMA_buf[0], 4, color_WHITE, color_BLACK, SIZE_ASCII_16x32);
        ST7789_ShowString(3*32+7*16, 32, "q7:", color_WHITE, color_BLUE, SIZE_ASCII_16x32);
        ST7789_ShowNumf(96, 32,  DMA_buf[1], 4, color_WHITE, color_BLACK, SIZE_ASCII_16x32);
}




