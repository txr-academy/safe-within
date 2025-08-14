/*
 * gsm.h
 *
 *  Created on: Aug 13, 2025
 *      Author: abhir
 */

#ifndef INC_GSM_H_
#define INC_GSM_H_

#include "main.h"


extern UART_HandleTypeDef huart4;

#define RX_BUFFER_SIZE 100
extern uint8_t rx_buffer[];
extern uint16_t rx_len;

typedef enum {
    GSM_STAGE_WAKE = 0,
    GSM_STAGE_ECHO_OFF,
    GSM_STAGE_AT_SENT,
    GSM_STAGE_DIAL,
    GSM_STAGE_DONE
} GSM_State_t;

extern volatile GSM_State_t gsm_state;

void gsm_init(void);
void gsm_recieve_function(void);


#endif /* INC_GSM_H_ */
