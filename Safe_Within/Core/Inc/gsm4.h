/*
 * gsm4.h
 *
 *  Created on: Aug 21, 2025
 *      Author: abhir
 */

#ifndef INC_GSM4_H_
#define INC_GSM4_H_

#include "main.h"
#include "stdio.h"
#include "string.h"

#define SIM_RX_BUFFER_SIZE 128
#define GSM_MAX_RETRIES 3
#define AT_TIMEOUT 2000
#define CALL_TIMEOUT 5000
#define MESSAGE_TIMEOUT 1000

#define OKAY_RESPONSE "OK"
#define EMERGENCY_CONTACT_1 "917994277760"
#define EMERGENCY_CONTACT_2 "918129409935"
#define EMERGENCY_CONTACT_3 "917907030939"
#define MESSAGE "Alert! A fall was detected"

#define CMGF "AT+CMGF=1\r\n"
#define CMGS "AT+CMGS=\"+917994277760\"\r\n"
#define MSG "EMERGENCY AT HOME"
#define CTRLZ 0x1A

typedef enum {
    GSM_STATE_IDLE,
    GSM_STATE_WAITING,
    GSM_STATE_OK,
    GSM_STATE_ERROR
} GSM_State_t;

extern UART_HandleTypeDef huart4;

void gsm_start_rx(void);
GSM_State_t gsm_init(void);
GSM_State_t gsm_call(const char *number);
GSM_State_t gsm_sms(const char *number, const char *message);

extern  uint8_t sim_rx_byte;
extern  char sim_rx_buffer[SIM_RX_BUFFER_SIZE];
extern  uint16_t sim_rx_index;
extern  uint8_t line_ready;

#endif /* INC_GSM4_H_ */
