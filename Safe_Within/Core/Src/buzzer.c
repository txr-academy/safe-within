/*
 * buzzer.c
 *
 *  Created on: Aug 13, 2025
 *      Author: abhir
 */

#include "buzzer.h"


/*
 * @brief Turns ON the buzzer, by resetting the "OUT" pin.
 * @param None
 * @retval None
 */
void buzzer_on(void) {
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
}

/*
 * @brief Turns OFF the buzzer, by setting the "OUT" pin.
 * @param None
 * @retval None
 */
void buzzer_off(void){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);

}
