#include "system.h"
#include "debug.h"
#include  "oled.h"
#include "openmv.h"
#include "usart.h"
void System_Init()
{
    OLED_Init();
    OLED_Clear();
//    OLEDGPIO_Init();
}

void System_Loop()
{

    OLED_ShowString(0,1,"data:",16);
    OLED_ShowNum(56,2,data2,3);
    Delay_Ms(1000);

}
