/*
 * @file switch.h
 * @brief Header for GPIO switch state detection functions for STM32
 * Declares function for reading switch state from a GPIO pin.
 *  Created on: Aug 13, 2025
 *      Author: abhir
 */

#ifndef INC_SWITCH_H_
#define INC_SWITCH_H_

#include "main.h"

States Switch(uint16_t GPIO_Pin);

#endif /* INC_SWITCH_H_ */
