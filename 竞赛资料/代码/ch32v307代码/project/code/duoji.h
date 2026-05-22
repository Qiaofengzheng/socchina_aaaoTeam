#ifndef DUOJI_H_
#define DUOJI_H_
//
#include "zf_common_headfile.h"


extern int target_angle[];  // X,Y,Z目标角度
extern int current_angle[]; // 当前角度

extern int last_processed_cmd;
void Smooth_Move(void);
void Process_Command(int cmd);
void update_servos_by_face(int face_x, int face_y) ;
void smooth_move_servos();
int map_constrain(int value, int in_min, int in_max, int out_min, int out_max);
#endif
