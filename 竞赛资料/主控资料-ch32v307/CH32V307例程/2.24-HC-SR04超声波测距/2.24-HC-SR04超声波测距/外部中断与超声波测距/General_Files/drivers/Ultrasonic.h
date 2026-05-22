#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

//============================¡¾Òý½Å¶¨Òå¡¿=================================
#define ULTRASONIC_PORT     GPIOA
#define ULTRASONIC_TRIG_PIN GPIO_Pin_6
#define ULTRASONIC_ECHO_PIN GPIO_Pin_1

void Ultrasonic_Init();
float Ultrasonic_GetDistance();

#endif
  
