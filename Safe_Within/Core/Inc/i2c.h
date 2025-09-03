/*
 * i2c.h
 *
 *  Created on: Sep 1, 2025
 *      Author: abhir
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

extern I2C_HandleTypeDef  hi2c1;
extern uint16_t EepromAddress;
extern uint16_t DevAddress;
extern uint8_t Transmit_data[];
extern uint8_t Read_address[];
extern uint8_t Received_data;

#define I2C_TRIALS 5

#endif /* INC_I2C_H_ */
