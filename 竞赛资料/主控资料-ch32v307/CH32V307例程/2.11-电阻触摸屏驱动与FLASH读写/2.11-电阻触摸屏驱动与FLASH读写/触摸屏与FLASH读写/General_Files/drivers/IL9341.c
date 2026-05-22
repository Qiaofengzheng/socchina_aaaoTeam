#include "IL9341.h"
#include "IL9341_font_ASCII.h"
#include "IL9341_font_CHN.h"

static void IL9341_DIR()
{
//============================【设置屏幕方向】============================

    switch(LCD_DIR)
    {
        case 1: FSMC_WR_REG(0x36); FSMC_WR_DATA(1<<3); break;
        case 2: FSMC_WR_REG(0x36); FSMC_WR_DATA((1<<3)|(1<<5)|(1<<6)); break;
        case 3: FSMC_WR_REG(0x36); FSMC_WR_DATA((1<<3)|(1<<7)|(1<<4)|(1<<6)); break;
        case 4: FSMC_WR_REG(0x36); FSMC_WR_DATA((1<<3)|(1<<7)|(1<<5)|(1<<4)); break;
        default:FSMC_WR_REG(0x36); FSMC_WR_DATA(1<<3); break;
    }
}

static void IL9341_MCU_Init()
{
//============================【MCU初始化】==============================
    //声明结构体
     GPIO_InitTypeDef GPIO_InitStructure = {0};
     FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure = {0};
     FSMC_NORSRAMTimingInitTypeDef  readWriteTiming = {0};
     FSMC_NORSRAMTimingInitTypeDef  writeTiming = {0};
     DMA_InitTypeDef DMA_InitStructure = {0};

     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC | RCC_AHBPeriph_DMA2, ENABLE);   //使能FSMC AHB总线时钟
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);   //使能GPIO APB2总线时钟

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置为复用（FSMC）推挽输出模式
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOD, &GPIO_InitStructure);

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置为复用（FSMC）推挽输出模式
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOE, &GPIO_InitStructure);

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//设置为复用（FSMC A16 - IL9341 DC）推挽输出模式
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOD, &GPIO_InitStructure);

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//设置为推挽（背光）输出模式
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOC, &GPIO_InitStructure);
     GPIO_ResetBits(GPIOC,GPIO_Pin_0);

     //==========================【FSMC读写时序配置】=====================================
     readWriteTiming.FSMC_AddressSetupTime = 0x01;      //地址建立时间
     readWriteTiming.FSMC_AddressHoldTime = 0x00;       //地址保持时间
     readWriteTiming.FSMC_DataSetupTime = 0x0f;         //数据建立时间
     readWriteTiming.FSMC_BusTurnAroundDuration = 0x00; //定义要配置的HCLK周期数
     readWriteTiming.FSMC_CLKDivision = 0x00;           //时钟分频系数
     readWriteTiming.FSMC_DataLatency = 0x00;           //数据延迟
     readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;//访问模式A

     writeTiming.FSMC_AddressSetupTime = 0x00;
     writeTiming.FSMC_AddressHoldTime = 0x00;
     writeTiming.FSMC_DataSetupTime = 0x03;
     writeTiming.FSMC_BusTurnAroundDuration = 0x00;
     writeTiming.FSMC_CLKDivision = 0x00;
     writeTiming.FSMC_DataLatency = 0x00;
     writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;

     FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;                     //使用块1
     FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;   //不复用数据地址线
     FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;               //存储器类型为SRAM
     FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;     //存储器数据宽度16bit
     FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;  //禁用突发访问模式
     FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;//等待信号极性
     FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; //在异步传输期间禁用等待信号，
     FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;               //不使用非对齐方式
     FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//FSMC等待信号在等待状态前激活
     FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;    //存储器写使能
     FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;           //不使用等待信号
     FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;        //读写使用不同的时序
     FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
     FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;       //读时序
     FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;               //写时序
     //=======================================================================================
     FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   //FSMC初始化
     FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);   //FSMC使能

     DMA_DeInit(DMA2_Channel5);
     DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&LCD->LCD_RAM;
     DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
     DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
     DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
     DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
     DMA_Init(DMA2_Channel5, &DMA_InitStructure);

     Delay_Ms(50);
}

