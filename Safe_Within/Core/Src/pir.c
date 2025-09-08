/*
 * pir.c
 *
 *  Created on: Aug 10, 2025
 *      Author: abhir
 */

#include "pir.h"

volatile int pir_1_int_count = 0;
volatile int pir_2_int_count = 0;
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

