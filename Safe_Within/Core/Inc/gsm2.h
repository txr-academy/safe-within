/*
 * gsm.h
 *
 *  Created on: Aug 7, 2025
 *      Author: hp
 */

#ifndef INC_GSM_H_
#define INC_GSM_H_

//#include "switch.h"
#include "main.h"
#include "stdio.h"

//typedef enum {
//    GSM_STAGE_WAKE = 0,
//    GSM_STAGE_ECHO_OFF,
//    GSM_STAGE_READY
//} GSM_State_t;

#define RX_BUFFER_SIZE 100
#define GSM_MAX_RETRIES 3

extern uint8_t rx_buffer[];
extern uint16_t rx_len;
extern char call_at[100];

//extern volatile GSM_State_t gsm_state;
int gsm_init(void);
int gsm_call(void);
//void gsm_msg(void);


#endif /* INC_GSM_H_ */