void IL9341_Init(void)
{
//============================【IL9341初始化】================================
    IL9341_MCU_Init();

    FSMC_WR_REG(0xCF);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0xC9);
    FSMC_WR_DATA(0X30);

    FSMC_WR_REG(0xED);
    FSMC_WR_DATA(0x64);
    FSMC_WR_DATA(0x03);
    FSMC_WR_DATA(0X12);
    FSMC_WR_DATA(0X81);

    FSMC_WR_REG(0xE8);
    FSMC_WR_DATA(0x85);
    FSMC_WR_DATA(0x10);
    FSMC_WR_DATA(0x7A);

    FSMC_WR_REG(0xCB);
    FSMC_WR_DATA(0x39);
    FSMC_WR_DATA(0x2C);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x34);
    FSMC_WR_DATA(0x02);

    FSMC_WR_REG(0xF7);
    FSMC_WR_DATA(0x20);

    FSMC_WR_REG(0xEA);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x00);

    FSMC_WR_REG(0xC0);    //Power control
    FSMC_WR_DATA(0x1B);   //VRH[5:0]

    FSMC_WR_REG(0xC1);    //Power control
    FSMC_WR_DATA(0x00);   //SAP[2:0];BT[3:0]

    FSMC_WR_REG(0xC5);    //VCM control
    FSMC_WR_DATA(0x30);   //3F
    FSMC_WR_DATA(0x30);
    //3C
    FSMC_WR_REG(0xC7);    //VCM control2
    FSMC_WR_DATA(0XB7);
//    FSMC_WR_REG(0x36);    // Memory Access Control
//    FSMC_WR_DATA(0x08);

    FSMC_WR_REG(0x3A);
    FSMC_WR_DATA(0x55);

    FSMC_WR_REG(0xB1);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x1A);

    FSMC_WR_REG(0xB6);    // Display Function Control
    FSMC_WR_DATA(0x0A);
    FSMC_WR_DATA(0xA2);

    FSMC_WR_REG(0xF2);    // 3Gamma Function Disable
    FSMC_WR_DATA(0x00);

    FSMC_WR_REG(0x26);    //Gamma curve selected
    FSMC_WR_DATA(0x01);

    FSMC_WR_REG(0xE0);    //Set Gamma
    FSMC_WR_DATA(0x0F);
    FSMC_WR_DATA(0x2A);
    FSMC_WR_DATA(0x28);
    FSMC_WR_DATA(0x08);
    FSMC_WR_DATA(0x0E);
    FSMC_WR_DATA(0x08);
    FSMC_WR_DATA(0x54);
    FSMC_WR_DATA(0XA9);
    FSMC_WR_DATA(0x43);
    FSMC_WR_DATA(0x0A);
    FSMC_WR_DATA(0x0F);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x00);

    FSMC_WR_REG(0XE1);    //Set Gamma
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x15);
    FSMC_WR_DATA(0x17);
    FSMC_WR_DATA(0x07);
    FSMC_WR_DATA(0x11);
    FSMC_WR_DATA(0x06);
    FSMC_WR_DATA(0x2B);
    FSMC_WR_DATA(0x56);
    FSMC_WR_DATA(0x3C);
    FSMC_WR_DATA(0x05);
    FSMC_WR_DATA(0x10);
    FSMC_WR_DATA(0x0F);
    FSMC_WR_DATA(0x3F);
    FSMC_WR_DATA(0x3F);
    FSMC_WR_DATA(0x0F);

    FSMC_WR_REG(0x2B);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x01);
    FSMC_WR_DATA(0x3f);

    FSMC_WR_REG(0x2A);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0x00);
    FSMC_WR_DATA(0xef);

    FSMC_WR_REG(0x11); //Exit Sleep
    Delay_Ms(120);
    FSMC_WR_REG(0x29); //display on
    //=======================================================================================

    IL9341_DIR();   //IL9341方向设置
    IL9341_FillColor(0, 0, LCD_WIDTH, LCD_HEIGHT, color_WHITE); //填充白色（清屏）
    TFT_BL_Ctrl(ENABLE);    //打开背光
}

static void IL9341_SetWindows(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
//============================【设置窗口】================================
//参数：
//x:起始x坐标
//y:起始y坐标
//width:窗口宽度
//height:窗口高度
//======================================================================
    FSMC_WR_REG(LCD_CMD_SETxOrgin);
	FSMC_WR_DATA(x >> 8);
	FSMC_WR_DATA(0x00FF & x);
	FSMC_WR_DATA((x + width - 1) >> 8);
	FSMC_WR_DATA((x + width - 1) & 0xFF);

	FSMC_WR_REG(LCD_CMD_SETyOrgin);
	FSMC_WR_DATA(y >> 8);
	FSMC_WR_DATA(0x00FF & y);
	FSMC_WR_DATA((y + height - 1) >> 8);
	FSMC_WR_DATA((y + height - 1) & 0xFF);

	FSMC_WR_REG(LCD_CMD_WRgram); //开始写入GRAM
}

