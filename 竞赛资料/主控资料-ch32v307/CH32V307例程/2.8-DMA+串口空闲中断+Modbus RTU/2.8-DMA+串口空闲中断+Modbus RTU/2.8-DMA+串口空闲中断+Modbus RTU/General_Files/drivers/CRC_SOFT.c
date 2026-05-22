#include "CRC_SOFT.h"

static unsigned char CRC8(unsigned char *_data, unsigned char length);
static void CRC16(unsigned char *_data, unsigned char length);

CRC_SOFT_t CRC_SOFT = {0, 0, CRC8, CRC16};

static unsigned char CRC8(unsigned char *_data, unsigned char length)
{
    unsigned int CRC_value = CRC8_INITIALIZATION;//初始值
    unsigned char i, j;
    unsigned int tmp;

    if(CRC8_REFLECT_INPUT)//输入数据以字节单位反转
    {
        for (i = 0; i < length; ++i)
        {
            tmp = 0x00;
            for (j = 0; j < 8; ++j)
            {
                tmp <<= 1;     //向高位移动
                tmp |= (*(_data + i) & 0x01);//提取输入数据最低位
                *(_data + i) >>= 1;            //将最低位移出
            }
            *(_data + i) = tmp;
        }
    }
    for(i = 0;i < length;i++)
    {
        CRC_value ^= *(_data + i);
        for(j = 0;j < 8;j++)
        {
            if(CRC_value & 0x80)CRC_value = (CRC_value << 1) ^ CRC8_POLYNOMIAL;
            else CRC_value = CRC_value << 1;
        }
    }
    if (CRC8_REFLECT_OUTPUT)//输出反转,比如1011反转后就是1101
    {
        tmp = 0x00;
        for(i = 0; i < 8; i++)
        {
            tmp <<= 1;     //向高位移动
            tmp |= (CRC_value & 0x01);//提取输入数据最低位
            CRC_value >>= 1;            //将最低位移出
        }
        CRC_value = tmp;
    }
    if(CRC8_REFLECT_INPUT)
    {
        for (i = 0; i < length; ++i)
        {
            tmp = 0x00;
            for (j = 0; j < 8; ++j)
            {
                tmp <<= 1;     //向高位移动
                tmp |= (*(_data + i) & 0x01);//提取输入数据最低位
                *(_data + i) >>= 1;            //将最低位移出
            }
            *(_data + i) = tmp;
        }
    }
    CRC_value ^= CRC8_FINALXOR;//结果异或值
    return CRC_value;
}

static void CRC16(unsigned char *_data, unsigned char length)
{
    unsigned int CRC_value = CRC16_INITIALIZATION;//初始值
    unsigned int tmp;
    unsigned char i, j;

    if(CRC16_REFLECT_INPUT)//输入以字节单位反转
    {
        for (i = 0; i < length; ++i)
        {
            tmp = 0x0000;
            for (j = 0; j < 8; ++j)
            {
                tmp <<= 1;     //向高位移动
                tmp |= (*(_data + i) & 0x0001);//提取输入数据最低位
                *(_data + i) >>= 1;            //将最低位移出
            }
            *(_data + i) = tmp;
        }
    }
    for(i = 0;i < length;i++)
    {
        tmp = *(_data + i) << 8;
        for(j = 0;j < 8;j++)
        {
            if((CRC_value ^ tmp) & 0x8000)CRC_value = (CRC_value << 1) ^ CRC16_POLYNOMIAL;
            else CRC_value <<= 1;
            tmp <<= 1;
        }
    }
    if (CRC16_REFLECT_OUTPUT)//输出反转,比如1011反转后就是1101
    {
        tmp = 0x0000;
        for(i = 0; i < 16; i++)
        {
            tmp <<= 1;     //向高位移动
            tmp |= (CRC_value & 0x0001);//提取输入数据最低位
            CRC_value >>= 1;            //将最低位移出
        }
        CRC_value = tmp;
    }
    CRC_value ^= CRC16_FINALXOR;//结果异或值
    CRC_SOFT.CRC16_H = CRC_value >> 8;
    CRC_SOFT.CRC16_L = CRC_value;
    for (i = 0; i < length; ++i)//恢复数据
    {
        tmp = 0x0000;
        for (j = 0; j < 8; ++j)
        {
            tmp <<= 1;     //向高位移动
            tmp |= (*(_data + i) & 0x0001);//提取输入数据最低位
            *(_data + i) >>= 1;            //将最低位移出
        }
        *(_data + i) = tmp;
    }
}
