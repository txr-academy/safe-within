/*
 * @file gsm6.c
 * @brief GSM communication using STM32 HAL UART.
 *  Created on: Aug 26, 2025
 *      Author: abhiram and anandu
 */

#include "gsm.h"
#include "pir.h"
#include "buzzer.h"

uint8_t sim_rx_byte = 0;
char sim_rx_buffer[SIM_RX_BUFFER_SIZE] = {0};
uint16_t sim_rx_index = 0;
uint8_t line_ready = 0;

GSM_State_t result = 0;
GSM_State_t result_wake = 0;
GSM_Response_t status;



/**
 * @brief Begins UART reception for GSM byte stream (interrupt).
 */
void gsm_start_rx(void)
{
    sim_rx_index = 0;
    line_ready = 0;
    memset(sim_rx_buffer, 0, SIM_RX_BUFFER_SIZE);
    HAL_UART_Receive_IT(&huart4, &sim_rx_byte, 1);
}

/**
 * @brief Sends AT command and receives GSM response.
 * @param at_cmd AT command string.
 * @param sim_response_buffer Response storage buffer.
 * @param size Bytes received.
 * @param timeout Maximum allowed  timeout.
 * @return RX_OK on success, RX_TIMEOUT otherwise.
 */
GSM_Response_t send_at_cmd( char *at_cmd, char *sim_response_buffer, int *size, int timeout)
{
	uint32_t tickstart;
	gsm_start_rx();
	HAL_UART_Transmit(&huart4, (uint8_t*)at_cmd, strlen(at_cmd), HAL_MAX_DELAY);
	tickstart = HAL_GetTick();
	while(1){

		if ((HAL_GetTick() - tickstart) > timeout){
			sim_rx_buffer[sim_rx_index] = '\0';
			strcpy(sim_response_buffer, sim_rx_buffer);
			*size = sim_rx_index+1;
		     sim_rx_index = 0;
			 break;
		}
	}
	if(*size == 0){
		return RX_TIMEOUT;
	}
	else{
		return RX_OK;
	}
}

GSM_State_t gsm_init(void)
{
	char gsm_wake[] = "AT\r\n";
	char gsm_rx[50];
	int rx_size=0;
	//int *rx_size = NULL;
	int timeout = AT_TIMEOUT;
	char echo_off[] = "ATE0\r\n";

//	GSM_Response_t status;
	HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), HAL_MAX_DELAY);
	memset(gsm_rx, 0, sizeof(gsm_rx));
	HAL_Delay(100);

	status = send_at_cmd(gsm_wake, gsm_rx, &rx_size, timeout);
	if (status == RX_OK){

		if(strstr(gsm_rx,"OK")){
			HAL_Delay(100);
			memset(gsm_rx, 0, sizeof(gsm_rx));
			rx_size =0;
			status = send_at_cmd(echo_off, gsm_rx, &rx_size, timeout);

			if(strstr(gsm_rx,"OK")){
				HAL_Delay(100);

				return GSM_STATE_OK;
			}
		}
	}
	return GSM_STATE_ERROR;
}

/**
 * @brief Wakes up GSM module by sending "AT" command.
 * @return GSM_STATE_OK if success, GSM_STATE_ERROR otherwise.
 */
GSM_State_t gsm_wake(void)
{
	char gsm_wake_up[] = "AT\r\n";
	char gsm_rx[20];
	int rx_size = 0;
	int timeout = AT_TIMEOUT;
	memset(gsm_rx, 0, sizeof(gsm_rx));
	status = send_at_cmd(gsm_wake_up, gsm_rx, &rx_size, timeout);
		if (status == RX_OK){
			if (strncmp(gsm_rx, "\r\nOK\r\n", rx_size) == 0){
				rx_size = 0;
				return GSM_STATE_OK;
			}
		}
		return GSM_STATE_ERROR;
}

/**
 * @brief Initiates call to provided number.
 * @param number Phone number
 * @return GSM_STATE_OK if call succeed, GSM_STATE_ERROR otherwise.
 */
GSM_State_t gsm_call(const char *number)
{
    char dial[30];
    snprintf(dial, sizeof(dial), "ATD+%s;\r\n", number);
    char gsm_rx[50];
    int rx_size = 0;
    int timeout = AT_TIMEOUT;

    memset(gsm_rx, 0, sizeof(gsm_rx));
    status = send_at_cmd(dial, gsm_rx, &rx_size, timeout);
    if (status == RX_OK){
    	if (strncmp(gsm_rx, "\r\nOK\r\n", rx_size) == 0){
    		HAL_Delay(1000);
    		rx_size = 0;

    		return GSM_STATE_OK;
    	}
    }
    return GSM_STATE_ERROR;
}


