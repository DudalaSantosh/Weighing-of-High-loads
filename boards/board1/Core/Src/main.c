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
#include "ADS1232.h"
#include <stdio.h>
//#include"tinyprintf.h"
#include <string.h>
#include "max232.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
UART_HandleTypeDef huart2;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define MY_ADDR 2
char RxBuffer[16];
char TxBuffer[64];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
volatile char buf[64];
int i=0;
const float NO_LOAD_UNITS = 777605.188f;
const float ONE_KG_UNITS  = 833276.875f;
const float KG_PER_UNIT   = 1.00f / (ONE_KG_UNITS - NO_LOAD_UNITS);
//MX485_HandleTypeDef hRs485;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

//  MX485_Init(&hRs485, &huart2, GPIOA, GPIO_PIN_8);
  ADS1232 adc;
  ADS1232_Init(
    &adc,
    GPIOA, dout_Pin,
    GPIOA, sck_Pin,
    GPIOA, pwdn_Pin,
    GPIOB, speed_Pin,
    GPIOB, gain_Pin,
    SLOW
  );
  adc.SCALE = 1.0f;
//  HAL_GPIO_WritePin(GPIOB, speed_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, gain_Pin, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOA, control_Pin, GPIO_PIN_SET);

  float units;
//  float weight=0;
//  char buf[64];
  int idx;
  float weight_kg;

  while (1)
  {
//	  int len = sprintf(buf, "HELLO123\r\n");


	  if(ADS1232_IsReady(&adc)){
		  ADS1232_GetUnits(&adc, &units, 10, 0);
		  i++;
		  units = -units;
		  float delta = units - NO_LOAD_UNITS;
		  weight_kg = (delta * KG_PER_UNIT);
		  if(weight_kg<0)weight_kg=0.0f;
  }
	  uint8_t c;
	  memset(RxBuffer, 0, sizeof(RxBuffer));
	  idx = 0;
	  int pdx=-1;
	  int flg=0;
	  while (HAL_UART_Receive(&huart2, &c, 1, HAL_MAX_DELAY) == HAL_OK) {

		  if(flg==1 ||c =='='){

			  if(pdx>=0)RxBuffer[pdx] = c;
			  pdx++;
			  flg=1;
		  }
		  idx++;
	      if (c == '\n' || idx >= sizeof(RxBuffer) - 1) break;
	  }
	  RxBuffer[idx] = '\0';

	  if (strncmp(RxBuffer, "REQ:", 4) == 0) {

	      int reqAddr = RxBuffer[4] - '0';
	      if (reqAddr == MY_ADDR) {
	          HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	          HAL_GPIO_WritePin(GPIOA, control_Pin, GPIO_PIN_SET);
//	          weight_kg=22.11f;
	          int txLen = snprintf(TxBuffer, sizeof(TxBuffer), "%+7.5f\n", weight_kg);
	          TxBuffer[txLen]='\n';
	          HAL_UART_Transmit(&huart2, TxBuffer, txLen, 1000);
//	          while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET) {}
	          HAL_GPIO_WritePin(GPIOA, control_Pin, GPIO_PIN_RESET);
	      }
	  }

	  HAL_Delay(100);
  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, pwdn_Pin|sck_Pin|dout_Pin|control_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, speed_Pin|gain_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : pwdn_Pin sck_Pin dout_Pin control_Pin */
  GPIO_InitStruct.Pin = pwdn_Pin|sck_Pin|dout_Pin|control_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : speed_Pin gain_Pin */
  GPIO_InitStruct.Pin = speed_Pin|gain_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
//  while (1)
//  {
//  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
