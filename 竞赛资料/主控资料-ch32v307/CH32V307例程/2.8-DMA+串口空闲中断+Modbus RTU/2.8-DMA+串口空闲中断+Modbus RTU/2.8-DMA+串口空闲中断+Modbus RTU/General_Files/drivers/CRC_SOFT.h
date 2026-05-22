#ifndef __CRC_SOFT_H
#define __CRC_SOFT_H

#define CRC8_POLYNOMIAL  0x31
#define CRC8_INITIALIZATION 0xFF
#define CRC8_FINALXOR 0x00
#define CRC8_REFLECT_INPUT 1
#define CRC8_REFLECT_OUTPUT 1

#define CRC16_POLYNOMIAL  0x8005
#define CRC16_INITIALIZATION 0xFFFF
#define CRC16_FINALXOR 0x0000
#define CRC16_REFLECT_INPUT 1
#define CRC16_REFLECT_OUTPUT 1

typedef struct
{
    unsigned char CRC16_H;
    unsigned char CRC16_L;
    unsigned char (*CRC8)(unsigned char*, unsigned char);
    void (*CRC16)(unsigned char*, unsigned char);
}CRC_SOFT_t;

extern CRC_SOFT_t CRC_SOFT;

#endif
