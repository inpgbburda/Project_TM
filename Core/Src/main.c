/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LED_display.h"
#include "LPS25HB.h"
#include "temp_and_light_calc.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUF_LEN 2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


enum DisplayMode {
	DisplayTemp, DisplayLight, DisplayPress
};
disp_mode = DisplayLight;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

float max_light_measurement=-1;
float min_light_measurement =-1;

float light_raw_measurement;

uint16_t temp_to_display  = 0;
uint16_t light_to_display = 0;
int press_to_display = 0;

//mode variable to maintain light modes
int mode = 0;

//raw table of temp and light measurements (For DMA, [temp, light,....])
uint16_t temp_and_light[10];

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	static int button_start;
	//delay value to deliver debouncing
	const int delay = 300;

	if (GPIO_Pin == B1_Pin) {
		if (disp_mode == DisplayLight) {
			disp_mode = DisplayTemp;
			mode = 0;
		}else if (disp_mode == DisplayTemp){
			disp_mode = DisplayPress;
		}else if (disp_mode == DisplayPress) {
			disp_mode = DisplayLight;
		}
	}

	if (GPIO_Pin == SWITCH_LEFT_Pin) {
		if(HAL_GetTick() - button_start >= delay){
			button_start = HAL_GetTick();
			if (mode == 0) {
				mode = 1;
			}else if (mode == 1) {
				mode = 2;
			} else if (mode == 2) {
				mode = 3;
			} else if (mode == 3) {
				mode = 0;
			}
		}
	}

	if (GPIO_Pin == SWITCH_RIGHT_Pin) {
		if (mode == 2) {
			min_light_measurement = light_raw_measurement;
		}else if (mode == 3) {
			max_light_measurement = light_raw_measurement;
		}
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM1) {
		uint16_t temp_raw_measurement;
		uint16_t sum_of_temp = 0;
		uint16_t sum_of_light = 0;

		for (int i = 0; i < 10; i++) {
			if (i % 2 == 0) {
				sum_of_temp += temp_and_light[i];
			} else {
				sum_of_light += temp_and_light[i];
			}
		}
		temp_raw_measurement = sum_of_temp / 5;
		light_raw_measurement = sum_of_light / 5 ;

		// Update  temp and light and press values
		light_to_display = calculate_light(light_raw_measurement, min_light_measurement, max_light_measurement);
		temp_to_display = calculate_temp(temp_raw_measurement);
		press_to_display = LPS25HB_Read_Press();

		uint8_t data[100];
		uint16_t msg = 0;

		msg = sprintf(data, "%d %d %d\n\r", light_to_display, temp_to_display, press_to_display);
		HAL_UART_Transmit_IT(&huart2, data, msg);
	}
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);
  //init DMA that gathers temp and light measurements to one array
  HAL_ADC_Start_DMA(&hadc1, temp_and_light, 10);
  LPS25HB_Init ();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		switch (disp_mode) {

		case DisplayTemp:
			display(temp_to_display, mode, 0b1);
			break;
		case DisplayLight:
			display(light_to_display, mode, 0b0);
			break;
		case DisplayPress:
			display(press_to_display, mode, 0b0);
			break;
		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
