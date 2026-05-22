#ifndef INIT_H_
#define INIT_H_
#include "zf_common_headfile.h"
   extern     int GQ,mq7,mq4,mq2,temp,semp;
   extern           uint8 status;

#define SERVO_X_PIN GPIO_Pin_0
#define SERVO_X_PORT GPIOA
#define SERVO_Y_PIN GPIO_Pin_1
#define SERVO_Y_PORT GPIOA
#define SERVO_Z_PIN GPIO_Pin_2
#define SERVO_Z_PORT GPIOA

   // PID ²ÎÊý
   #define KP 1.2f
   #define KI 0.04f
   #define KD 0.1f
   #define INTEGRAL_LIMIT 30.0f
   #define DEAD_ZONE 5.0f
   #define MAX_STEP 10.0f

void Kang_init(void);
void Encoder_Init();
#endif
