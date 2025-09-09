/*
 * gsm6.h
 * @file gsm6.h
 * @brief GSM module header for calls and SMS via UART using STM32 HAL.
 * Declares GSM functions, configuration macros, types, and global variables for GSM control.
 *  Created on: Aug 26, 2025
 *      Author: abhiram and anandu
 */

#ifndef INC_GSM_H_
#define INC_GSM_H_


#include "main.h"
#include "stdio.h"
#include "string.h"


/** @def SIM_RX_BUFFER_SIZE
 *  @brief Size of buffer for GSM receive via UART.
 */
#define SIM_RX_BUFFER_SIZE 100

/** @def MAX_GSM_INIT_ATTEMPTS
 *  @brief Maximum GSM initialization retry attempts.
 */
#define MAX_GSM_INIT_ATTEMPTS 3

#define AT_TIMEOUT 2000
#define CALL_TIMEOUT 5000
#define MESSAGE_TIMEOUT 1000

#define OKAY_RESPONSE "OK"
#define EMERGENCY_CONTACT_1 "917994277760"
#define EMERGENCY_CONTACT_2 "918129409935"
#define EMERGENCY_CONTACT_3 "917907030939"
#define MESSAGE "Alert! A fall was detected"

/*  @brief AT Command for GSM text mode
 */
#define CMGF "AT+CMGF=1\r\n"

/*  @brief AT command  used to send SMS using a GSM module.
 */
#define CMGS "AT+CMGS=\"+917994277760\"\r\n"

/*  @brief SMS message text for  provided contact numbers
 */
#define MSG "EMERGENCY AT HOME"

/*  @brief send Ctrl+Z (0x1A in hex) to indicate the end of the SMS.
 */
#define CTRLZ 0x1A

/*  @brief  GSM command response status
 */
typedef enum {
	RX_OK,            /*< Command succeeded. */
	RX_TIMEOUT        /*< Response timed out. */
} GSM_Response_t;

/*  @brief  GSM states
 */
typedef enum {
    GSM_STATE_OK,     /*< GSM ready/OK state. */
    GSM_STATE_ERROR   /*< GSM error state. */
} GSM_State_t;

/**
 * @brief Sends an AT command via UART and waits for response.
 * @param at_cmd AT command string to send.
 * @param sim_response_buffer Buffer to store response.
 * @param size Pointer to response length.
 * @param timeout Timeout in milliseconds.
 * @return GSM_Response_t Result of AT command.
 */
GSM_Response_t send_at_cmd(char *at_cmd, char *sim_response_buffer, int *size, int timeout);


/**
 * @brief Begins UART receive operation for GSM.
 */
void gsm_start_rx(void);


/**
 * @brief Initializes the GSM module.
 * @return GSM_State_t Returns GSM_STATE_OK or GSM_STATE_ERROR.
 */
GSM_State_t gsm_init(void);

/**
 * @brief Performs GSM call to a contact  number.
 * @param  phone number.
 * @return GSM_State_t Call operation status.
 */
GSM_State_t gsm_call(const char *number);

/**
 * @brief Sends an SMS message via GSM.
 * @param phone number
 * @param message Message string to send.
 * @return GSM_State_t Message operation status.
 */
GSM_State_t gsm_sms(const char *number, const char *message);

/**
 * @brief Calls and sends SMS to emergency contacts, with retry logic.
 */
void call_sms_function(void);

/**
 * @brief Sends GSM reset command via UART to ensure GSM is in Command mode, not in text prompt mode.
 */
void gsm_reset(void);



/**
 * @brief Global variable indicating  GSM state.
 */
extern GSM_State_t result;

extern UART_HandleTypeDef huart4;

extern  uint8_t sim_rx_byte;
extern  char sim_rx_buffer[SIM_RX_BUFFER_SIZE];
extern  uint16_t sim_rx_index;



#endif /* INC_GSM_H_ */
