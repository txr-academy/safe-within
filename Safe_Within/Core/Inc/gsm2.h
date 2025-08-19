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
typedef enum {
    GSM_STAGE_WAKE = 0,
    GSM_STAGE_ECHO_OFF,
    GSM_STAGE_AT_SENT,
    GSM_STAGE_DIAL,
    GSM_STAGE_DONE
} GSM_State_t;

volatile GSM_State_t gsm_state = GSM_STAGE_WAKE;
void gsm_init(void);
void gsm_call(void);
void gsm_msg(void);


#endif /* INC_GSM_H_ */