void IL9341_FillColor(unsigned int x, unsigned int y, unsigned int width, unsigned int height, color_t color)
{
//============================【填充屏幕】================================
//参数：
//x:起始x坐标
//y:起始y坐标
//width:窗口宽度
//height:窗口高度
//color:要填充的颜色(RGB565)
//======================================================================
    uint32_t i;
    IL9341_SetWindows(x, y, width, height);//设置显示范围
    for(i = 0; i < height * width; i++)FSMC_WR_DATA(color);
}

void IL9341_DrawPic(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char *_data)
{
//============================【画图】================================
//参数：
//x:起始x坐标
//y:起始y坐标
//width:窗口宽度
//height:窗口高度
//*_data:包含字模的一维数组地址
//===================================================================
    uint32_t i;
    IL9341_SetWindows(x, y, width, height);
    for(i = 0; i < width * height * 2; i += 2)
        FSMC_WR_DATA(_data[i] << 8 | _data[i + 1]);
}

void IL9341_DMA_DrawPic(const unsigned char *_data)
{
/*============================【DMA画图】================================
//参数：
 * *_data:包含字模的一维数组地址
 * 仅支持全屏绘图
 * DMA是小端模式，而取模软件生成的字模是大端模式，会导致RGB565的颜色错误
//===================================================================*/
    IL9341_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);

    DMA_Cmd(DMA2_Channel5, DISABLE);
    DMA_SetCurrDataCounter(DMA2_Channel5, 65534);
    DMA2_Channel5->PADDR = (u32)_data;
    DMA_Cmd(DMA2_Channel5, ENABLE);
    while(DMA_GetFlagStatus(DMA2_FLAG_TC5) != SET);
    DMA_ClearFlag(DMA2_FLAG_TC5);

    DMA_Cmd(DMA2_Channel5, DISABLE);
    DMA_SetCurrDataCounter(DMA2_Channel5, 11266);
    DMA2_Channel5->PADDR = (u32)(_data + 2*65534);
    DMA_Cmd(DMA2_Channel5, ENABLE);
    while(DMA_GetFlagStatus(DMA2_FLAG_TC5) != SET);
    DMA_ClearFlag(DMA2_FLAG_TC5);
}

void IL9341_ShowChar(unsigned int x, unsigned int y, unsigned char chr, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE)
{
//=========================【显示ASCII字符】==============================
//参数：
//x:起始x坐标
//y:起始y坐标
//chr:要显示的字符
//color_background:背景色(RGB565)
//color_foreground:前景色(RGB565)
//IL9341_SIZE:字体大小
//======================================================================
    unsigned char var, page, col;

    chr -= ' ';

    if(IL9341_SIZE == IL9341_SIZE_ASCII_16x8)
    {
        IL9341_SetWindows(x, y, 8, 16);
        for(page = 0; page < 16; page++)
        {
            var = ASCII_1608[chr][page];
            for(col = 0; col < 8; col++)
            {
                if(var & 0x01)FSMC_WR_DATA(color_foreground);
                else FSMC_WR_DATA(color_background);
                var >>= 1;
            }
        }
    }

    if(IL9341_SIZE == IL9341_SIZE_ASCII_24x12)
    {
        IL9341_SetWindows(x, y, 12, 24);
        for(page = 0; page < 48; page += 2)
        {
            var = ASCII_2412[chr][page];
            for(col = 0; col < 8; col++)
            {
                if(var & 0x01)FSMC_WR_DATA(color_foreground);
                else FSMC_WR_DATA(color_background);
                var >>= 1;
            }
            var = ASCII_2412[chr][page + 1];
            for(col = 0; col < 4; col++)
            {
                if(var & 0x01)FSMC_WR_DATA(color_foreground);
                else FSMC_WR_DATA(color_background);
                var >>= 1;
            }
        }
    }
}

static unsigned long Math_Pow(unsigned char y)//计算10的y次方（10^y）函数
{
    unsigned long result = 1;
    while(y--)result *= 10;
    return result;
}

