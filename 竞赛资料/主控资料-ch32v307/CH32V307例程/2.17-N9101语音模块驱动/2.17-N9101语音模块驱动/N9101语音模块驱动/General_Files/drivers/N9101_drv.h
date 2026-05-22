#ifndef __N9101_DRV_H
#define __N9101_DRV_H

#define N9101_ONE_WIRE_PORT GPIOC
#define N9101_ONE_WIRE_PIN  GPIO_Pin_10

#define N9101_SDA_CTRL(n) GPIO_WriteBit(N9101_ONE_WIRE_PORT, N9101_ONE_WIRE_PIN, n);

//#define N9101_UART USART2
//#define N9101_UART_PORT     GPIOA
//#define N9101_UART_TX_Pin   GPIO_Pin_2
//#define N9101_UART_RX_Pin   GPIO_Pin_3

typedef enum
{
    N9101_CMD_VOL_Base          = (unsigned char)0xD2,
    N9101_CMD_VOL_MAX           = (unsigned char)0xF0,
    N9101_CMD_Disk_U            = (unsigned char)0xF1,
    N9101_CMD_Disk_TF           = (unsigned char)0xF2,
    N9101_CMD_MP3               = (unsigned char)0xF3,
    N9101_CMD_Disk_FLASH        = (unsigned char)0xF4,
    N9101_CMD_EQ_Next           = (unsigned char)0xF5,
    N9101_CMD_Cycle_Full        = (unsigned char)0xF6,
    N9101_CMD_Cycle_Single      = (unsigned char)0xF7,
    N9101_CMD_Music_Last        = (unsigned char)0xF8,
    N9101_CMD_Music_Next        = (unsigned char)0xF9,
    N9101_CMD_Music_Play        = (unsigned char)0xFA,
    N9101_CMD_Music_Pause       = (unsigned char)0xFB,
    N9101_CMD_Music_Stop        = (unsigned char)0xFC,
    N9101_CMD_File_Folder_Last  = (unsigned char)0xFD,
    N9101_CMD_File_Folder_Next  = (unsigned char)0xFE,
}N9101_CMD_t;

void N9101_OneWire_Init();
void N9101_OneWire_SendByte(N9101_CMD_t N9101_CMD);

#endif /* __N9101_DRV_H */


