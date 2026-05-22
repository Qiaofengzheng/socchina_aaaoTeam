#include "zf_common_headfile.h"

// 全局变量
int target_angle[3] = {90, 120, 40};  // X/Y/Z目标角度
int current_angle[3] = {90, 120, 40}; // 当前角度
//=========================================================
// 在全局变量区域添加
#define SCREEN_X_MIN 0
#define SCREEN_X_MAX 270
#define SCREEN_Y_MIN 0
#define SCREEN_Y_MAX 240
#define SERVO_X_MIN 90   // X轴舵机最小角度
#define SERVO_X_MAX 120   // X轴舵机最大角度
#define SERVO_Y_MIN 100    // Y轴舵机最小角度
#define SERVO_Y_MAX 240   // Y轴舵机最大角度
#define SERVO_Z_MIN 30    // Z轴舵机最小角度(可选)
#define SERVO_Z_MAX 60   // Z轴舵机最大角度(可选)

// 平滑移动参数
#define SMOOTH_STEP 3     // 每次最大变化角度

//==================================================================================
// 坐标映射函数（带约束）
int map_constrain(int value, int in_min, int in_max, int out_min, int out_max) {
    value = (value < in_min) ? in_min : ((value > in_max) ? in_max : value);
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// 平滑移动函数
//void smooth_move_servos() {
//    for(int i = 0; i < 3; i++) {
//        int step = target_angle[i] - current_angle[i];
//        // 限制最大步长
//        step = (step > SMOOTH_STEP) ? SMOOTH_STEP :
//              ((step < -SMOOTH_STEP) ? -SMOOTH_STEP : step);
//        current_angle[i] += step;
//    }
//
//    // 更新舵机
//    Servo_Set_Angle(SERVO_X, current_angle[0]);
//    Servo_Set_Angle(SERVO_Y, current_angle[1]);
//    Servo_Set_Angle(SERVO_Z, current_angle[2]);
//}

//void smooth_move_servos() {
//    // 使用PID控制实现更平滑的移动
//    for(int i = 0; i < 3; i++) {
//        int error = target_angle[i] - current_angle[i];
//
//        // 根据误差大小动态调整步长
//        int step;
//        if(abs(error) > 20) {
//            step = error / 4; // 大误差时快速移动
//        } else if(abs(error) > 5) {
//            step = error / 2; // 中等误差
//        } else {
//            step = error; // 小误差时精确移动
//        }
//
//        current_angle[i] += step;
//    }
//
//    // 更新舵机
//    Servo_Set_Angle(SERVO_X, current_angle[0]);
//    Servo_Set_Angle(SERVO_Y, current_angle[1]);
//    Servo_Set_Angle(SERVO_Z, current_angle[2]);
//}

void smooth_move_servos() {
    // 使用动态步长实现更平滑的移动
    for(int i = 0; i < 3; i++) {
        int error = target_angle[i] - current_angle[i];

        // 根据误差大小动态调整步长
        int step;
        if(abs(error) > 20) {
            step = error / 2; // 大误差时快速移动
        } else if(abs(error) > 5) {
            step = error / 3; // 中等误差
        } else {
            step = (error != 0) ? (error > 0 ? 1 : -1) : 0; // 小误差时微调
        }

        current_angle[i] += step;
    }

    // 更新舵机
    Servo_Set_Angle(SERVO_X, current_angle[0]);
    Servo_Set_Angle(SERVO_Y, current_angle[1]);
    Servo_Set_Angle(SERVO_Z, current_angle[2]);
}
// 全局变量
//void update_servos_by_face(int face_x, int face_y)
//{
//    //static uint32_t last_time = 0;
//    static int prev_x = 160, prev_y = 120; // 初始化为屏幕中心
//
//    // 1. 计算时间差（简易版，假设每次调用间隔固定）
//    uint32_t time_elapsed = 33; // 假设30fps，约33ms/帧
//
//    // 2. 计算速度（像素/秒）
//    float speed_x = 1000.0f * abs(face_x - prev_x) / time_elapsed;
//    float speed_y = 1000.0f * abs(face_y - prev_y) / time_elapsed;
//    float avg_speed = (speed_x + speed_y) / 2.0f;
//
//    // 3. 控制Z轴（示例：速度>50px/s时触发）
//    target_angle[2] = (avg_speed > 50) ? 120 : 90; // 二值化控制
//
//    // 更新X/Y轴（原逻辑）
//    target_angle[0] = map_constrain(face_x, 0, 320, 30, 150);
//    target_angle[1] = map_constrain(face_y, 0, 240, 150, 30);
//
//    // 记录状态
//    prev_x = face_x;
//    prev_y = face_y;
//
//    smooth_move_servos();
//
//    // 调试输出
//    printf("Servo Targets: X=%d, Y=%d, Z=%d\n",
//           target_angle[0], target_angle[1], target_angle[2]);
//}

//void update_servos_by_face(int face_x, int face_y) {
//    //static int prev_x = SCREEN_X_MAX/2, prev_y = SCREEN_Y_MAX/2;
//
//    // 1. 计算人脸相对于屏幕中心的位置（归一化到-1到1范围）
//    float norm_x = (face_x - SCREEN_X_MAX/2) / (float)(SCREEN_X_MAX/2);
//    float norm_y = (SCREEN_Y_MAX/2 - face_y) / (float)(SCREEN_Y_MAX/2);
//
//    // 2. 计算各轴角度（关键修改）
//    // X轴：控制左右转动（人脸左右移动时X轴转动）
//    target_angle[0] = map_constrain(face_x, SCREEN_X_MIN, SCREEN_X_MAX, SERVO_X_MIN, SERVO_X_MAX);
//
//    // Y轴：控制上下转动（人脸上下移动时Y轴转动）
//    target_angle[1] = map_constrain(face_y, SCREEN_Y_MIN, SCREEN_Y_MAX, SERVO_Y_MIN, SERVO_Y_MAX);
//
//    // 3. Z轴协同控制（新算法）
//    // 根据X/Y偏移量计算Z轴补偿角度
//    float z_compensation = sqrt(norm_x*norm_x + norm_y*norm_y) * 30; // 补偿系数
//    target_angle[2] = 90 + z_compensation * (norm_x > 0 ? 1 : -1); // 根据X方向决定补偿方向
//
//    // 4. 限制角度范围
//    target_angle[0] = constrain(target_angle[0], SERVO_X_MIN, SERVO_X_MAX);
//    target_angle[1] = constrain(target_angle[1], SERVO_Y_MIN, SERVO_Y_MAX);
//    target_angle[2] = constrain(target_angle[2], SERVO_Z_MIN, SERVO_Z_MAX);
//
//    // 5. 平滑移动
//    smooth_move_servos();
//
//    printf("Servo Angles: X=%d, Y=%d, Z=%d\n",
//           current_angle[0], current_angle[1], current_angle[2]);
//}

void update_servos_by_face(int face_x, int face_y) {
    // 1. 计算人脸相对于屏幕中心的位置（归一化到-1到1范围）
    float norm_x = (face_x - SCREEN_X_MAX/2) / (float)(SCREEN_X_MAX/2);
    float norm_y = (SCREEN_Y_MAX/2 - face_y) / (float)(SCREEN_Y_MAX/2);

    // 2. 计算各轴角度
    // X轴：控制左右转动（人脸左右移动时X轴转动）
    target_angle[0] = map_constrain(face_x, SCREEN_X_MIN, SCREEN_X_MAX, SERVO_X_MIN, SERVO_X_MAX);

    // Y轴：控制上下转动（人脸上下移动时Y轴转动）
    target_angle[1] = map_constrain(face_y, SCREEN_Y_MIN, SCREEN_Y_MAX, SERVO_Y_MIN, SERVO_Y_MAX);

    // 3. Z轴协同控制 - 根据X/Y偏移量计算补偿角度
    // 当人脸在左侧时，Z轴需要逆时针转动（增加角度）补偿
    // 当人脸在右侧时，Z轴需要顺时针转动（减少角度）补偿
    float z_compensation = norm_x * 20.0f; // 补偿系数，可根据实际调整
    z_compensation += norm_y * 5.0f;
    target_angle[2] = 40 + z_compensation; // 40是初始角度

    // 4. 限制角度范围
    target_angle[0] = constrain(target_angle[0], SERVO_X_MIN, SERVO_X_MAX);
    target_angle[1] = constrain(target_angle[1], SERVO_Y_MIN, SERVO_Y_MAX);
    target_angle[2] = constrain(target_angle[2], SERVO_Z_MIN, SERVO_Z_MAX);

    // 5. 平滑移动
    smooth_move_servos();

    printf("Servo Angles: X=%d, Y=%d, Z=%d\n",
           current_angle[0], current_angle[1], current_angle[2]);
}

//=================================================================================
// 平滑移动（限制最大步长）
void Smooth_Move(void) {
    for(int i = 0; i < 3; i++) {
        int step = target_angle[i] - current_angle[i];
        step = constrain(step, -5, 5); // 每次最大变化5°
        current_angle[i] += step;
    }
}
int last_processed_cmd = 0;
// 处理串口指令
void Process_Command(int cmd)
{
    if(cmd == last_processed_cmd) return;

      last_processed_cmd = cmd;
    switch(cmd)
    {
    case 1: // X轴转动+5°
        current_angle[0] +=5;
        current_angle[0] = constrain(current_angle[0], 0, 180);
        //cmd=0;
        tft180_show_string(70, 55,  "  X+  ");
        gpio_set_level(BLUETOOTH_CH9141_RTS_PIN, 0);
        bluetooth_ch9141_send_string ("mode:1   X+");
        bluetooth_ch9141_send_string ("\r\n");
        uart_write_byte(UART_1, '3');

        break;
    case 2: // X轴转动+5°
          current_angle[0] -= 5;
          current_angle[0] = constrain(current_angle[0], 0, 180);
        //  cmd=0;
          tft180_show_string(70,55,  "  X-  ");
          bluetooth_ch9141_send_string ("mode:2   X-");
          bluetooth_ch9141_send_string ("\r\n");
          uart_write_byte(UART_1, '4');
          break;

    case 3: // Y轴转动+5°
        current_angle[1] += 5;
        current_angle[1] = constrain(current_angle[1], 0, 180);
       // cmd=0;
        tft180_show_string(70,55,  "  Y+  ");
        bluetooth_ch9141_send_string ("mode:3   Y+");
        bluetooth_ch9141_send_string ("\r\n");
        uart_write_byte(UART_1, '5');

        break;
    case 4: // Y轴转动+5°
            current_angle[1] -= 5;
            current_angle[1] = constrain(current_angle[1], 0, 180);
            //cmd=0;
            tft180_show_string(70, 55,  "  Y-  ");
            bluetooth_ch9141_send_string ("mode:4   Y-");
            bluetooth_ch9141_send_string ("\r\n");
            uart_write_byte(UART_1, '6');

            break;
    case 6: // Z轴转动+5°
        current_angle[2] +=5;
        current_angle[2] = constrain(current_angle[2], 0, 180);
       // cmd=0;
        tft180_show_string(70, 55,  "   Z+  ");
        bluetooth_ch9141_send_string ("mode:6   Z+");
        bluetooth_ch9141_send_string ("\r\n");
        uart_write_byte(UART_1, '7');

        break;
    case 7: // Z轴转动+5°
            current_angle[2] -= 5;
            current_angle[2] = constrain(current_angle[2], 0, 180);
           // cmd=0;
            tft180_show_string(70, 55,  "  Z-  ");
            bluetooth_ch9141_send_string ("mode:7   Z-");
            bluetooth_ch9141_send_string ("\r\n");
            uart_write_byte(UART_1, '8');

            break;
    case 5:
            uart_write_byte(UART_1, '9');
            tft180_show_string(75, 55,  "track");
            break;

    default:
        printf("Unknown CMD: %d\n", cmd);
    }
//
////    // 角度限幅
////    for(int i = 0; i < 3; i++) {
////        target_angle[i] = constrain(target_angle[i], 0, 180);
////    }



}