void IL9341_ShowNum(unsigned int x, unsigned int y, signed long num, unsigned char len, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE)
{
//==============================【显示数字】==============================
//参数：
//x:起始x坐标
//y:起始y坐标
//num:要显示的数字，支持负数
//len:数字位数
//color_background:背景色(RGB565)
//color_foreground:前景色(RGB565)
//IL9341_SIZE:字体大小
//注意溢出问题
//======================================================================
    unsigned char i, temp, step = 0, zero_flag = 0;
    if(IL9341_SIZE == IL9341_SIZE_ASCII_16x8)step = 8;
    else if(IL9341_SIZE == IL9341_SIZE_ASCII_24x12) step = 12;
    if(num < 0)
    {
        num = -num;
        IL9341_ShowChar(x, y, '-', color_background, color_foreground, IL9341_SIZE);
        x += step;//指针移动
    }
    else
    {
        IL9341_ShowChar(x, y, ' ', color_background, color_foreground, IL9341_SIZE);
        x += step;//指针移动
    }
    for(i = 0;i < len;i++)//循环显示每一个字符
    {
        temp = (num / Math_Pow(len - i - 1)) % 10;//求出当前位的数值，假如数字为25，len为3，当i为0时temp = 0，当i为1时temp = 2，，当i为2时temp = 5
        if(zero_flag == 0 && i < (len - 1))//未进入过该判断语句且i不是最后一位时
        {
            if(temp == 0)
            {
                IL9341_ShowChar(x + step * i, y, ' ', color_background, color_foreground, IL9341_SIZE);
                continue;//跳出本次循环
            }
            else zero_flag = 1;//从此不进入该判断语句
        }
        IL9341_ShowChar(x + step * i, y, temp + '0', color_background, color_foreground, IL9341_SIZE);
    }
}

void IL9341_ShowNumFormat(unsigned int x, unsigned int y, unsigned long num, char *str, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE)
{
//===========================【显示数字带格式控制】==============================
//参数：
//x:起始x坐标
//y:起始y坐标
//num:要显示的数字
//str:格式控制符
//color_background:背景色(RGB565)
//color_foreground:前景色(RGB565)
//SIZE:字体大小
//注意溢出问题
//==========================================================================
    char s[100];
    sprintf(s, str, num);//整形转十六进制（字符串格式化）
    IL9341_ShowString(x, y, s, color_background, color_foreground, IL9341_SIZE);
}

void IL9341_ShowNumf(unsigned int x, unsigned int y, float num, unsigned char len, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE)
{
//==============================【显示小数】==============================
//参数：
//x:起始x坐标
//y:起始y坐标
//num:要显示的数字，两位小数，支持负数
//len:数字位数，包括小数位
//color_background:背景色(RGB565)
//color_foreground:前景色(RGB565)
//IL9341_SIZE:字体大小
//注意溢出问题
//======================================================================
    unsigned char i, temp, step = 0, zero_flag = 0;
    unsigned long num1;
    if(IL9341_SIZE == IL9341_SIZE_ASCII_16x8)step = 8;
    else if(IL9341_SIZE == IL9341_SIZE_ASCII_24x12) step = 12;
    if(num < 0)
    {
        num = -num;
        IL9341_ShowChar(x, y, '-', color_background, color_foreground, IL9341_SIZE);
        x += step;//指针移动
    }
    else
    {
        IL9341_ShowChar(x, y, ' ', color_background, color_foreground, IL9341_SIZE);
        x += step;//指针移动
    }
    num1 = num * 100;
    for(i = 0;i < len;i++)
    {
        temp = (num1 / Math_Pow(len - i - 1)) % 10;
        if(zero_flag == 0 && i < (len - 3))
        {
            if(temp == 0)
            {
                IL9341_ShowChar(x + step * i, y, ' ', color_background, color_foreground, IL9341_SIZE);
                continue;//跳出本次循环
            }
            else zero_flag = 1;//从此不进入该判断语句
        }
        if(i == (len - 2))
        {
            IL9341_ShowChar(x + step * i, y, '.', color_background, color_foreground, IL9341_SIZE);
            i++;
            len += 1;
        }
        IL9341_ShowChar(x + step * i, y, temp + '0', color_background, color_foreground, IL9341_SIZE);
    }
}

