/*
 * gsm6.c
 *
 *  Created on: Aug 26, 2025
 *      Author: abhir
 */

#include "gsm6.h"

uint8_t sim_rx_byte = 0;
char sim_rx_buffer[SIM_RX_BUFFER_SIZE] = {0};
uint16_t sim_rx_index = 0;
uint8_t line_ready = 0;

GSM_State_t result = 0;
GSM_State_t result_wake = 0;
GSM_Response_t status;

void gsm_start_rx(void)
{
    sim_rx_index = 0;
    line_ready = 0;
    HAL_UART_Receive_IT(&huart4, &sim_rx_byte, 1);
}

GSM_Response_t send_at_cmd( char *at_cmd, char *sim_response_buffer, int *size, int timeout)
{
	uint32_t tickstart;
	HAL_UART_Transmit(&huart4, (uint8_t*)at_cmd, strlen(at_cmd), HAL_MAX_DELAY);
	gsm_start_rx();
	tickstart = HAL_GetTick();
	while(1){
		if ((HAL_GetTick() - tickstart) > timeout){
			sim_rx_buffer[sim_rx_index] = '\0';
			strcpy(sim_response_buffer, sim_rx_buffer);
			*size = sim_rx_index-1;
			memset(sim_rx_buffer, 0, SIM_RX_BUFFER_SIZE);
			break;
		}
	}
	if(sim_rx_index == 0){
		return RX_TIMEOUT;
	}
	else{
		return RX_OK;
	}
}

//GSM_Response_t check_only(char *response_buffer, int *size)
//{
//	uint32_t tickstart;
//	while(1){
//		tickstart = HAL_GetTick();
//		if ((HAL_GetTick() - tickstart) > AT_TIMEOUT){
//			sim_rx_buffer[sim_rx_index] = '\0';
//			strcpy(response_buffer, sim_rx_buffer);
//			*size = sim_rx_index-1;
//			memset
//			break;
//		}
//	}
//	if(sim_rx_index == 0){
//		return RX_TIMEOUT;
//	}
//	else{
//		return RX_OK;
//	}
//}

GSM_State_t gsm_init(void)
{
	char gsm_wake[] = "AT\r\n";
	char gsm_rx[20];
	int rx_size=0;
	//int *rx_size = NULL;
	int timeout = AT_TIMEOUT;

	char echo_off[] = "ATE0\r\n";

//	GSM_Response_t status;
	status = send_at_cmd(gsm_wake, gsm_rx, &rx_size, timeout);
	if (status == RX_OK){
		if (strncmp(gsm_rx, "AT\r\r\nOK\r\n", rx_size) == 0){
			memset(gsm_rx, 0, 20);
			rx_size =0;
			status = send_at_cmd(echo_off, gsm_rx, &rx_size, timeout);
			if (strncmp(gsm_rx, "ATE0\r\r\nOK\r\n", rx_size) == 0){
				return GSM_STATE_OK;
			}
		}
	}
	return GSM_STATE_ERROR;
}

GSM_State_t gsm_wake(void)
{
	char gsm_wake_up[] = "AT\r\n";
	char gsm_rx[20];
	int rx_size = 0;
	int timeout = AT_TIMEOUT;

	status = send_at_cmd(gsm_wake_up, gsm_rx, &rx_size, timeout);
		if (status == RX_OK){
			if (strncmp(gsm_rx, "\r\nOK\r\n", rx_size) == 0){
				memset(gsm_rx, 0, 20);
				rx_size = 0;
				return GSM_STATE_OK;
			}
		}
		return GSM_STATE_ERROR;
}

GSM_State_t gsm_call(const char *number)
{
    char dial[30];
    snprintf(dial, sizeof(dial), "ATD+%s;\r\n", number);
    char gsm_rx[40];
    int rx_size = 0;
    int timeout = AT_TIMEOUT;
//    GSM_Response_t status;

    status = send_at_cmd(dial, gsm_rx, &rx_size, timeout);
    if (status == RX_OK){
    	if (strncmp(gsm_rx, "\r\nOK\r\n", rx_size) == 0){
    		memset(gsm_rx, 0, 40);
    		return GSM_STATE_OK;
    	}
    }
    return GSM_STATE_ERROR;
}

GSM_State_t gsm_sms(const char *number, const char *message)
{
	char gsm_text_mode[] = "AT+CMGF=1\r\n";
	char gsm_rx[30];
	int rx_size = 0;
	int timeout = AT_TIMEOUT;
	char gsm_send_sms[30];
	char msg[40] = {0};
	int len;
	snprintf(gsm_send_sms, sizeof(gsm_send_sms), "AT+CMGS=\"+%s\"\r\n", number);

	status = send_at_cmd(gsm_text_mode, gsm_rx, &rx_size, timeout);
	if (status == RX_OK){
		if (strncmp(gsm_rx, "\r\nOK\r\n", rx_size) == 0){
			memset(gsm_rx, 0, 30);
			rx_size = 0;
			status = send_at_cmd(gsm_send_sms, gsm_rx, &rx_size, timeout);
			if (status == RX_OK){
				if (strncmp(gsm_rx, "\r\n> ", rx_size) == 0){
					strcpy(msg, message);
					len = strlen(message);
					msg[len] = 26;
					memset(gsm_rx, 0, 30);
					rx_size = 0;
					status = send_at_cmd(msg, gsm_rx, &rx_size, CALL_TIMEOUT);
					if (status == RX_OK){
						if (strncmp(gsm_rx, "\r\n+CMGS:", 8) == 0){
							memset(gsm_rx, 0, 30);
							rx_size = 0;
							return GSM_STATE_OK;
						}
					}
				}
			}
		}
	}
	memset(gsm_rx, 0, 30);
	rx_size = 0L;

	return GSM_STATE_ERROR;

}

void init_recall_function(void)
{
    GSM_State_t result_wake;
    result_wake = gsm_wake();
    if (result_wake == GSM_STATE_OK) {
        gsm_call(EMERGENCY_CONTACT_1);
        gsm_sms(EMERGENCY_CONTACT_1, MESSAGE);
        HAL_Delay(10000);
        gsm_call(EMERGENCY_CONTACT_2);
        gsm_sms(EMERGENCY_CONTACT_2, MESSAGE);
        HAL_UART_Transmit(&huart4, (uint8_t*)echo_on, strlen(echo_on), HAL_MAX_DELAY);
    }
}




