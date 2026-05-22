#include "system.h"
#include "debug.h"
#include "../General_Files/drivers/IL9341.h"
#include "../General_Files/drivers/touch.h"

void DrawPic_Init()
{
    IL9341_FillColor(24*0, LCD_HEIGHT - 24, 24, 24, color_RED);
    IL9341_FillColor(24*1, LCD_HEIGHT - 24, 24, 24, color_GREEN);
    IL9341_FillColor(24*2, LCD_HEIGHT - 24, 24, 24, color_BLUE);
    IL9341_FillColor(24*3, LCD_HEIGHT - 24, 24, 24, color_YELLOW);
    IL9341_FillColor(24*4, LCD_HEIGHT - 24, 24, 24, color_MAGENTA);
    IL9341_FillColor(24*5, LCD_HEIGHT - 24, 24, 24, color_CYAN);
    IL9341_FillColor(24*6, LCD_HEIGHT - 24, 24, 24, color_BROWN);
    IL9341_FillColor(24*7, LCD_HEIGHT - 24, 24, 24, color_LIGHTBLUE);
    IL9341_FillColor(24*8, LCD_HEIGHT - 24, 24, 24, color_GRAY);
    IL9341_FillColor(24*9, LCD_HEIGHT - 24, 24, 24, color_BLACK);
    IL9341_FillColor(24*10, LCD_HEIGHT - 24, 24, 24, color_WHITE);
    IL9341_ShowString(24*11, LCD_HEIGHT - 2*16, "X:", color_WHITE, color_BLACK, IL9341_SIZE_ASCII_16x8);
    IL9341_ShowString(24*11, LCD_HEIGHT - 1*16, "Y:", color_WHITE, color_BLACK, IL9341_SIZE_ASCII_16x8);
}

void System_Init()
{
    IL9341_Init();
    TFT_Touch_Init();
    Touch_Para_Restore();//恢复触摸屏参数
    if(GPIO_ReadInputDataBit(TOUCH_PEN_PORT, TOUCH_PEN_PIN) == RESET)touch_para.calibrate_flag = RESET;//如果开机时屏幕处于触摸状态则进行校准
    while(GPIO_ReadInputDataBit(TOUCH_PEN_PORT, TOUCH_PEN_PIN) == RESET);//停止触摸进入校准
    while(touch_para.calibrate_flag == RESET)Touch_Calibrate();
    Touch_Para_Save();//保存触摸屏参数
    DrawPic_Init();//开机时就显示画板
}

void System_Loop()
{
    if(Touch_Get_XY())
    {
        static unsigned int color;
        if(touch_Axias.Y < 24 && touch_Axias.Y > 0)//如果触摸的位置是屏幕底部
        {
            DrawPic_Init();//刷新画板
            color = IL9341_GetPiexlColor(touch_Axias.X, LCD_HEIGHT - touch_Axias.Y);//获取触摸点颜色

            //显示方框以指示选中哪个颜色
            if(touch_Axias.X < 24)IL9341_DrawRectangle(0, LCD_HEIGHT - 24, 24, 24, color_GREEN, 2, DISABLE);
            else if(touch_Axias.X < 48)IL9341_DrawRectangle(24, LCD_HEIGHT - 24, 24, 24, color_BLUE, 2, DISABLE);
            else if(touch_Axias.X < 72)IL9341_DrawRectangle(48, LCD_HEIGHT - 24, 24, 24, color_YELLOW, 2, DISABLE);
            else if(touch_Axias.X < 96)IL9341_DrawRectangle(72, LCD_HEIGHT - 24, 24, 24, color_MAGENTA, 2, DISABLE);
            else if(touch_Axias.X < 120)IL9341_DrawRectangle(96, LCD_HEIGHT - 24, 24, 24, color_CYAN, 2, DISABLE);
            else if(touch_Axias.X < 144)IL9341_DrawRectangle(120, LCD_HEIGHT - 24, 24, 24, color_BROWN, 2, DISABLE);
            else if(touch_Axias.X < 168)IL9341_DrawRectangle(144, LCD_HEIGHT - 24, 24, 24, color_LIGHTBLUE, 2, DISABLE);
            else if(touch_Axias.X < 192)IL9341_DrawRectangle(168, LCD_HEIGHT - 24, 24, 24, color_GRAY, 2, DISABLE);
            else if(touch_Axias.X < 216)IL9341_DrawRectangle(192, LCD_HEIGHT - 24, 24, 24, color_BLACK, 2, DISABLE);
            else if(touch_Axias.X < 240)IL9341_DrawRectangle(216, LCD_HEIGHT - 24, 24, 24, color_WHITE, 2, DISABLE);
            else if(touch_Axias.X < 264)IL9341_DrawRectangle(240, LCD_HEIGHT - 24, 24, 24, color_RED, 2, DISABLE);
            else if(touch_Axias.X < 320)IL9341_FillColor(0, 0, LCD_WIDTH, LCD_HEIGHT - 24, color_WHITE);//如果触摸的是此区域，则清屏绘画区域

            //X部分在绘画区域，如果被清除需重新显示
            IL9341_ShowString(24*11, LCD_HEIGHT - 2*16, "X:", color_WHITE, color_BLACK, IL9341_SIZE_ASCII_16x8);
        }
        else IL9341_DrawDot(touch_Axias.X, LCD_HEIGHT - touch_Axias.Y, color, 5);//如果触摸的位置不是屏幕底部就直接画点，摸哪儿画哪儿

        //刷新触摸坐标值
        IL9341_ShowNum(24*11 + 16, LCD_HEIGHT - 2*16, touch_Axias.X, 3, color_WHITE, color_RED, IL9341_SIZE_ASCII_16x8);
        IL9341_ShowNum(24*11 + 16, LCD_HEIGHT - 1*16, touch_Axias.Y, 3, color_WHITE, color_BLUE, IL9341_SIZE_ASCII_16x8);
    }
}
