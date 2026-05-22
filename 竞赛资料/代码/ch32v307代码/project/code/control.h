#ifndef CONTROL_H_
#define CONTROL_H_
#include "zf_common_headfile.h"

void key_control();
void celiang();
void uart_control();
void tft_oled();
void send_control(uint8 send_data,uint ssend_data);
void send_float_data(float data);
void Int_To_Str(int x,char *Str);
extern uint8 set_ceju,set_mq7,set_mq4,set_temp,set_semp;
extern int CMD ;

#define BUF_SIZE 64
#define FACE_STABLE_THRESHOLD 5

extern char rx_buf[BUF_SIZE];
extern uint8_t rx_index ;
extern int face_coords[FACE_STABLE_THRESHOLD][2];
extern int coord_index;
extern int smooth_x, smooth_y;
extern int xx, yy;
extern float avg_temp;

void process_coordinates(char* data);
void update_display(int x, int y);
//void smooth_coordinates(int x, int y, int *out_x, int *out_y);



void parse_coordinates(char *buf);
void tempcontorl();
#endif /* CONTROL_H_ */
