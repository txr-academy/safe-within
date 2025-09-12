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

States pir_status = 0;



/*
 * @brief Callback function to handle GPIO external interrupts of PIR_1, PIR_2 and Switch
 * @param GPIO_Pin Specifies the pin getting the interrupt
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == PIR_1_Pin){
		pir_1_int_count++;
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_3);
	}

	else if (GPIO_Pin == PIR_2_Pin) {
		pir_2_int_count++;
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_4);
	}

	else if(GPIO_Pin == switch_Pin){
		switch_flag = 0;
	}
}

States get_pir_state (void)
{
	 if ((g_time - time_check) < PIR_MONITOR_INTERVAL)
	 {
		 // Case 1: Both PIRs active
		 if ((pir_1_int_count > INTERRUPT_THRESHOLD_2) && (pir_2_int_count > INTERRUPT_THRESHOLD_2)){
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
			 pir_status = ACTIVE;
			 // LED-Blue ON
		 }
		 // Case 2: One PIR abnormal -> Alert mode
		 else if ((pir_1_int_count <= INTERRUPT_THRESHOLD) && (pir_2_int_count > INTERRUPT_THRESHOLD)){
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
			 pir_status = ALERT;
			 // LED-Red ON
		 }
		 // Case 3: Both inactive -> Idle mode
		 else if ((pir_1_int_count <= INTERRUPT_THRESHOLD) && (pir_2_int_count <= INTERRUPT_THRESHOLD)){
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
			 pir_status = IDLE;
			 // LED-Green ON
		 }
	 }

	 else
	 {
		 // reset counters and time stamps for each interval check
		 time_check = g_time;
		 pir_1_int_count = 0;
		 pir_2_int_count = 0;
		 // switch back to default state
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		 pir_status = IDLE;
		 // LED-Green ON
	 }

	 return pir_status;
}

