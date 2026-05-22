  #include "zf_common_headfile.h"

// 假设蜂鸣器连接在PA0引脚
#define BEEP_PIN GPIO_Pin_4
#define BEEP_PORT GPIOA

void Beep_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_PORT, &GPIO_InitStructure);

    GPIO_ResetBits(BEEP_PORT, BEEP_PIN); // 初始关闭蜂鸣器
}

void Beep_On(void) {
    GPIO_SetBits(BEEP_PORT, BEEP_PIN); // 打开蜂鸣器
}

void Beep_Off(void) {
    GPIO_ResetBits(BEEP_PORT, BEEP_PIN); // 关闭蜂鸣器
}
