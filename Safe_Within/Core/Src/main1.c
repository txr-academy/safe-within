/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "pir.h"				// remove this later
#include "buzzer.h"
//#include "gsm.h"
//#include "gsm2.h"
#include "gsm6.h"
#include "switch.h"
#include "string.h"
#include "modbus.h"
//#include "modbus.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
//CRC_HandleTypeDef hcrc;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile uint32_t g_time = 0;
volatile int uart_flag = 0;

volatile uint32_t uart_int_time = 0; // time of uart interrupt
volatile uint32_t uart_idle_time = 0;  // uart idle time
uint32_t time_check = 0;
uint32_t emergency_time_check = 0;

uart_t request_t, response_t;

uint8_t data = 0;
int uart_index = 0;
//int line_ready=0;

States current_state = IDLE;
States last_state = IDLE;
uint32_t alert_start_time = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
    	g_time++ ;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	/*
	 * 		Callback for modbus
	 */
	if (huart->Instance == USART2){
		uart_flag = 1;
		uart_int_time = g_time;
		modbus_frame[uart_index++] = data;
		HAL_UART_Receive_IT(&huart2, &data, 1);
	}

	/*
	 * 			Callback for gsm
	 */
    if (huart->Instance == UART4)
    {
        if (sim_rx_index < SIM_RX_BUFFER_SIZE - 1)
        {
            sim_rx_buffer[sim_rx_index++] = sim_rx_byte;
//            if (sim_rx_byte == '\n') {
//                            line_ready = 1;
//                        }
        }

        HAL_UART_Receive_IT(&huart4, &sim_rx_byte, 1);
    }
//    if (huart->Instance == huart4.Instance && gsm_state != GSM_STAGE_DONE) {
//        rx_len++;
//        if (rx_len < RX_BUFFER_SIZE - 1) {
//            rx_buffer[rx_len] = '\0';
//
//            if (strstr((char*)rx_buffer, "\r\nOK\r\n") != NULL) {
//                rx_len = 0; // Clear buffer for next command
//
//                if (gsm_state == GSM_STAGE_WAKE) {
//                    gsm_state = GSM_STAGE_ECHO_OFF;
//                    // Disable echo after wakeup
//                    HAL_UART_Transmit(&huart4, (uint8_t*)"ATE0\r\n", strlen("ATE0\r\n"), HAL_MAX_DELAY);
//
//                } else if (gsm_state == GSM_STAGE_ECHO_OFF) {
//                    gsm_state = GSM_STAGE_AT_SENT;
//                    // Send AT to confirm connection once more (optional)
//                    HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), HAL_MAX_DELAY);
//
//                } else if (gsm_state == GSM_STAGE_AT_SENT) {
//                    gsm_state = GSM_STAGE_DIAL;
//                    // Send dial command
//                    HAL_UART_Transmit(&huart4,
//                        (uint8_t*)"ATD+917994277760;\r\n",
//                        strlen("ATD+917994277760;\r\n"),
//                        HAL_MAX_DELAY);
//                    gsm_state = GSM_STAGE_DONE;
//                }
//            }
//            gsm_recieve_function();
//        }
//    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
 // States pir_state;

  /*
   * Testing the peripherals
   */
  buzzer_on();
  HAL_Delay(500);
  buzzer_off();
//  HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), HAL_MAX_DELAY);
//  echo_on();

  GSM_Reset();
  result = gsm_init();
  if(result!=GSM_STATE_OK){
	  while(1){
	  }
  }
  HAL_TIM_Base_Start_IT(&htim6);
