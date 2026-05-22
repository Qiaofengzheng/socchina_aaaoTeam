#include "zf_common_headfile.h"
#include "init.h"
int GQ=0,mq7=0,mq4=0,mq2=0,temp=0,semp=0;
          uint8 status=0;

void Kang_init(void)
{

          key_init(10);
          tft180_init();
        //  DHT11_Init();
          pit_ms_init(TIM6_PIT, 3);
          adc_init(ADC1_IN4_A4,ADC_8BIT);
          adc_init(ADC1_IN0_A0,ADC_8BIT);
          adc_init(ADC1_IN2_A2,ADC_8BIT);
          adc_init(ADC1_IN1_A1,ADC_8BIT);
          gpio_init(A3,GPI,0,GPI_FLOATING_IN);
          bluetooth_init();
          debug_init();

          Servo_PWM_Init_All();
           PID_Init_All();
            Servo_Set_Angle(SERVO_X, 90);
            Servo_Set_Angle(SERVO_Y, 90);
            Servo_Set_Angle(SERVO_Z, 90);

//          // 舵机初始化
//             Servo_Init(SERVO_X_PORT, SERVO_X_PIN);
//             Servo_Init(SERVO_Y_PORT, SERVO_Y_PIN);
//             Servo_Init(SERVO_Z_PORT, SERVO_Z_PIN);
//
//             Servo_SetAngle(SERVO_X_PORT, SERVO_X_PIN, 94);
//             Servo_SetAngle(SERVO_Y_PORT, SERVO_Y_PIN, 165);
//             Servo_SetAngle(SERVO_Z_PORT, SERVO_Z_PIN,130);
//
//             // PID 初始化
//             PID_Init(&pidX, KP, KI, KD, INTEGRAL_LIMIT);
//             PID_Init(&pidY, KP, KI, KD, INTEGRAL_LIMIT);
//             PID_Init(&pidZ, KP, KI, KD, INTEGRAL_LIMIT);

  //mt9v03x_init();
  //mpu6050_init ();
//  wireless_uart_init ();
//  Encoder_Init();
//  motor_init();
  //pwm_init(TIM2_PWM_CH1_A15, 5000, 10000);
}
