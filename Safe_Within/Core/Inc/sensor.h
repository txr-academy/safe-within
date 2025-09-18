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
#define SENSOR_DATA_RX_BITS 20
#define OBJECT_DISTANCE_ARR_LEN 10
#define ROOM_HEIGHT_THRESHOLD 300
#define ALERT_THRESHOLD 225
#define DETECTION_MIN_THRESHOLD 0

extern UART_HandleTypeDef huart5;

typedef enum{
	IDLE, ACTIVE, ALERT
}detection_states;

extern char sensor_data_buffer[SENSOR_DATA_BUFFER_LEN];
extern uint8_t sensor_data_bit;
extern uint8_t sensor_data_buffer_index;

float get_object_distance(void);
detection_states get_sensor_state(float object_distance);

#endif /* INC_SENSOR_H_ */