//  HAL_UART_Receive_IT(&huart2, &data, 1);
//  gsm_init();
  time_check = g_time;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		  if (result != GSM_STATE_OK){
//			  result = gsm_init();
//			  if (result == GSM_STATE_OK){
//				  result = gsm_call(EMERGENCY_CONTACT_2);
//				  if (result != GSM_STATE_OK){
//					  result = gsm_init();
//					  if (result == GSM_STATE_OK){
//						  result = gsm_call(EMERGENCY_CONTACT_3);
//						  if ( result != GSM_STATE_OK ){
//							  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//							  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
//							  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
//						  }
//						  else{
//							  gsm_sms(EMERGENCY_CONTACT_3, MESSAGE);
//						  }
//					  }
//				  }
//				  else {
//					  gsm_sms(EMERGENCY_CONTACT_2, MESSAGE);
//				  }
//			  }
//		  }
//		  else{
//			  gsm_sms(EMERGENCY_CONTACT_1, MESSAGE);
//		  }
//	  }

//  code start from here!!!!1!!

//	  while ((g_time - time_check) < 2000)
//	  {
//
//		  if ((pir_1_int_count > 3) && (pir_2_int_count > 3)){
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
//			  pir_state = ACTIVE;
//		  }
//		  else if ((pir_1_int_count < 3) && (pir_2_int_count > 3)){
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
//			  pir_state = ALERT;
//			  emergency_time_check = g_time;
//			  break;
//		  }
//		  else if ((pir_1_int_count <= 2) && (pir_2_int_count <= 2)){
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
//			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
//			  pir_state = IDLE;
//
//		  }
//
//		  if (switch_count == 0){
//			  buzzer_off();
//			  pir_state = ACTIVE;
//			  switch_count=1;
//		  }
//
//
//		  if (uart_flag){
//			  uart_idle_time = g_time - uart_int_time;
//			  if (uart_idle_time > UART_TIMEOUT){
//				  memcpy(request_t.message, modbus_frame, uart_index);
//				  request_t.size = uart_index;
//				  uart_index = 0;
//				  uart_flag = 0;
//				  Process_Modbus_Frame(modbus_frame);
//
//			  }
//		  }
//
//
//
//	  }

//	  if (pir_state == ALERT){
//		  if (g_time - emergency_time_check > 1000){
//			  if(result==GSM_STATE_OK){
//				  buzzer_on();
//				  init_recall_function();
//			  }
//			  else
//			  {
//				  gsm_init();
//				  init_recall_function();
//			  }
//		  }
//		  else {
//			  buzzer_off();
//		  }
//	  }

//	  if (pir_state == ALERT){
	  int gsm_count=0;
      while(gsm_count <=3){
		  if (result == GSM_STATE_OK){
			  //buzzer_on();
			  call_sms_function();
//
		  }
		  else{
			  result=gsm_init();
			  gsm_count++;
//
//
		  }
	  }

//	  else {
//		  buzzer_off();
//	  }


	  if (uart_flag){
		  uart_idle_time = g_time - uart_int_time;
		  if (uart_idle_time > UART_TIMEOUT){
			  memcpy(request_t.message, modbus_frame, uart_index);
			  request_t.size = uart_index;
			  uart_index = 0;
			  uart_flag = 0;
			  Process_Modbus_Frame(modbus_frame);

		  }
	  }

	  time_check = g_time;
	  emergency_time_check = 0;
	  pir_1_int_count = 0;
	  pir_2_int_count = 0;

/* integrated code
	  current_state = get_pir_state(pir_1_time,pir_2_time);
	  if (current_state == ALERT){
		  if (last_state != ALERT){
			  alert_start_time = g_time; // set ONCE on entering ALERT
		  }
		  else if ((g_time - alert_start_time) >= 1000){
			  buzzer_on();
			  result = gsm_wake();
			  if (result == GSM_STATE_OK){
				  result = gsm_call(EMERGENCY_CONTACT_1);
				  if (result == GSM_STATE_OK){
					  gsm_sms(EMERGENCY_CONTACT_1, MESSAGE);
				  }
			  }

			  else
			  {
				  		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		  	  	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		  	  	  	  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
			  }
		  }
	  }
*/

//	        	  HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
//	        	  HAL_Delay(2000);
//	        	  HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
//	        	  HAL_UART_Transmit(&huart4, (uint8_t*)"ATD+917994277760;\r\n", strlen("ATD+917994277760;\r\n"), HAL_MAX_DELAY);
//			  if ( switch_count == 0){
//				  buzzer_off();
//				  current_state = ACTIVE;
//				  switch_count = 1;
//				  HAL_Delay(15000);
//			  }
//	  }

//	   else { 							// Not in ALERT
//		   alert_start_time = 0;
//	       buzzer_off();
//	   }
//	   last_state = current_state;

//	   current_state = get_pir_state(pir_1_time,pir_2_time);
//	   if (current_state == ALERT){
//		   buzzer_on();
//		   HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
//		   HAL_Delay(2000);
//		   HAL_UART_Transmit(&huart4, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 1000);
//		   HAL_UART_Transmit(&huart4, (uint8_t*)"ATD+917994277760;\r\n", strlen("ATD+917994277760;\r\n"), HAL_MAX_DELAY);
//		   if ( switch_count == 0){
//			   buzzer_off();
//			   current_state = ACTIVE;
//			   switch_count = 1;
//			   HAL_Delay(15000);
//		   }
//
//	   }
//	   else{
//		   buzzer_off();
//	   }


//	  if (uart_flag){
//		  uart_idle_time = g_time - uart_int_time;
//		  if (uart_idle_time > UART_TIMEOUT){
//			  memcpy(request_t.message, modbus_frame, uart_index);
//			  request_t.size = uart_index;
//			  uart_index = 0;
//			  uart_flag = 0;
//			  Process_Modbus_Frame(modbus_frame);
//
//		  }
//	  }
//
//
//
//
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
//  hcrc.Instance = CRC;
//  if (HAL_CRC_Init(&hcrc) != HAL_OK)
//  {
//    Error_Handler();
//  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 83;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 9999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : STLK_RX_Pin STLK_TX_Pin */
  GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PIR_1_Pin PIR_2_Pin */
  GPIO_InitStruct.Pin = PIR_1_Pin|PIR_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
  GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : switch_Pin */
  GPIO_InitStruct.Pin = switch_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(switch_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