void IL9341_ShowString(unsigned int x, unsigned int y, const char *chr, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE)
{
//=========================【显示ASCII字符串】============================
//参数：
//x:起始x坐标
//y:起始y坐标
//*chr:要显示的字符串
//color_background:背景色(RGB565)
//color_foreground:前景色(RGB565)
//IL9341_SIZE:字体大小
//======================================================================
    while (*chr)
    {
        if(*chr == '\r' && *(chr + 1) == '\n')
        {
            x = 0;
            y += IL9341_SIZE;
            chr +=2 ;
        }
        if (x + IL9341_SIZE/2 > LCD_WIDTH)
        {
            x = 0;
            y += IL9341_SIZE;
        }
        if (y + IL9341_SIZE > LCD_HEIGHT)
        {
            x = 0;
            y = 0;
        }
        IL9341_ShowChar(x, y, *chr, color_background, color_foreground, IL9341_SIZE);
        chr++;
        x += IL9341_SIZE/2;
    }
}

void IL9341_ShowCHN(unsigned int x, unsigned int y, unsigned char *chr, color_t color_background, color_t color_foreground, unsigned char IL9341_SIZE)
{
//============================【显示中文字符串】=============================
//参数：
//x:起始x坐标
//y:起始y坐标
//*chr:要显示的字符串
//color_background:背景色(RGB565)
//color_foreground:前景色(RGB565)
//IL9341_SIZE:字体大小
//=========================================================================
    uint8_t var, page, col;
    uint16_t index;

    while(*chr != '\0')
    {
        if(*chr == '\r' && *(chr + 1) == '\n')
        {
            x = 0;
            y += IL9341_SIZE;
            chr += 2 ;
        }
        if (x + IL9341_SIZE > LCD_WIDTH)
        {
            x = 0;
            y += IL9341_SIZE;
        }
        if(IL9341_SIZE == IL9341_SIZE_CHN_24x24)
        {
            for(index = 0; index < (sizeof(CHN_24)/sizeof(FONT_CHN24_t)); index++)
            {
                if(CHN_24[index].index[0] == *chr && CHN_24[index].index[1] == *(chr + 1) && CHN_24[index].index[2] == *(chr + 2))
                {
                    IL9341_SetWindows(x, y, 24, 24);
                    for(page = 0; page < 72; page++)
                    {
                        var = CHN_24[index].code[page];
                        for(col = 0; col < 8; col++)
                        {
                            if(var & 0x01)FSMC_WR_DATA(color_foreground);
                            else FSMC_WR_DATA(color_background);
                            var >>= 1;
                        }
                    }
                    break;
                }
            }
        }
        chr += 3;
        x += IL9341_SIZE;
    }
}

unsigned int IL9341_GetPiexlColor(unsigned int x, unsigned int y)
{
//==========================【获取坐标像素颜色】===========================
//参数：
//x:起始x坐标
//y:起始y坐标
//返回值:
//像素点颜色
//======================================================================
    uint16_t red = 0, green = 0, blue = 0;

    if(x < LCD_WIDTH && y < LCD_HEIGHT)
    {
        IL9341_SetWindows(x, y, 1, 1);
        FSMC_WR_REG(LCD_CMD_RDgram);
        FSMC_RD_DATA();  //读Dummy数据，舍弃
        red = FSMC_RD_DATA();
        blue = FSMC_RD_DATA();
        green = FSMC_RD_DATA();
    }
    return  ((red>>11)<<11) | ((green>>10)<<5) | (blue>>11);
}

void IL9341_DrawDot(unsigned int x, unsigned int y, color_t color, unsigned char bold)
{
//============================【画点】=============================
//参数：
//x:起始x坐标
//y:起始y坐标
//color:颜色(RGB565)
//bold:粗细(像素)
//======================================================================
    uint16_t var;
    if(x < LCD_WIDTH && y < LCD_HEIGHT)
    {
        IL9341_SetWindows(x, y, bold, bold);
        for (var = 0; var < bold * bold; ++var)FSMC_WR_DATA(color);
    }
}