/**
 * @brief Sends SMS to a number via GSM.
 * @param number  phone number.
 * @param message Message string to send.
 * @return GSM_STATE_OK if SMS is sent, GSM_STATE_ERROR otherwise.
 */
GSM_State_t gsm_sms(const char *number, const char *message)
{
	char gsm_text_mode[] = "AT+CMGF=1\r\n";
	char gsm_rx[30];
	int rx_size = 0;
	int timeout = AT_TIMEOUT;
	char gsm_send_sms[50];
	char msg[40] = {0};
	int len;

	snprintf(gsm_send_sms, sizeof(gsm_send_sms), "AT+CMGS=\"+%s\"\r\n", number);
	memset(gsm_rx, 0, sizeof(gsm_rx));

	status = send_at_cmd(gsm_text_mode, gsm_rx, &rx_size, timeout);
	if (status == RX_OK){
		if (strncmp(gsm_rx, "\r\nOK\r\n", rx_size) == 0){
			HAL_Delay(100);
			memset(gsm_rx, 0, sizeof(gsm_rx));
			rx_size = 0;

			status = send_at_cmd(gsm_send_sms, gsm_rx, &rx_size, timeout);
			if (status == RX_OK){
				if (strncmp(gsm_rx, "\r\n> ", rx_size) == 0){
					len = strlen(message);

					if (len < sizeof(msg) - 1) {
						strcpy(msg, message);
						msg[len] = 26;
						msg[len + 1] = '\0';

						HAL_Delay(100);
					    memset(gsm_rx, 0, sizeof(gsm_rx));
					    rx_size = 0;
					    status = send_at_cmd(msg, gsm_rx, &rx_size, CALL_TIMEOUT);

					    if (status == RX_OK){
						    if (strncmp(gsm_rx, "\r\n+CMGS:", 8) == 0){

							rx_size = 0;
							return GSM_STATE_OK;
						}
					}
				}
			}
     	}
     }
  }
	return GSM_STATE_ERROR;
}

void call_sms_function(void)
{
    GSM_State_t result_wake;
    result_wake = gsm_wake();
    int call_sms_maxtry =3;
    int call_sms_count=0;
    // Try call and sms by checking gsm condition state  3 times
    while( call_sms_count <= call_sms_maxtry){
       if (result_wake == GSM_STATE_OK) {
           gsm_call(EMERGENCY_CONTACT_1);
           gsm_sms(EMERGENCY_CONTACT_1, MESSAGE);
           uint32_t call_time;
           call_time = g_time;
           // this loop acts as the timer before the second call
           while(1){
        	   if(g_time - call_time > CALL_INTERVAL){
            	  result_wake = gsm_wake();
            	  int second_call_maxtry = 3;
            	  int second_call_count = 0;
            	  // call and sms secondary contact number after primary number by again AT\r\n command and trying 3 times for checking for appropriate response
            	  while(second_call_count<=second_call_maxtry){
            		  if (result_wake == GSM_STATE_OK) {
            			  gsm_call(EMERGENCY_CONTACT_3);
            			  gsm_sms(EMERGENCY_CONTACT_3, MESSAGE);
            			  HAL_Delay(1000);
            			  break;
            		  }
            		  else{
            			  result_wake = gsm_wake();
            			  second_call_count++;
            			  HAL_Delay(100);
            		  }
            	  }
        	   }
        	   if (switch_flag == 0){
        		   buzzer_off();
//        		   pir_state = ACTIVE;
        		   switch_flag = 1;
        		   break;
        	   }
           }
           break;
       }
       else{
    	   result_wake = gsm_wake();
    	   call_sms_count++;
    	   HAL_Delay(100);
       }
    }
}

/**
 * @brief Sends GSM reset command via UART to ensure GSM is in Command mode, not in text prompt mode.
 */
void gsm_reset(void)
{
	    uint8_t esc = 0x1B;  // ESC key
	    HAL_UART_Transmit(&huart4, &esc, 1, HAL_MAX_DELAY);
	    HAL_Delay(200);
	    HAL_UART_Transmit(&huart4, &esc, 1, HAL_MAX_DELAY);
	    HAL_Delay(200);
}
