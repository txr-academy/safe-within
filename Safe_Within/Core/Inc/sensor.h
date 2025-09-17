/*
 * sensor.h
 *
 *  Created on: Sep 17, 2025
 *      Author: abhir
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "main.h"

#define SENSOR_DATA_BUFFER_LEN 25

extern UART_HandleTypeDef huart5;

typedef enum{
	IDLE, ACTIVE, ALERT
}detection_states;

extern char sensor_data_buffer[SENSOR_DATA_BUFFER_LEN];
extern uint8_t sensor_data_bit;
extern uint8_t sensor_data_buffer_index;

#endif /* INC_SENSOR_H_ */
