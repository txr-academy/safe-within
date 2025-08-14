/*
 * modbus.h
 *
 *  Created on: Aug 6, 2025
 *      Author: abhir
 */

#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"


extern UART_HandleTypeDef huart2;

extern int count;

extern uint8_t modbus_frame[];   // receive buffer
extern uint8_t reply[];
extern uint16_t holding_registers[100]; // simulate holding registers

extern uint8_t slave_address, function_code;
extern uint16_t start_address, register_count, received_crc, calculated_crc, byte_count, val;
extern uint16_t values[100];

void Process_Modbus_Frame(uint8_t *modbus_frame);
uint16_t Modbus_CRC16(uint8_t *buf, uint8_t len);

#endif /* INC_MODBUS_H_ */
