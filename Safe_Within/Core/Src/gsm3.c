#include "main.h"
#include <string.h>
#include "gsm2.h"

extern UART_HandleTypeDef huart4;

#define RX_BUFFER_SIZE 100
#define GSM_MAX_RETRIES 3

typedef enum {
    GSM_STAGE_WAKE = 0,
    GSM_STAGE_ECHO_OFF,
    GSM_STAGE_READY,
    GSM_STAGE_DIAL,
    GSM_STAGE_DONE,
    GSM_STAGE_ERROR
} GSM_State_t;

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint16_t rx_len = 0;
//GSM_State_t gsm_state = GSM_STAGE_WAKE;

int gsm_receive_response(uint32_t timeout_ms) {
	 HAL_StatusTypeDef status;
	memset(rx_buffer, 0, RX_BUFFER_SIZE);

    status = HAL_UART_Receive(&huart4, rx_buffer, RX_BUFFER_SIZE - 1, timeout_ms);
//		 HAL_UART_Receive(&huart4, rx_buffer, RX_BUFFER_SIZE - 1, timeout_ms);
		 rx_buffer[RX_BUFFER_SIZE - 1] = '\0';

		    if (status == HAL_OK) {
		        return 1;  // success
		    } else {
		        return 0;  // fail or timeout
		    }
}

// Initialize GSM module
int gsm_init(void) {
    // 1. Send first AT command
    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
    HAL_Delay(500);

    // 2. Send second AT command and receive response, verify echo and OK
    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
    if (!gsm_receive_response(1000)) {
            return 0;  // receive failed
        }
    if (!(strstr((char*)rx_buffer, "AT\r") != NULL &&
        strstr((char*)rx_buffer, "OK\r\n") != NULL)) {
    	HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
    	return 0;
    }

    return 1;

}

int gsm_call(void) {
    char cmd[100]="ATD+918129409935;\r\n";

    int retries = 0;
    while (retries < GSM_MAX_RETRIES) {
        HAL_UART_Transmit(&huart4, (uint8_t*)cmd, strlen(cmd),1000);
        if (gsm_receive_response(5000)) {
            if (strstr((char*)rx_buffer, "ATD+918129409935;\r") != NULL &&
                strstr((char*)rx_buffer, "OK/r/n") != NULL) {
                return 1;
            }

        }
        retries++;
        HAL_Delay(500);
        }
    return 0;

}






