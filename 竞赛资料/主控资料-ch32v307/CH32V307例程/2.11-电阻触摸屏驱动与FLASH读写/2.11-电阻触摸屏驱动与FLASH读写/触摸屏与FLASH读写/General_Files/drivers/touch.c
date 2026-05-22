#include "touch.h"
#include "debug.h"
#include "../General_Files/drivers/IL9341.h"

void TFT_Touch_Init()
{
//============================【IL9341初始化】================================
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI3, ENABLE );

    GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( SPI_CS_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = TOUCH_PEN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( TOUCH_PEN_PORT   , &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = SPI_SCLK_PIN | SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( SPI_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( SPI_PORT, &GPIO_InitStructure );

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init( SPI3, &SPI_InitStructure );

    SPI_Cmd( SPI3, ENABLE );
}

static void XPT2046_Write_Byte(unsigned char _data)
{
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI3, _data);
}

static unsigned char XPT2046_Read_Byte()
{
    XPT2046_Write_Byte(0x00);//为了产生8个SCLK
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
    return (uint8_t)SPI_I2S_ReceiveData(SPI3);
}

static unsigned int Touch_Get_ADC_Average(unsigned char XT2046_CMD)
{
    uint8_t i, j;
    uint16_t temp[TOUCH_READ_TIMES], ret;

    for(i = 0; i < TOUCH_READ_TIMES; i++)//读取多次的数据
    {
        SPI_RESET_CS();
        XPT2046_Write_Byte(XT2046_CMD);//发送指令

        Delay_Us(1);//延时一段时间等待转换完成

        temp[i] = XPT2046_Read_Byte();
        temp[i] &= ~0x80;//复位最高位
        temp[i] <<= 8;//为下个字节留出空间
        temp[i] |= XPT2046_Read_Byte();//接收第二个字节
        temp[i] >>= 3;//舍弃低三位

        while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));//等待SPI通讯完成
        SPI_SET_CS();
    }

    //排序
    for(i = 0; i < TOUCH_READ_TIMES - 1; i++)//最后一个不需要排序了，所以-1
    {
        for(j = i + 1; j < TOUCH_READ_TIMES; j++)//前面已经排好了，所以i+1
        {
            if(temp[i] < temp[j])
            {
                ret = temp[i];
                temp[i]  = temp[j];
                temp[j]  = ret;
            }
        }
    }

    ret = 0;
    for(i = 1; i < TOUCH_READ_TIMES - 1; i++)ret += temp[i];//掐头去尾去极值并求和

    return  ret/(TOUCH_READ_TIMES - 2);//求平均，因为掐头去尾了，所以要-2
}

static unsigned char Touch_Get_ADC_XY(unsigned int *X, unsigned int *Y)
{
    uint16_t x_val[2], y_val[2];
    uint16_t x_err, y_err;

    //采样两次
    x_val[0] = Touch_Get_ADC_Average(XPT2046_X_CMD);
    y_val[0] = Touch_Get_ADC_Average(XPT2046_Y_CMD);
    x_val[1] = Touch_Get_ADC_Average(XPT2046_X_CMD);
    y_val[1] = Touch_Get_ADC_Average(XPT2046_Y_CMD);

    //计算2次采样的差值
    if(x_val[0] >= x_val[1])x_err = x_val[0] - x_val[1];
    else x_err = x_val[1] - x_val[0];
    if(y_val[0] >= y_val[1])y_err = y_val[0] - y_val[1];
    else y_err = y_val[1] - y_val[0];

    if(x_err > TOUCH_ERROR || y_err > TOUCH_ERROR)return ERROR;//超过允许误差就舍弃

    //自适应屏幕方向
#if (LCD_DIR == 1)||(LCD_DIR == 3)
    *X = (x_val[0] + x_val[1]) / 2;
    *Y = (y_val[0] + y_val[1]) / 2;
#else
    *Y = (x_val[0] + x_val[1]) / 2;
    *X = (y_val[0] + y_val[1]) / 2;
#endif

    //临界值处理
    if(*X > TOUCH_X_MAX || *X < TOUCH_X_MIN)return ERROR;
    if(*Y > TOUCH_Y_MAX || *Y < TOUCH_Y_MIN)return ERROR;

    return SUCCESS;
}

