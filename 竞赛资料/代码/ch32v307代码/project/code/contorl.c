#include "zf_common_headfile.h"
int key11=0;
uint8 set_ceju=200,set_mq7=135,set_mq4=100,set_temp=24,set_semp=50;
 int CMD ;

#define BUF_SIZE 64
#define FACE_STABLE_THRESHOLD 5

char rx_buf[BUF_SIZE];
uint8_t rx_index = 0;
// 初始化 face_coords 为 0
int face_coords[FACE_STABLE_THRESHOLD][2] = {0};
int coord_index = 0;

#define VALID_COORDINATE(x) ((x) >= 0 && (x) <= 180)

float avg_temp;
//========================================================================// 人脸坐标范围

//==============================================================================

// 坐标映射函数
int map(int value, int in_min, int in_max, int out_min, int out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// 坐标平滑滤波
void smooth_coordinates(int x, int y, int *out_x, int *out_y) {
    face_coords[coord_index][0] = x;
    face_coords[coord_index][1] = y;
    coord_index = (coord_index + 1) % FACE_STABLE_THRESHOLD;

    int count = FACE_STABLE_THRESHOLD;
    // 如果是前几次调用，只计算已有数据
    if(coord_index < FACE_STABLE_THRESHOLD) {
        count = coord_index;
    }

    int sum_x = 0, sum_y = 0;
    for(int i=0; i<count; i++) {
        sum_x += face_coords[i][0];
        sum_y += face_coords[i][1];
    }

    *out_x = sum_x / count;
    *out_y = sum_y / count;
}

// 更新屏幕显示
//void update_display(int x, int y) {
//    printf("Updating Display: X=%d, Y=%d\n", x, y);  // 调试输出
//    char buf[32];
//    //tft180_clear();
//    sprintf(buf, "X: %d", x);
//    tft180_show_string(10,75, buf);  // 确保坐标值在屏幕范围内
//    sprintf(buf, "Y: %d", y);
//    tft180_show_string(10, 95, buf);
//
//    // 2. 显示当前舵机角度
//       sprintf(buf, "Servo X: %d", current_angle[0]);
//       tft180_show_string(10, 115, buf);
//       sprintf(buf, "Servo Y: %d", current_angle[1]);
//       tft180_show_string(10, 135, buf);
////       sprintf(buf, "Servo Z: %d", current_angle[2]);
////        tft180_show_string(10, 150, buf);
//       // 3. 更新舵机位置
//       update_servos_by_face(x, y);
//
//       // 调试输出
//       printf("Updating Display: X=%d, Y=%d\n", x, y);
//
//}

// 修改后的update_display函数
void update_display(int x, int y) {
    // 限制x,y范围在50-150
    x = (x < 20) ? 20 : ((x > 150) ? 150 : x);
    y = (y < 20) ? 20 : ((y > 150) ? 150 : y);

    printf("Updating Display: X=%d, Y=%d\n", x, y);
    char buf[32];

    // 显示X坐标，根据百位数字决定显示位数
    if((x /100)> 1) {
        sprintf(buf, "X: %2d", x);  // 三位数显示
    } else
    {
        sprintf(buf, "X: %3d", x);  // 两位数显示
    }
    tft180_show_string(10, 75, buf);

    // 显示Y坐标，根据百位数字决定显示位数
    if((y /100)> 1)
    {
        sprintf(buf, "Y: %2d", y);  // 三位数显示
    } else
    {
        sprintf(buf, "Y: %3d", y);  // 两位数显示
    }
    tft180_show_string(10, 95, buf);

    // 显示当前舵机角度
    if((current_angle[0] /100)>1) {
        sprintf(buf, "Servo X: %2d", current_angle[0]);
    } else {
        sprintf(buf, "Servo X: %3d", current_angle[0]);
    }
    tft180_show_string(10, 115, buf);

    if((current_angle[1] /100)>1){
        sprintf(buf, "Servo Y: %2d", current_angle[1]);
    } else {
        sprintf(buf, "Servo Y: %3d", current_angle[1]);
    }
    tft180_show_string(10, 135, buf);

    // 更新舵机位置
    update_servos_by_face(x, y);
    printf("Updating Display: X=%d, Y=%d\n", x, y);
}

int smooth_x=0, smooth_y=0;

void process_coordinates(char* data) {
    printf("Raw Data: %s\n", data);  // 检查原始数据是否正确（如 "100,200"）
    int x = 0, y = 0 ;  // 显式初始化为 -1，便于调试
    if (sscanf(data, "%d,%d", &x, &y) == 2) {
        printf("Parsed: X=%d, Y=%d\n", x, y);  // 确认解析是否成功

        // 确保坐标在50-150范围内
               x = (x < 50) ? 50 : ((x > 150) ? 150 : x);
               y = (y < 50) ? 50 : ((y > 150) ? 150 : y);

              // 映射到屏幕坐标（根据Python端逻辑调整）
              int screen_x = map(y, 0, 270, 0, 320);  // 水平方向
              int screen_y = map(x, 0, 270, 0, 240);  // 垂直方向
              printf("[DEBUG] Mapped: X=%d, Y=%d\n", screen_x, screen_y);

        smooth_coordinates(x, y, &smooth_x, &smooth_y);
        update_display(smooth_x, smooth_y);
        send_control(smooth_x,smooth_y);
       // tft180_show_string(80, 55,  "    ");

    } else {
        printf("Invalid Data: %s\n", data);  // 如果解析失败，打印错误
        // 可以选择不更新显示，或显示默认值（如 0,0）
    }
}

//void process_coordinates(char* data) {
//    // 清洗数据：只保留数字、逗号、负号（必须在开头）
//    char clean_data[BUF_SIZE] = {0};
//    int j = 0;
//    for (int i = 0; data[i] != '\0' && j < BUF_SIZE-1; i++) {
//        if (isdigit(data[i]) || data[i] == ',' || (data[i] == '-' && j == 0)) {
//            clean_data[j++] = data[i];
//        }
//    }
//    clean_data[j] = '\0';
//
//    // 解析并约束范围
//    int x = 90, y = 90; // 默认值
//    if (sscanf(clean_data, "%d,%d", &x, &y) == 2) {
//        x = (x < 0) ? 0 : ((x > 180) ? 180 : x); // 强制约束到0-180
//        y = (y < 0) ? 0 : ((y > 180) ? 180 : y);
//
//        printf("Valid: X=%d, Y=%d\n", x, y);
//        smooth_coordinates(x, y, &smooth_x, &smooth_y);
//    } else {
//        printf("Invalid: %s\n", clean_data);
//    }
//}

//void process_coordinates(char* data) {
//    // 先清洗数据（去除非法字符）
//    char clean_data[BUF_SIZE];
//    int clean_index = 0;
//    for(int i=0; data[i]!='\0' && clean_index<BUF_SIZE-1; i++) {
//        // 只保留数字、负号和逗号
//        if(isdigit(data[i]) || data[i]==',' || (data[i]=='-' && clean_index==0)) {
//            clean_data[clean_index++] = data[i];
//        }
//    }
//    clean_data[clean_index] = '\0';
//
//    // 解析数据
//    int x =0, y = 0;
//    if(sscanf(clean_data, "%d,%d", &x, &y) == 2) {
//        // 硬性约束范围（关键修改）
//        x = (x < 0) ? 0 : ((x > 180) ? 180 : x);
//        y = (y < 0) ? 0 : ((y > 180) ? 180 : y);
//
//        printf("Clean Data: %s -> Parsed: X=%d, Y=%d\n", clean_data, x, y);
//
//        // 只有有效数据才处理
//        if(VALID_COORDINATE(x) && VALID_COORDINATE(y)) {
//            smooth_coordinates(x, y, &smooth_x, &smooth_y);
//            update_display(smooth_x, smooth_y);
//        } else {
//            printf("Out of range: X=%d, Y=%d\n", x, y);
//        }
//    } else {
//        printf("Invalid Format: %s\n", clean_data);
//    }
//}

//=================================================================================

//===============================================================================



void key_control()
{
    key_scanner();
    key11=key1;
    key1=key_get_state(KEY_1);
    if(key1==0 && key11==2)
    {
        flag_mode++;
    }
}
void tft_oled()
{

    tft180_show_chinese (2, 5, 16, tft_chinese[0],4, RGB565_RED);//欢迎来到
           tft180_show_chinese (10, 27, 16, tft_chinese[8],6, RGB565_RED);//稳像云台控制系统
           tft180_draw_line(5,50, 120,50,RGB565_RED);      // 坐标 0,0 到 10,10 画一条红色的线
           tft180_show_string(0,55, "mode:");
//    //tft180_show_int(40,65,flag_mode,2);
//    //tft180_show_int(40,80,buff_mode,4);
////        tft180_show_chinese (0, 5, 16, tft_chinese[36],2, RGB565_RED);
//        tft180_show_chinese (35, 5, 16, tft_chinese[40],2, RGB565_RED);//欢迎来到
//       //tft180_show_chinese (5, 35, 16, tft_chinese[44],8, RGB565_RED);//智慧食安小系统
//        tft180_draw_line(5,60, 120, 60,RGB565_RED);      // 坐标 0,0 到 10,10 画一条红色的线
//        //tft180_show_chinese (0, 65, 16, tft_chinese[0],2, RGB565_RED);
////        tft180_show_int(40,65,flag_mode,2);
////    switch (flag_mode)
//    {
//        case 1:
//          //烟雾
//          tft180_show_chinese (0, 95, 16, tft_chinese[70],2, RGB565_RED);//烟雾
//          tft180_show_chinese (33, 95, 16, tft_chinese[32],2, RGB565_RED);//浓度
//          tft180_show_char(67, 95, ':');
//          tft180_show_int(77,95,mq7,5);
//          tft180_show_chinese (0, 125, 16, tft_chinese[66],2, RGB565_RED);//设定
//          tft180_show_chinese (33, 125, 16, tft_chinese[32],2, RGB565_RED);//浓度
//          tft180_show_char(67, 125, ':');
//          tft180_show_int(77,125,set_mq7,5);
//          break;
//        case 2:
//           //天然气
//           tft180_show_chinese (0, 125, 16, tft_chinese[66],2, RGB565_RED);//设定
//           tft180_show_chinese (33, 125, 16, tft_chinese[32],2, RGB565_RED);//浓度
//           tft180_show_char(67,125, ':');
//           tft180_show_int(77,125,set_mq4,5);
//           tft180_show_chinese (0, 95, 16, tft_chinese[74],5, RGB565_RED);//天然气浓度
//           tft180_show_char(77, 95, ':');
//           tft180_show_int(85,95,mq4,5);
//
//           break;
//        case 3:
//           //红外测距，数值变化区间 10-240，距离小于3厘米后数值锐减
//           tft180_show_chinese (0, 95,  16, tft_chinese[20],2, RGB565_RED);//红外
//           tft180_show_chinese (33, 95, 16, tft_chinese[24],2, RGB565_RED);//感应
//           tft180_show_char(67, 95, ':');
//           tft180_show_int(77,95,ceju,5);
////           tft180_show_int(77,125,s_ceju,5);
//           if(ceju<set_ceju)
//           {
//               tft180_show_chinese (33, 125, 16, tft_chinese[84],4, RGB565_RED);//物体靠近
//           }
//           else tft180_show_string(0, 125, "              ");
//           break;
//        case 4:
//            //火焰，等于1就没着火
//           tft180_show_chinese (0, 95,  16, tft_chinese[12],2, RGB565_RED);//火焰
//           tft180_show_chinese (33, 95, 16, tft_chinese[16],2, RGB565_RED);//情况
//           tft180_show_char(67, 95, ':');
////           tft180_show_int(77,95,status,5);
//           if(status==0)
//            {
//              tft180_show_chinese (33, 125, 16, tft_chinese[92],4, RGB565_RED);//存在火情
//              Beep_Config();
//             }
//           else
//           {
//              tft180_show_chinese (33, 125, 16, tft_chinese[100],3, RGB565_RED);//无火情
//              tft180_show_string(85, 125, "       ");
//           }
//           break;
//        case 5:
//           if(DHT11_Read() == 0)
//            {
//                tft180_show_chinese (0, 95, 16, tft_chinese[58],2, RGB565_RED);//当前
//                tft180_show_chinese (33, 95, 16, tft_chinese[4],2, RGB565_RED);//温度
//                tft180_show_char(67, 95, ':');
//                tft180_show_int(77,95,DHT11.temperature,5);
//                tft180_show_chinese (0, 125, 16, tft_chinese[66],2, RGB565_RED);//设定
//                tft180_show_chinese (33, 125, 16, tft_chinese[4],2, RGB565_RED);//温度
//                tft180_show_char(67, 125, ':');
//                tft180_show_int(77,125,set_temp,5);
//            }
//
//            if(temp>set_temp)//温度过高灯会亮
//            {
//                  led();
//            }
//            break;
//        case 6:
//            if(DHT11_Read() == 0)
//             {
//                tft180_show_chinese (0, 95, 16, tft_chinese[58],2, RGB565_RED);//当前
//                tft180_show_chinese (35, 95, 16, tft_chinese[8],2, RGB565_RED);//湿度
//                tft180_show_char(67, 95, ':');
//                tft180_show_int(77,95,DHT11.humidity,5);
//                tft180_show_chinese (0, 125, 16, tft_chinese[66],2, RGB565_RED);//设定
//                tft180_show_chinese (35, 125, 16, tft_chinese[8],2, RGB565_RED);//湿度
//                tft180_show_char(67, 125, ':');
//                tft180_show_int(77,125,set_semp,5);
//             }
//            if(semp>set_semp)//湿度过高灯会亮
//             {
//                led();
//             }
//            break;
//        }

}


void Int_To_Str(int x,char *Str)
{
     int t;
     char *Ptr,Buf[5];                 //定义一个字符串数组和字符串指针，
     int i = 0;                             //用于计数
         Ptr = Str;                           //内部指针指向外部指针，进行参数传递，是属于源参数传递（通过地址），
     if(x < 10)  // 当整数小于10，转换为0x格式
     {
         *Ptr ++ = '0';            //单个数字前面补“0”
         *Ptr ++ = x+0x30;
     }
     else
     {
         while(x > 0)
         {
             t = x % 10;
             x = x / 10;
             Buf[i++] = t+0x30;  // 通过计算把数字编成ASCII码形式
         }
         i -- ;
         for(;i >= 0;i --)  // 将得到的字符串倒序
           {
             *(Ptr++) = Buf[i];
           }
     }
     *Ptr = '\0';
}



void send_control(uint8 send_data,uint ssend_data)
{
    if (send_data == -1 || ssend_data == -1) {
        printf("Invalid data: x=%d, y=%d (skipping transmission)\n", send_data, ssend_data);
        return;
    }

    char send[6] = {0};  // 最大支持3位数 + 符号 + 终止符
    char ssend[6] = {0};

    Int_To_Str(send_data,send);
    Int_To_Str(ssend_data, ssend);
    //tft180_show_string(0, 20, te1);
    gpio_set_level(BLUETOOTH_CH9141_RTS_PIN, 0);
    bluetooth_ch9141_send_string ("mode:5 ");
    bluetooth_ch9141_send_string ("data-x");
    bluetooth_ch9141_send_string (":");
    bluetooth_ch9141_send_string(send);
    bluetooth_ch9141_send_string ("   ");
    Int_To_Str(ssend_data,ssend);
    bluetooth_ch9141_send_string ("data-y");
    bluetooth_ch9141_send_string (":");
    bluetooth_ch9141_send_string(ssend);
    bluetooth_ch9141_send_string ("\r\n");
    gpio_set_level(BLUETOOTH_CH9141_RTS_PIN, 1);
}
void celiang()
{
   // ceju=adc_mean_filter_convert(ADC1_IN4_A4,5);//红外测距
    system_delay_ms(10);
    mq7=adc_mean_filter_convert(ADC1_IN0_A0,5);//co
    system_delay_ms(10);
    mq4=adc_mean_filter_convert(ADC1_IN1_A1,5);//天然气
    system_delay_ms(10);
    mq2=adc_mean_filter_convert(ADC1_IN2_A2,5);//烟雾
    system_delay_ms(10);
    status = gpio_get_level(A3);//是否有火焰，是0否1
    system_delay_ms(10);
  //  temp=DHT11.temperature;
    system_delay_ms(10);
   // semp=DHT11.humidity;
}
void uart_control()
{
    if(uart_data1>48)
    {
        CMD=uart_data1-48;
        uart_data1=48;

    }


}


void tempcontorl()
{

       // 初始化温度传感器
       ADC_TemperatureSensor_Init();

       //float temp = Read_Temperature();

      avg_temp = Read_Temperature_Average(5);
       tft180_show_float(0, 80, avg_temp, 2, 3);

           system_delay_ms(1000);  // 每秒更新一次
       }


