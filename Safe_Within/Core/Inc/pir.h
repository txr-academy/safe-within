/*
 * pir.h
 *
 *  Created on: Jul 30, 2025
 *      Author: abhir
 */

#ifndef INC_PIR_H_
#define INC_PIR_H_

#include "stdint.h"
#include "main.h"

#define FLAG_ON 1
#define FLAG_OFF 0
extern volatile int pir_1_flag;
extern volatile int pir_2_flag;
extern volatile uint32_t pir_1_time;
extern volatile uint32_t pir_2_time;
extern volatile uint32_t pir_1_off_time;
extern volatile uint32_t pir_2_off_time;

States get_pir_state(uint32_t pir_1_time, uint32_t pir_2_time);

//typedef enum {
//	IDLE , ACTIVE, ALERT
//} States;

#endif /* INC_PIR_H_ */