touch_para_t touch_para;
unsigned char Touch_Calibrate()
{
//============================【触摸校准】================================
//返回值：成功SUCCESS,失败ERROR
//======================================================================

    //5个校准位置，中间的用于校验
    unsigned int Calibrate_xy[5][2] =
    {
        {20,20},                        //左上
        {20,LCD_HEIGHT-20},             //左下
        {LCD_WIDTH-20,LCD_HEIGHT-20},   //右下
        {LCD_WIDTH-20,20},              //右上
        {LCD_WIDTH/2,LCD_HEIGHT/2}      //中心
    };

    unsigned int x_val[5], y_val[5];         //5个校准位置对应的坐标值
    uint16_t x_opposite[2], y_opposite[2];   //计算得到对角的坐标
    uint16_t x_opposite_avr, y_opposite_avr; //对角坐标的平均值，用于与屏幕中央的坐标值比较
    uint8_t  i;
    s16 j;

    //读取5个校准点的坐标值
    for(i = 0; i < 5; i++)
    {
        IL9341_FillColor(0, 0, LCD_WIDTH, LCD_HEIGHT, color_WHITE); //清屏
        while(1)
        {
            IL9341_DrawCircle(Calibrate_xy[i][0], Calibrate_xy[i][1], 10, color_RED, 1, ENABLE);//画圆
            if(Touch_Get_ADC_XY(&x_val[i],&y_val[i]))break;//如果获取到ADC值就退出while循环
        }
        Delay_Ms(300);//延时300ms后校准下一个位置
    }

    IL9341_FillColor(0, 0, LCD_WIDTH, LCD_HEIGHT, color_WHITE); //清屏

    //将正方形的4个校准点整合成对角两点，减小触摸误差,假设A(0,240),B(0,0),C(320,0),D(320,240)
    x_opposite[0] = (x_val[0] + x_val[1]) / 2;  //求出1和2校准点的x平均值，此时x0 = 0
    y_opposite[0] = (y_val[1] + y_val[2]) / 2;  //求出2和3校准点的y平均值，此时y0 = 0
    x_opposite[1] = (x_val[2] + x_val[3]) / 2;  //求出3和4校准点的x平均值，此时x1 = 320
    y_opposite[1] = (y_val[0] + y_val[3]) / 2;  //求出1和4校准点的y平均值，此时y1 = 240

    //计算对角两点的平均值
    x_opposite_avr = (x_opposite[0] + x_opposite[1]) / 2;
    y_opposite_avr = (y_opposite[0] + y_opposite[1]) / 2;

    //与第5个校准点校准值误差大于100则舍弃
    j = x_opposite_avr - x_val[4];
    if(j > 100 || j < -100)
    {
        IL9341_ShowString(0, 110, "Fail!", color_WHITE, color_BLACK, IL9341_SIZE_ASCII_24x12);
        Delay_Ms(2000);
        return ERROR;
    }
    j = y_opposite_avr - y_val[4];
    if(j > 100 || j < -100)
    {
        IL9341_ShowString(0, 110, "Fail!", color_WHITE, color_BLACK, IL9341_SIZE_ASCII_24x12);
        Delay_Ms(2000);
        return ERROR;
    }

    //计算比例因子
    touch_para.x_factor = (float)(LCD_WIDTH - 40)  / (x_opposite[1] - x_opposite[0]);
    touch_para.y_factor = (float)(LCD_HEIGHT - 40) / (y_opposite[1] - y_opposite[0]);
    if(touch_para.x_factor < 0)touch_para.x_factor = -touch_para.x_factor;
    if(touch_para.y_factor < 0)touch_para.y_factor = -touch_para.y_factor;

    //计算偏移量
    touch_para.x_offset = (uint8_t)(touch_para.x_factor * x_opposite_avr - LCD_WIDTH / 2);
    touch_para.y_offset = (uint8_t)(touch_para.y_factor * y_opposite_avr - LCD_HEIGHT / 2);

    IL9341_ShowString(0, 110, "Success!", color_WHITE, color_BLACK, IL9341_SIZE_ASCII_24x12);
    Delay_Ms(2000);

    touch_para.calibrate_flag = SET;    //校准完成
    IL9341_FillColor(0, 0, LCD_WIDTH, LCD_HEIGHT, color_WHITE);//清屏
    return SUCCESS;
}

