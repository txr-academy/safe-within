/*
 * gsm.c
 *
 *  Created on: Aug 13, 2025
 *      Author: abhir
 */

//#include "main.h"
#include "stdint.h"
#include "gsm.h"
#include <string.h>

//extern UART_HandleTypeDef huart4;
uint8_t rx_buffer[100] = {0};
uint16_t rx_len = 0;


volatile GSM_State_t gsm_state = GSM_STAGE_WAKE;
// Buffer setup same as before...
void gsm_recieve_function(void){
	HAL_UART_Receive_IT(&huart4, &rx_buffer[rx_len], 1);
}
void gsm_init(void) {
    rx_len = 0;
    gsm_state = GSM_STAGE_WAKE;

    // 1. Wake GSM module with AT command
    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), HAL_MAX_DELAY);
    gsm_recieve_function();
}

// ISR callback — handle "OK" sequencing
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == huart4.Instance && gsm_state != GSM_STAGE_DONE) {
//        rx_len++;
//        if (rx_len < RX_BUFFER_SIZE - 1) {
//            rx_buffer[rx_len] = '\0';
//
//            if (strstr((char*)rx_buffer, "\r\nOK\r\n") != NULL) {
//                rx_len = 0; // Clear buffer for next command
//
//                if (gsm_state == GSM_STAGE_WAKE) {
//                    gsm_state = GSM_STAGE_ECHO_OFF;
//                    // Disable echo after wakeup
//                    HAL_UART_Transmit(&huart4, (uint8_t*)"ATE0\r\n", strlen("ATE0\r\n"), HAL_MAX_DELAY);
//
//                } else if (gsm_state == GSM_STAGE_ECHO_OFF) {
//                    gsm_state = GSM_STAGE_AT_SENT;
//                    // Send AT to confirm connection once more (optional)
//                    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), HAL_MAX_DELAY);
//
//                } else if (gsm_state == GSM_STAGE_AT_SENT) {
//                    gsm_state = GSM_STAGE_DIAL;
//                    // Send dial command
//                    HAL_UART_Transmit(&huart4,
//                        (uint8_t*)"ATD+918129409945;\r\n",
//                        strlen("ATD+918129409945;\r\n"),
//                        HAL_MAX_DELAY);
//                    gsm_state = GSM_STAGE_DONE;
//                }
//            }
//            gsm_recieve_function();
//        }
//    }
// }

