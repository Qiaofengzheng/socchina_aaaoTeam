#ifndef __DAC_DRV_H
#define __DAC_DRV_H

#define DAC_PORT GPIOA
#define DAC1_PIN GPIO_Pin_4
#define DAC2_PIN GPIO_Pin_5

#define DAC1 0x01 //0001
#define DAC2 0x02 //0010

#define VREF 2510//²Î¿¼µçÑ¹Öµ(mV)

void DAC_User_Init();
void DAC_User_Set_Voltage(unsigned char DAC_channel, unsigned int voltage);

#endif /* __DAC_H */