void IL9341_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, color_t color, unsigned char bold)
{
//============================【画线】=============================
//参数：
//x1:起始x坐标
//y1:起始y坐标
//x2:终止x坐标
//y2:终止y坐标
//color:颜色(RGB565)
//bold:粗细(像素)
//================================================================
	//#假设传入的参数为0，0，300，100
    uint16_t i, x, y;
    int16_t sError_X=0,sError_Y=0,sDelta_X,sDelta_Y,sDistance;
    int16_t sIncrease_X, sIncrease_Y;

    //计算坐标增量
    sDelta_X = x2 - x1;//#sDelta_X = 300
    sDelta_Y = y2 - y1;//#sDelta_Y = 100
    //当前坐标位置
    x = x1;//#0
    y = y1;//#0

    if(sDelta_X > 0)sIncrease_X = 1;
    else if(sDelta_X == 0)sIncrease_X = 0;
    else
    {
        sIncrease_X = -1;
        sDelta_X = -sDelta_X;
    }

    if(sDelta_Y > 0)sIncrease_Y = 1;
    else if(sDelta_Y == 0)sIncrease_Y = 0;
    else
    {
        sIncrease_Y = -1;
        sDelta_Y = -sDelta_Y;
    }
    //#sIncrease_X = 1  sIncrease_Y = 1

    //选取基本增量坐标轴
    if(sDelta_X > sDelta_Y)sDistance = sDelta_X;//#sDistance = 300
    else sDistance = sDelta_Y;

    //#sDelta_X = 300, #sDelta_Y = 100, sIncrease_X = 1, sIncrease_Y = 1, sDistance = 300
    //画线输出
    for(i = 0; i <= sDistance + 1; i++)
    {
        IL9341_DrawDot(x, y, color, bold);
        sError_X += sDelta_X;
        sError_Y += sDelta_Y;
        if(sError_X > sDistance)
        {
            sError_X -= sDistance;
            x += sIncrease_X;
        }
        if(sError_Y > sDistance)
        {
            sError_Y -= sDistance;
            y += sIncrease_Y;
        }
        //printf("i=%d, \tx=%d, \ty=%d, \tsError_X=%d, \tsError_Y=%d\r\n", i, x, y, sError_X, sError_Y);
    }
}

void IL9341_DrawRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, color_t color, unsigned char bold, FunctionalState fill)
{
//============================【画矩形】=============================
//参数：
//x:起始x坐标
//y:起始y坐标
//width:宽度
//height:高度
//color:颜色(RGB565)
//bold:粗细(像素)
//fill:是否填充(ENABLE OR DISABLE)
//==================================================================
    if(fill)IL9341_FillColor(x, y, width, height, color);
    else
    {
        IL9341_DrawLine(x, y, x + width - 1, y, color, bold);
        IL9341_DrawLine(x + width - 1, y, x + width - 1, y + height - 1, color, bold);
        IL9341_DrawLine(x + width - 1, y + height - 1, x, y + height - 1, color, bold);
        IL9341_DrawLine(x, y + height - 1, x, y, color, bold);
    }
}

void IL9341_DrawCircle(unsigned int x, unsigned int y, unsigned int radius, color_t color, unsigned char bold, FunctionalState fill)
{
//============================【画圆】=============================
//参数：
//x:起始x坐标
//y:起始y坐标
//radius:半径(像素)
//color:颜色(RGB565)
//bold:粗细(像素)
//fill:是否填充(ENABLE OR DISABLE)
//================================================================
    int16_t sCurrentX, sCurrentY;
    int16_t sError;

    sCurrentX = 0;
    sCurrentY = radius;
    sError = 3 - (radius << 1);   //判断下个点位置的标志

    while(sCurrentX <= sCurrentY)
    {
        int16_t sCountY = 0;
        if(fill)
        {
            for(sCountY = sCurrentX; sCountY <= sCurrentY; sCountY++)
            {
                IL9341_DrawDot(x + sCurrentX, y + sCountY, color, bold);
                IL9341_DrawDot(x - sCurrentX, y + sCountY, color, bold);
                IL9341_DrawDot(x - sCountY,   y + sCurrentX, color, bold);
                IL9341_DrawDot(x - sCountY,   y - sCurrentX, color, bold);
                IL9341_DrawDot(x - sCurrentX, y - sCountY, color, bold);
                IL9341_DrawDot(x + sCurrentX, y - sCountY, color, bold);
                IL9341_DrawDot(x + sCountY,   y - sCurrentX, color, bold);
                IL9341_DrawDot(x + sCountY,   y + sCurrentX, color, bold);
            }
        }
        else
        {
            IL9341_DrawDot(x + sCurrentX, y + sCurrentY, color, bold);
            IL9341_DrawDot(x - sCurrentX, y + sCurrentY, color, bold);
            IL9341_DrawDot(x - sCurrentY, y + sCurrentX, color, bold);
            IL9341_DrawDot(x - sCurrentY, y - sCurrentX, color, bold);
            IL9341_DrawDot(x - sCurrentX, y - sCurrentY, color, bold);
            IL9341_DrawDot(x + sCurrentX, y - sCurrentY, color, bold);
            IL9341_DrawDot(x + sCurrentY, y - sCurrentX, color, bold);
            IL9341_DrawDot(x + sCurrentY, y + sCurrentX, color, bold);
    }
        sCurrentX++;
        if(sError < 0)sError += (4*sCurrentX + 6);
        else
        {
            sError += (10 + 4*(sCurrentX - sCurrentY));
            sCurrentY--;
        }
    }
}

