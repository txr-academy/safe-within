/*
 * buzzer.c
 *
 *  Created on: Aug 13, 2025
 *      Author: abhir
 */

#include "buzzer.h"

void buzzer_on(void) {
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
}

void buzzer_off(void){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);

}
