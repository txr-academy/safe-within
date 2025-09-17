/*
 * sensor.c
 *
 *  Created on: Sep 17, 2025
 *      Author: abhir
 */

#include "sensor.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char sensor_data_buffer[SENSOR_DATA_BUFFER_LEN];

int get_object_distance(void)
{
	HAL_UART_Receive(&huart5, sensor_data_buffer, 20, HAL_MAX_DELAY);
	sensor_data_buffer[20] = '\0';
	char *token;
	char *rest = sensor_data_buffer; // Use a pointer to keep track of the rest of the string
	int object_distance;
	int detection_flag = 0;

	if (strstr(sensor_data_buffer, "ON")){
		detection_flag = 1;
	}
	// First split by pipe character '|'
	while (detection_flag){
		token = strtok_r(rest, "\r\n", &rest);
		if (strstr(token, "Range")){
			strtok_r(token, " ", &token);
			break;
		}
	}
	object_distance = atoi(token);
	return object_distance;
}

detection_states get_sensor_state(int object_distance)
{
	detection_states sensor_state;
//	detection_states sensor_state;
	if (object_distance < 2.25){
		sensor_state = ACTIVE;
	}
	else if((object_distance >= 2.25) && (object_distance < 3)){
		sensor_state = ALERT;
	}
	else{
		sensor_state = IDLE;
	}
	return sensor_state;
}