uint32_t IL9341_ReadID(void)
{
//=======================【获取IL9341的32位ID】========================
//返回值：IL9341的ID
//================================================================
    uint32_t LCD_id = 0;
    uint32_t temp[3];

    FSMC_WR_REG(0xD3);
    FSMC_RD_DATA();        // 第一个读取数据无效
    temp[0] = FSMC_RD_DATA()&0x00FF; // 只有低8位数据有效
    temp[1] = FSMC_RD_DATA()&0x00FF; // 只有低8位数据有效
    temp[2] = FSMC_RD_DATA()&0x00FF; // 只有低8位数据有效

    LCD_id = (temp[0] << 16) | (temp[1] << 8) | temp[2];

    return LCD_id;
}

void IL9341_ToGrayscale(const unsigned char *data)
{
/*==========================【灰度化】=============================
 * 参数:
 * data: 图像字模
 *
 * 灰度图像在黑色与白色之间一般有255级，也被称为256灰阶，灰度的特点是R = G = B
 * 一个合适的加权平均公式Y = 0.3R + 0.59G + 0.11B
 * 以0xFFFF为例，得出F = 49，但是RB只有5bit = 31，故需要先将0-49映射到0-31
 * 故Y = (31/49) * (0.3R + 0.59G + 0.11B), 将浮点运算转换成整数运算，再将除
 * 数进行2的指数幂近似得到Y = (190 * R + 370 * G + 70 * B) >> 10;
 * Y其实就是YCbCr色彩空间中的亮度信息(Y:灰度值;Cb:蓝色色度分量;Cr:红色色度分量)
 ================================================================*/
    uint32_t i;
    IL9341_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);
    for(i = 0; i < LCD_WIDTH * LCD_HEIGHT * 2; i += 2)
    {
        uint8_t R, G, B, Y;
        //提取RGB信息，
        R = data[i] >> 3;
        G = ((data[i] & 0x07) << 3) | (data[i + 1] >> 5);
        B = (data[i + 1] & 0x1F);

        //加权平均法，人眼对绿色最敏感，对蓝色最不敏感
        Y = (190 * R + 370 * G + 70 * B) >> 10;
        FSMC_WR_DATA(Y<<11|Y<<6|Y);//绿色是6bit，要多移一位
    }
}

void IL9341_ToBinarization(const unsigned char *data, unsigned char level)
{
/*============================【二值化】=============================
 * 参数:
* data: 图像字模
* level: 阈值，一般取值17
 ==================================================================*/
    uint32_t i;
    IL9341_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);
    for(i = 0; i < LCD_WIDTH * LCD_HEIGHT * 2; i += 2)
    {
        uint16_t R, G, B, F;
        R = data[i] >> 3;
        G = ((data[i] & 0x07) << 3) | (data[i + 1] >> 5);
        B = (data[i + 1] & 0x1F);

        F = (190 * R + 370 * G + 70 * B) >> 10;
        //判断阈值
        if(F <= level)F = 0x0000;
        else F = 0xFFFF;

        FSMC_WR_DATA(F);
    }
}

void IL9341_Scale1_2(const unsigned char *data)
{
/*============================【缩放0.5x】=============================
 * 参数:
* data: 图像字模
 ==================================================================*/
    uint32_t i, j;
    IL9341_SetWindows(0, 0, LCD_WIDTH/2, LCD_HEIGHT/2);
    for(i = 0; i < LCD_HEIGHT; i += 2)
    {
        for (j = 0; j < LCD_WIDTH * 2; j += 4)//一次2个字节，横向填充320*2个字节数据
            FSMC_WR_DATA(data[i * (LCD_WIDTH << 1) + j] << 8 | data[i * (LCD_WIDTH << 1) + j + 1]);
    }
}
