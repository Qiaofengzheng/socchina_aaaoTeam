/*
 * bluetooth.h
 *
 *  Created on: Jun 29, 2023
 *      Author: 17839
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "zf_common_headfile.h"

extern uint8 buff_bt[64];
extern int buff_mode;
void bluetooth_init();
void bluetooth_read();
void bluetooth_ctrl();
void modo_slect();
#endif /* BLUETOOTH_H_ */
