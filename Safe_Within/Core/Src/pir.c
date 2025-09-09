/*
 * @file pir.c
 * @brief PIR sensor interrupt handler implementation using STM32 HAL GPIO EXTI callback
 * Implements external interrupt callback to count PIR sensor triggers and switch flag.
 *  Created on: Aug 10, 2025
 *      Author: abhiram and anandu
 */

#include "pir.h"

/** @brief PIR sensor 1 interrupt count */
volatile int pir_1_int_count = 0;

/** @brief PIR sensor 2 interrupt count */
volatile int pir_2_int_count = 0;

/** @brief Switch debounce flag . */
volatile int switch_flag = 1;


/*
 * @brief Callback function to handle GPIO external interrupts of PIR_1, PIR_2 and Switch
 * @param GPIO_Pin Specifies the pin getting the interrupt
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == PIR_1_Pin){
		pir_1_int_count++;
	}

	else if (GPIO_Pin == PIR_2_Pin) {
		pir_2_int_count++;
	}

	else if(GPIO_Pin == switch_Pin){
		switch_flag = 0;
	}
}

