/*
 * pir.c
 *
 *  Created on: Aug 10, 2025
 *      Author: abhir
 */

#include "pir.h"
//#include "main.h"

volatile int pir_1_flag = 0;
volatile int pir_2_flag = 0;

volatile uint32_t pir_1_time = 0;
volatile uint32_t pir_2_time = 0;

volatile uint32_t pir_1_off_time = 0;
volatile uint32_t pir_2_off_time = 0;

volatile int switch_count = 0;

States pir_state;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == PIR_1_Pin){
		pir_1_flag = FLAG_ON;// indicates pir_1 is ON
		pir_1_time = g_time;
		//HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);
	}

	else if (GPIO_Pin == PIR_2_Pin) {
		pir_2_flag = FLAG_ON;
		pir_2_time = g_time;
	}

	else if(GPIO_Pin == switch_Pin){
//		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		current_state = ACTIVE;
		switch_count++;
		HAL_Delay(1000);
		NVIC_ClearPendingIRQ(EXTI9_5_IRQn);

	}
}


States get_pir_state(uint32_t pir_1_time, uint32_t pir_2_time){
	  pir_1_off_time = g_time - pir_1_time;
	  pir_2_off_time = g_time - pir_2_time;
	  if ((pir_1_off_time > 1000) && (pir_2_off_time > 1000)){     // idle state , yellow led
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		  pir_state = IDLE;
	  }

	  else if ((pir_1_off_time > 1000) && (pir_2_off_time < 1000) && (pir_state == ACTIVE)){      // emergency, red
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
		  pir_state = ALERT;
	  }
	  else if ((pir_2_off_time < 1000) && (pir_2_off_time > 100) && (pir_1_off_time < 1000)){         		// active state, blue
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		  pir_state = ACTIVE;
	  }

//	  else{
//		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);		// error state, all leds on
//		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
//		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
//	  }
	  return pir_state;
}
