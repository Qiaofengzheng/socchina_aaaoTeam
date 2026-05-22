#include "system.h"
#include "debug.h"
#include  "../General_Files/drivers/SSD1306.h"
#include  "../General_Files/drivers/DS18B20.h"

void System_Init()
{
    DS18B20_Init();
    SSD1306_Init();
    //DS18B20_WriteUserEEPROM(-30,100);
}

void System_Loop()
{
    int8_t TH,TL;
    DS18B20_ReadUserEEPROM(&TL, &TH);
    SSD1306_ShowNumf(0, 0, DS18B20_GetTemp(), 5, SSD1306_SIZE_16, SSD1306_PO);
    SSD1306_ShowNum(0, 2, TL, 3, SSD1306_SIZE_16, SSD1306_PO);
    SSD1306_ShowNum(0, 4, TH, 3, SSD1306_SIZE_16, SSD1306_PO);
}
