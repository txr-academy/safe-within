#include "main.h"
#include <string.h>
#include "gsm2.h"

extern UART_HandleTypeDef huart4;

#define RX_BUFFER_SIZE 100
#define GSM_MAX_RETRIES 3
#define AT "AT"
#define ATD "ATD+918129409935;"
#define OK "OK"


uint8_t rx_buffer[RX_BUFFER_SIZE];
uint16_t rx_len = 0;

int gsm_receive_response(uint32_t timeout_ms) {
	 HAL_StatusTypeDef status;
	 int response_count=0;
	 while(response_count<3){
	memset(rx_buffer, 0, RX_BUFFER_SIZE);

    status= HAL_UART_Receive(&huart4, rx_buffer, RX_BUFFER_SIZE - 1, timeout_ms);


    if (status == HAL_OK) {
        return 1;  // Success
    } else {
        response_count++; // Increment and retry
    }
	 }
	 return 0; //fail after three times
}

// Initialize GSM module
int gsm_init(void) {
    // 1. Send first AT command
    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 10);
    HAL_Delay(1000);

    // 2. Send second AT command and receive response, verify echo and OK
    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 10);
    if (!gsm_receive_response(1000)) {
            return 0;  // receive failed
        }
    if (!(strstr((char*)rx_buffer,AT) != NULL &&
        strstr((char*)rx_buffer,OK) != NULL)) {
    	HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 10);
    	return 0;
    }

    return 1;

}

int gsm_call(void) {
    char cmd[100]="ATD+918129409935;\r\n";

    int retries = 0;
    while (retries < GSM_MAX_RETRIES) {
        HAL_UART_Transmit(&huart4, (uint8_t*)cmd, strlen(cmd),10);
        if (gsm_receive_response(1000)) {
            if (strstr((char*)rx_buffer,ATD) != NULL &&
                strstr((char*)rx_buffer,OK) != NULL) {
                return 1;
            }

        }
        retries++;
        HAL_Delay(5000);
        }
    return 0;

}

int gsm_sms(){
return 0;
}





