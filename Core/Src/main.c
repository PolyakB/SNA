/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "statemachine.h"
#include "control.h"
#include "si446x.h"

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

/* USER CODE BEGIN PV */
uint32_t startFreq = 119000000;
uint32_t stopFreq = 960000000;
uint32_t resolution = 10000;
statemachine_t sm;
si446x_t transmitter;
si446x_t receiver;

uint8_t Buffer[4] = {0};
uint8_t CMDBuffer;
uint32_t BufferData;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void BlinkLED(uint16_t time)
{
	HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_SET);
	HAL_Delay(time);
	HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_RESET);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  StateMachine_Init(&sm, State_WaitingForCommand, &huart2);
  HAL_Delay(1);
  Control_Init();
  Si446x_Init(&transmitter, Slave_TX_GPIO_Port, Slave_TX_Pin, SDN_TX_GPIO_Port, SDN_TX_Pin, &hspi3, 29997500, OOK);
  Si446x_Init(&receiver, Slave_RX_GPIO_Port, Slave_RX_Pin, SDN_RX_GPIO_Port, SDN_RX_Pin, &hspi3, 29997500, OOK);

  HAL_UART_Receive_IT(sm.huart, &CMDBuffer, 1);

//  Si446x_SetFrequency(&transmitter, 915000000);
////  Si446x_SetFrequency(&receiver, 915000000);
//  Si446x_StartTX(&transmitter);

  uint32_t freq_buffer = Si446x_GetFrequency(&transmitter);
  uint8_t mod_buffer = Si446x_GetModulationType(&transmitter);
  uint8_t band_buffer = Si446x_GetBand(&transmitter);
  uint8_t pwr_lvl_buffer = Si446x_GetPowerLevel(&transmitter);
  uint8_t rssi = Si446x_GetRSSI(&receiver);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//	  SP4T_SelectInput(0);
//
//	  for (int i = 0; i < 2; i++)
//	  {
//		SP4T_SelectInput(i);
////		DPDT_SelectInput(i);
//	  }

	  switch (sm.state){
	  	  case State_WaitingForCommand:
	  		  WaitingForCommand(&sm);
	  		  break;
	  	  case State_GettingStartFrequency:
	  		  GettingStartFrequency(&sm);
	  		  break;
	  	  case State_GettingStopFrequency:
	  		  GettingStopFrequency(&sm);
	  		  break;
	  	  case State_StartMeasurement:
	  		  StartMeasurement(&sm);
	  		  break;
	  	  case State_Measuring:
	  		  Measuring(&sm);
	  		  break;
	  	  default:
	  		  WaitingForCommand(&sm);
	  		  break;
	  }
  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if(huart->Instance == USART2)
	{
		BufferData = (Buffer[3] << 24) + (Buffer[2] << 16) + (Buffer[1] << 8) + Buffer[0];
		//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

		if(sm.state == State_WaitingForCommand)
		{

			if(CMDBuffer == GET_START_FREQUENCY_CMD)
			{
				HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_SET);
				sm.state = State_GettingStartFrequency;
				HAL_UART_Receive_IT(sm.huart, Buffer, 4);
				return;
			}
			if(CMDBuffer == GET_STOP_FREQUENCY_CMD)
			{
				HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_SET);
				sm.state = State_GettingStopFrequency;
				HAL_UART_Receive_IT(sm.huart, Buffer, 4);
				return;
			}
			if(CMDBuffer == GET_RESOLUTION_CMD)
			{
				HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_SET);
				sm.state = State_GettingResolution;
				HAL_UART_Receive_IT(sm.huart, Buffer, 4);
				return;
			}
			if(CMDBuffer == START_MEASUREMENT_CMD)
			{
				sm.state = State_StartMeasurement;
				HAL_UART_Receive_IT(sm.huart, &CMDBuffer, 1);
				return;
			}
		}
		if(sm.state == State_GettingStartFrequency)
		{
			startFreq = BufferData;
			HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_RESET);
			sm.state = State_WaitingForCommand;
			HAL_UART_Receive_IT(sm.huart, &CMDBuffer, 1);
			return;
		}
		if(sm.state == State_GettingStopFrequency)
		{
			stopFreq = BufferData;
			HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_RESET);
			sm.state = State_WaitingForCommand;
			HAL_UART_Receive_IT(sm.huart, &CMDBuffer, 1);
			return;
		}
		if(sm.state == State_GettingResolution)
		{
			resolution = BufferData;
			HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_RESET);
			sm.state = State_WaitingForCommand;
			HAL_UART_Receive_IT(sm.huart, &CMDBuffer, 1);
			return;
		}

	}
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
