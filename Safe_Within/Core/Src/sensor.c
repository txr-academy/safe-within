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
int object_distance_arr[OBJECT_DISTANCE_ARR_LEN];
int arr_counter = 0;

float get_object_distance(void)
{
	int sum = 0;
	float object_distance_avg = 0;
	while (arr_counter < OBJECT_DISTANCE_ARR_LEN)
	{
		HAL_UART_Receive(&huart5, sensor_data_buffer, SENSOR_DATA_RX_BITS, HAL_MAX_DELAY);
		sensor_data_buffer[SENSOR_DATA_RX_BITS] = '\0';
		char *token;
		char *rest = sensor_data_buffer; // Use a pointer to keep track of the rest of the string
//		int object_distance;
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
		object_distance_arr[arr_counter] = atoi(token);
		arr_counter++;
	}
	arr_counter = 0;

	for(int i = 0; i < OBJECT_DISTANCE_ARR_LEN; i++)
	{
		sum += object_distance_arr[i];
	}
	object_distance_avg = sum/10;

	return object_distance_avg;
}

detection_states get_sensor_state(float object_distance)
{

	detection_states sensor_state;
//	detection_states sensor_state;
	if ((object_distance < ALERT_THRESHOLD) && (object_distance > DETECTION_MIN_THRESHOLD)){
		sensor_state = ACTIVE;
	}
	else if((object_distance >= ALERT_THRESHOLD) && (object_distance < ROOM_HEIGHT_THRESHOLD)){
		sensor_state = ALERT;
	}
	else{
		sensor_state = IDLE;
	}
	return sensor_state;
}
