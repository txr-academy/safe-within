#include "main.h"
#include <string.h>
#include "gsm2.h"

extern UART_HandleTypeDef huart4;

#define RX_BUFFER_SIZE 100
#define GSM_ATD_MAX_RETRIES 3
#define GSM_AT_MAX_RETRIES 3
#define GSM_SMS_MAX_RETRIES 3
#define AT "AT\r\n"
#define ATD "ATD+917994277760;\r\n"
#define OK "OK"
#define CMGF "AT+CMGF=1\r\n"
#define CMGS "AT+CMGS=\"+918129409935\"\r\n"
#define MSG "EMERGENCY AT HOME"
#define CTRLZ 0x1A

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint16_t rx_len = 0;

int gsm_receive_response(uint32_t timeout_ms) {
	 HAL_StatusTypeDef status;
	// int response_count=0;
	memset(rx_buffer, 0, RX_BUFFER_SIZE);
    status= HAL_UART_Receive(&huart4, rx_buffer,RX_BUFFER_SIZE-1, timeout_ms);


    if (status == HAL_OK) {
    	int8_t len = huart4.RxXferSize - huart4.RxXferCount;
    	 rx_buffer[len] = '\0';
        return 1;  // Success
    } else {
        return 0; // fail
    }
}

// Initialize GSM module
int gsm_init(void) {
	 int8_t retries = 0;
    // 1. Send first AT command
    HAL_UART_Transmit(&huart4, (uint8_t*)AT, strlen(AT), 10);
//    gsm_receive_response(1000);
    HAL_Delay(1000);
    // 2. Send second AT command and receive response, verify echo and OK
    while (retries < GSM_AT_MAX_RETRIES) {
    HAL_UART_Transmit(&huart4, (uint8_t*)AT, strlen(AT), 10);
    int16_t res_flag=gsm_receive_response(3000);

    if (res_flag && strstr((char*)rx_buffer,OK) != NULL) {
        return 1; // Success
    }
    retries++;
    HAL_Delay(500);
    }
    return 0;
}
//    if (!res_flag) {
//    	gsm_init();
////    	HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 10);
//             // retry AT command if no response
//        }
//
//    else if (strstr((char*)rx_buffer,OK) == NULL) {
//    	gsm_init();
//    	HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 10);
    	//retry AT command if expected response not received

int gsm_call(void) {

    int retries = 0;
    while (retries < GSM_ATD_MAX_RETRIES) {
        HAL_UART_Transmit(&huart4, (uint8_t*)ATD, strlen(ATD),10);
        if (gsm_receive_response(3000) && strstr((char*)rx_buffer,OK) != NULL) {

                return 1;
            }

        retries++;
        HAL_Delay(5000);
        }
    return 0;

}

int gsm_sms(){
	 uint8_t ctrlz = CTRLZ;
	 int retries=0;
	 while(retries<GSM_SMS_MAX_RETRIES){
	 HAL_UART_Transmit(&huart4, (uint8_t*)CMGF, strlen(CMGF),10);
	 if ((gsm_receive_response(3000)) && strstr((char*)rx_buffer,OK) != NULL){

		 HAL_UART_Transmit(&huart4, (uint8_t*)CMGS, strlen(CMGS),10);
		 HAL_UART_Transmit(&huart4, (uint8_t*)MSG, strlen(MSG),10);
		     HAL_UART_Transmit(&huart4, &ctrlz, 1, 1000);
		     return 1;
		 }

		 retries++;
		 HAL_Delay(2000);
	 }
return 0;
	 }





