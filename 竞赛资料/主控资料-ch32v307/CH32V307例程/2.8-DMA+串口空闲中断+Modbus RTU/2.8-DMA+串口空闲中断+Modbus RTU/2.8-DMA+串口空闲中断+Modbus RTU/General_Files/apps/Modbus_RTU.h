#ifndef __MODBUS_RTU_H
#define __MODBUS_RTU_H

#define RS485_UART USART2

#define RS485_PORT   GPIOA
#define RS485_TX_PIN    GPIO_Pin_2
#define RS485_RX_PIN    GPIO_Pin_3

#define RS485_DE_PORT GPIOB
#define RS485_DE_PIN  GPIO_Pin_13

#define RS485_TX 1
#define RS485_RX 0

#define RX_BUFF_LENGTH 20
#define TX_BUFF_LENGTH 20

enum modbus_RTU_CMD
{
    Modbus_RTU_Addr = 1,
    Modbus_RTU_CMD_RD_REG = 0x03,
    Modbus_RTU_CMD_WR_REG = 0x06,
    Modbus_RTU_DeviceREG_LED1 = 40001,
    Modbus_RTU_DeviceREG_LED2 = 40002,
    Modbus_RTU_DeviceREG_LED3 = 40003,
    Modbus_RTU_DeviceREG_DS18B20 = 40004,
};

typedef struct
{
    unsigned char send_buff_p[TX_BUFF_LENGTH];
    unsigned char rec_buff_p[RX_BUFF_LENGTH];
}Modbus_RTU_Buff;

void Modbus_RTU_Init(unsigned int baudrate);
void Modbus_RTU_Protocol_Analysis();

#endif