touch_Axias_t touch_Axias = {0};
unsigned char Touch_Get_XY()
{
//============================【获取XY坐标】==============================
//返回值：成功SUCCESS,失败ERROR
//======================================================================
    if(GPIO_READ_PEN_PIN() == RESET)
    {
        if(Touch_Get_ADC_XY(&touch_Axias.ADC_X, &touch_Axias.ADC_Y) == ERROR)return ERROR;

        touch_Axias.X = touch_Axias.ADC_X * touch_para.x_factor - touch_para.x_offset;
        touch_Axias.Y = touch_Axias.ADC_Y * touch_para.y_factor - touch_para.y_offset;

        if(touch_Axias.X > LCD_WIDTH - 1)touch_Axias.X = LCD_WIDTH - 1;
        if(touch_Axias.Y > LCD_HEIGHT - 1)touch_Axias.Y = LCD_HEIGHT - 1;

#if LCD_DIR == 1
        touch_Axias.Y = LCD_HEIGHT - touch_Axias.Y;
#elif LCD_DIR == 3
        touch_Axias.X = LCD_WIDTH - touch_Axias.X;
#elif LCD_DIR == 4
        touch_Axias.X = LCD_WIDTH - touch_Axias.X;
        touch_Axias.Y = LCD_HEIGHT - touch_Axias.Y;
#endif
        return SUCCESS;
    }
    else return ERROR;
}


void Touch_Para_Save()
{
//============================【保存参数】==============================

    uint32_t buf[64] = {0};
    buf[0] = touch_para.calibrate_flag;
    buf[1] = touch_para.x_factor * 1000;
    buf[2] = touch_para.x_offset;
    buf[3] = touch_para.y_factor * 1000;
    buf[4] = touch_para.y_offset;

    //RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;//对HCLK时钟二分频，防止超过100MHz
    //__disable_irq();//禁止所有中断
    FLASH_Unlock_Fast();//解锁FLASH快速模式

    FLASH_ErasePage_Fast(TOUCH_SAVE_FLASH_ADDR);        //擦除TOUCH_SAVE_FLASH_ADDR的内容
    FLASH_ProgramPage_Fast(TOUCH_SAVE_FLASH_ADDR, buf); //向TOUCH_SAVE_FLASH_ADDR写入内容

    FLASH_Lock_Fast();//上锁FLASH快速模式
    //RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;//恢复HCLK时钟
    //__enable_irq();//恢复所有中断
}

void Touch_Para_Restore()
{
//============================【恢复参数】==============================

    //__disable_irq();//禁止所有中断
    FLASH_Unlock_Fast();//解锁FLASH快速模式

    touch_para.calibrate_flag = *(u32 *)(TOUCH_SAVE_FLASH_ADDR + 4 * 0);
    touch_para.x_factor = (float)*(u32 *)(TOUCH_SAVE_FLASH_ADDR + 4 * 1) / 1000;
    touch_para.x_offset = *(u32 *)(TOUCH_SAVE_FLASH_ADDR + 4 * 2);
    touch_para.y_factor = (float)*(u32 *)(TOUCH_SAVE_FLASH_ADDR + 4 * 3) / 1000;
    touch_para.y_offset = *(u32 *)(TOUCH_SAVE_FLASH_ADDR + 4 * 4);

    FLASH_Lock_Fast();//上锁FLASH快速模式
    //__enable_irq();//恢复所有中断
}
