#include "main.h"
#include <string.h>

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
GSM_State_t gsm_state = GSM_STAGE_WAKE;

int gsm_receive_response(uint32_t timeout_ms) {
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    rx_len = 0;
    HAL_StatusTypeDef res = HAL_UART_Receive(&huart4, rx_buffer, RX_BUFFER_SIZE - 1, timeout_ms);
    if (res == HAL_OK) {
        rx_len = strlen((char*)rx_buffer);
        return 1;  // success
    }
    // Partial or timeout
    rx_len = strlen((char*)rx_buffer);
    return 0;
}

// Initialize GSM module
int gsm_init(void) {
    // 1. Send first AT command
    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 200);
    HAL_Delay(500);

    // 2. Send second AT command and receive response, verify echo and OK
    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 200);
    if (!gsm_receive_response(2000)) {
        return 0;  // no response, fail
    }
    if (strstr((char*)rx_buffer, "AT\r") != NULL &&
        strstr((char*)rx_buffer, "OK\r\n") != NULL) {
        gsm_state = GSM_STAGE_WAKE;
        rx_len = 0;
    } else {
        return 0; // response not as expected
    }

    // 3. Disable echo and verify response (positive logic)
    HAL_UART_Transmit(&huart4, (uint8_t*)"ATE0\r\n", strlen("ATE0\r\n"), 200);
    if (!gsm_receive_response(2000)) {
        return 0;
    }
    if (strstr((char*)rx_buffer, "ATE0\r") != NULL &&
        strstr((char*)rx_buffer, "OK\r\n") != NULL) {
        gsm_state = GSM_STAGE_ECHO_OFF;
        rx_len = 0;
    } else {
        return 0;
    }

    // 4. Confirm echo off with AT and verify OK
    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 200);
    if (!gsm_receive_response(2000)) {
        return 0;
    }
    if (strstr((char*)rx_buffer, "OK\r\n") != NULL) {
        gsm_state = GSM_STAGE_READY;
        rx_len = 0;
    } else {
        return 0;
    }

    return 1;  // Initialization success
}



int gsm_call(const char* number) {
    char cmd[40];
    snprintf(cmd, sizeof(cmd), "ATD%s;\r\n", number);

    int retries = 0;
    while (retries < GSM_MAX_RETRIES) {
        HAL_UART_Transmit(&huart4, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);
        if (gsm_receive_response(5000)) {
            if (strstr((char*)rx_buffer, "OK") != NULL ||
                strstr((char*)rx_buffer, "CONNECT") != NULL) {
                return 1;
            }
        }
        retries++;
        HAL_Delay(500);
    }
    return 0;
}
