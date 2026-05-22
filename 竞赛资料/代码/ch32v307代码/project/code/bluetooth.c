#include "bluetooth.h"

#include "zf_common_headfile.h"
int buff_mode=0;
int last_flag_mode=0;
uint8 buff_bt[64]="text";
char schar[3]="";

void bluetooth_read(void* paramenter){
    bluetooth_ch9141_read_buff(buff_bt,64);

//    modo_slect();
}

void bluetooth_init(){
    bluetooth_ch9141_init();
    pit_ms_init(TIM10_PIT,500);
}


void bluetooth_ctrl()
{
    buff_mode=buff_bt[0]-48;
    buff_bt[0]=48;
    if(buff_mode==1)
    {
//        flag_mode=1;
//        last_flag_mode=1;
        send_control(1,2);
        //buff_mode=0;
    }

    if(buff_mode==2)
        {
        flag_mode=2;
        last_flag_mode=2;
        send_control(mq4,set_mq4);
        buff_mode=0;
        }
    if(buff_mode==3)
        {
        flag_mode=3;
        last_flag_mode=3;
       // send_control(ceju,set_ceju);
        buff_mode=0;
        }
    if(buff_mode==4)
        {
        flag_mode=4;
        last_flag_mode=4;
        }
    if(buff_mode==5)
        {
        flag_mode=5;
        last_flag_mode=5;
        send_control(temp,set_temp);
        buff_mode=0;
        }
    if(buff_mode==6)
        {
        flag_mode=6;
        last_flag_mode=6;
        send_control(semp,set_semp);
        buff_mode=0;
        }

            if(buff_mode==7 && last_flag_mode==1)
            {
                set_mq7=set_mq7-1;
                send_control(mq7,set_mq7);
                buff_mode=0;
            }
            else if(buff_mode==7 && last_flag_mode==2)
            {
                set_mq4=set_mq4-1;
                send_control(mq4,set_mq4);
                buff_mode=0;
            }
            else if(buff_mode==7 && last_flag_mode==3)
            {
              //  ceju=ceju-1;
              //  send_control(ceju,set_ceju);
                buff_mode=0;
            }
            else if(buff_mode==7 && last_flag_mode==5)
            {
                set_temp=set_temp-1;
                send_control(temp,set_temp);
                buff_mode=0;
            }
            else if(buff_mode==7 && last_flag_mode==6)
            {
                set_semp=set_semp-1;
                send_control(semp,set_semp);
                buff_mode=0;
            }


            if(buff_mode==8 && last_flag_mode==1)
            {
                set_mq7=set_mq7+1;
                send_control(mq7,set_mq7);
                buff_mode=0;
            }
            else if(buff_mode==8 && last_flag_mode==2)
            {
                set_mq4=set_mq4+1;
                send_control(mq4,set_mq4);
                buff_mode=0;
            }
            else if(buff_mode==8 && last_flag_mode==3)
            {
              //  ceju=ceju+1;
               // send_control(ceju,set_ceju);
                buff_mode=0;
            }
            else if(buff_mode==8 && last_flag_mode==5)
            {
                set_temp=set_temp+1;
                send_control(temp,set_temp);
                buff_mode=0;
            }
            else if(buff_mode==8 && last_flag_mode==6)
            {
                set_semp=set_semp+1;
                send_control(semp,set_semp);
                buff_mode=0;
            }
            if(buff_mode==9)
            {
                gpio_set_level(BLUETOOTH_CH9141_RTS_PIN, 0);
                bluetooth_ch9141_send_string ("mq7:");
                system_delay_ms(10);
                send_control(mq7,set_mq7);
                system_delay_ms(10);
                gpio_set_level(BLUETOOTH_CH9141_RTS_PIN, 0);
                bluetooth_ch9141_send_string ("mq4:\r\n");
                system_delay_ms(10);
                send_control(mq4,set_mq4);
                system_delay_ms(10);
                gpio_set_level(BLUETOOTH_CH9141_RTS_PIN, 0);
                bluetooth_ch9141_send_string ("ceju1:\r\n");
                system_delay_ms(10);
             //   send_control(ceju,set_ceju);
                system_delay_ms(10);
                gpio_set_level(BLUETOOTH_CH9141_RTS_PIN, 0);
                bluetooth_ch9141_send_string ("temp1:\r\n");
                system_delay_ms(10);
                send_control(temp,set_temp);
                system_delay_ms(10);
                gpio_set_level(BLUETOOTH_CH9141_RTS_PIN, 0);
                bluetooth_ch9141_send_string ("semp1:\r\n");
                system_delay_ms(10);
                send_control(semp,set_semp);
                buff_mode=0;
            }
}
