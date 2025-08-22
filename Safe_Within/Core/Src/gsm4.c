/*
 * gsm4.c
 *
 *  Created on: Aug 21, 2025
 *      Author: abhir
 */

#include "gsm4.h"

uint8_t sim_rx_byte = 0;
char sim_rx_buffer[SIM_RX_BUFFER_SIZE] = {0};
uint16_t sim_rx_index = 0;
uint8_t line_ready = 0;

// static GSM_State_t gsm_state = GSM_STATE_IDLE;

/* --- Start async reception --- */
void gsm_start_rx(void)
{
    sim_rx_index = 0;
    line_ready = 0;
    HAL_UART_Receive_IT(&huart4, &sim_rx_byte, 1);
}

/* --- Wait for OK in response (non-blocking check) --- */
static GSM_State_t gsm_check_ok(void)
{
    if (line_ready)
    {
        line_ready = 0;
        if (strstr(sim_rx_buffer, "OK") != NULL)
        {
            return GSM_STATE_OK;
        }
        else
        {
            return GSM_STATE_ERROR;
        }
    }
    return GSM_STATE_WAITING;
}

/* --- GSM Initialization --- */
GSM_State_t gsm_init(void)
{
    char at_cmd[] = "AT\r\n";
    for (int attempt = 0; attempt < GSM_MAX_RETRIES; attempt++)
    {
        gsm_start_rx();
        HAL_UART_Transmit(&huart4, (uint8_t*)at_cmd, strlen(at_cmd), HAL_MAX_DELAY);
        gsm_start_rx();

        uint32_t tickstart = HAL_GetTick();
        while ((HAL_GetTick() - tickstart) < AT_TIMEOUT)  // 2s timeout
        {
            GSM_State_t result = gsm_check_ok();
            if (result == GSM_STATE_OK)
            {
                return GSM_STATE_OK;
            }
            else if (result == GSM_STATE_ERROR)
            {
                break; // retry
            }
        }
    }
    return GSM_STATE_ERROR;
}

/* --- GSM Make a Call --- */
GSM_State_t gsm_call(const char *number)
{
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "ATD+%s;\r\n", number);

    gsm_start_rx();
    HAL_UART_Transmit(&huart4, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);

    uint32_t tickstart = HAL_GetTick();
    while ((HAL_GetTick() - tickstart) < CALL_TIMEOUT)  // wait up to 5s
    {
        GSM_State_t result = gsm_check_ok();
        if (result == GSM_STATE_OK)
        {
            return GSM_STATE_OK;
        }
        else if (result == GSM_STATE_ERROR)
        {
            return GSM_STATE_ERROR;
        }
    }
    return GSM_STATE_ERROR;
}

GSM_State_t gsm_sms(const char *number, const char *message)
{
    char cmd[64];
    uint8_t ctrlz = 0x1A;

    /* Step 1: Set text mode */
    gsm_start_rx();
    char cmgf[] = "AT+CMGF=1\r\n";
    HAL_UART_Transmit(&huart4, (uint8_t*)cmgf, strlen(cmgf), HAL_MAX_DELAY);

    uint32_t tickstart = HAL_GetTick();
    while ((HAL_GetTick() - tickstart) < MESSAGE_TIMEOUT)
    {
        GSM_State_t result = gsm_check_ok();
        if (result == GSM_STATE_OK) break;
        else if (result == GSM_STATE_ERROR) return GSM_STATE_ERROR;
    }

    /* Step 2: Send recipient number */
    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"+%s\"\r\n", number);
    gsm_start_rx();
    HAL_UART_Transmit(&huart4, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);

    tickstart = HAL_GetTick();
    while ((HAL_GetTick() - tickstart) < CALL_TIMEOUT)
    {
        if (line_ready) break; // Usually gets ">"
    }

    /* Step 3: Send message + Ctrl+Z */
    gsm_start_rx();
    HAL_UART_Transmit(&huart4, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart4, &ctrlz, 1, HAL_MAX_DELAY);

    tickstart = HAL_GetTick();
    while ((HAL_GetTick() - tickstart) < MESSAGE_TIMEOUT)
    {
        GSM_State_t result = gsm_check_ok();
        if (result == GSM_STATE_OK) return GSM_STATE_OK;
        else if (result == GSM_STATE_ERROR) return GSM_STATE_ERROR;
    }

    return GSM_STATE_ERROR;
}

//GSM_State_t gsm_sms(void){
//	 uint8_t ctrlz = 0x1A;
//	gsm_start_rx();
//	HAL_UART_Transmit(&huart4, (uint8_t*)CMGF, strlen(CMGF),HAL_MAX_DELAY);
//	uint32_t tickstart = HAL_GetTick();
//	while ((HAL_GetTick() - tickstart) < MESSAGE_TIMEOUT) // waits for 1s
//	{
//		GSM_State_t result = gsm_check_ok();
//		if (result == GSM_STATE_OK){
//			HAL_UART_Transmit(&huart4, (uint8_t*)CMGS, strlen(CMGS),HAL_MAX_DELAY);
//			HAL_UART_Transmit(&huart4, (uint8_t*)MSG, strlen(MSG),HAL_MAX_DELAY);
//			HAL_UART_Transmit(&huart4, &ctrlz, 1, 1000);
//			return result;
//		}
//		else {
//			return GSM_STATE_ERROR;
//		}
//	}
//	return GSM_STATE_ERROR;
//}
