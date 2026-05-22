#include "system.h"
#include "debug.h"
#include  "../General_Files/drivers/SSD1306.h"
#include "../General_Files/apps/Modbus_RTU.h"
#include "../General_Files/drivers/DS18B20.h"

float temperature;

void System_Init()
{
    SSD1306_Init();
    DS18B20_Init();
    Modbus_RTU_Init(9600);
}

void System_Loop()
{
    temperature = DS18B20_GetTemp();
    SSD1306_ShowNumf(0, 0, temperature, 4, SSD1306_SIZE_16, SSD1306_PO);
}
