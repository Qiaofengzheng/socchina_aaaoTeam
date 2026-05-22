#ifndef __INMP441_H
#define __INMP441_H

#define I2S3_PORT       GPIOB
#define I2S3_SCK_PIN    GPIO_Pin_3	//时钟
#define I2S3_SD_PIN     GPIO_Pin_5	//数据

#define I2S3_WS_PORT    GPIOA
#define I2S3_WS_PIN     GPIO_Pin_15	//左右声道

#define I2S3_MCK_PORT    GPIOC
#define I2S3_MCK_PIN     GPIO_Pin_7

typedef struct
{
	unsigned long audio[4];//LH/LL/RH/RL
}Audio_t;

void INMP441_Init();

extern Audio_t Audio;
/*
 * WS = 0: Left channel; WS = 1: Right channel, WS的频率等于音频采样率fs(例程为44.1kHz)
 * SCK计算: fs * 32bit * 2channel (例程为2.8224MHz)
 */

#endif /* __INMP441_H */


