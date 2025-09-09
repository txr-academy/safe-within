/*
 /**
 * @file modbus.h
 * @brief Modbus RTU protocol implementation over UART
 * Declares Modbus function codes, external variables, and functions for processing Modbus frames.
 *  Created on: Aug 6, 2025
 *      Author: abhiram and anandu
 */

#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_

#include "stdint.h"
#include "stm32f4xx_hal.h"

/** @brief Modbus function code to read single register */
#define READ_SINGLE_REGISTER 0x03

/** @brief Modbus function code to write single register */
#define WRITE_SINGLE_REGISTER 0x06

/** @brief Modbus function code to write multiple registers */
#define WRITE_MULTIPLE_REGISTERS 0x10

extern UART_HandleTypeDef huart2;

/** @brief Buffer for recieving  Modbus frame */
extern uint8_t modbus_frame[];

/** @brief Buffer for sending Modbus reply */
extern uint8_t reply[];

/** @brief Simulated Modbus holding registers */
extern uint16_t holding_registers[100];

/** @brief Modbus slave address and function code */
extern uint8_t slave_address, function_code;

/** @brief Starting register address */
extern uint16_t start_address;

/** @brief Number of registers requested */
extern uint16_t register_count;


/** @brief CRC received in frame */
extern uint16_t received_crc ;

/** @brief CRC calculated from frame */
extern uint16_t calculated_crc;

/** @brief Byte count in response */
extern uint16_t byte_count;

/** @brief Temporary value storage */
extern uint16_t val;

/** @brief Buffer to hold data values for multiple registers */
extern uint16_t values[100];

/*
 * Function to parse the request message
 */
void Process_Modbus_Frame(uint8_t *modbus_frame);


/*
 * Function to calculate crc_16
 */
uint16_t Modbus_CRC16(uint8_t *buf, uint8_t len);

#endif /* INC_MODBUS_H_ */
