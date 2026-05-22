#include "system.h"
#include "debug.h"
#include  "../General_Files/drivers/DHT11.h"
#include "../General_Files/drivers/ST7789.h"

void System_Init()
{
    DHT11_Init();
    ST7789_Init();
}

void System_Loop()
{
    if(DHT11_Read() == 0)
    {
        ST7789_ShowCHN(0, 32, "Êª¶È£º", color_WHITE, color_BLUE, SIZE_CHN_32x32);
        ST7789_ShowString(3*32+7*16, 32, "%RH", color_WHITE, color_BLUE, SIZE_ASCII_16x32);
        ST7789_ShowNumf(96, 32, DHT11.humidity, 4, color_WHITE, color_BLACK, SIZE_ASCII_16x32);
        ST7789_ShowCHN(0, 64, "ÎÂ¶È£º", color_WHITE, color_BLUE, SIZE_CHN_32x32);
        ST7789_ShowNumf(96, 64, DHT11.temperature, 4, color_WHITE, color_BLACK, SIZE_ASCII_16x32);
        ST7789_ShowCHN(3*32+7*16, 64, "¡æ", color_WHITE, color_BLUE, SIZE_CHN_32x32);
    }
    Delay_Ms(2000);
}
