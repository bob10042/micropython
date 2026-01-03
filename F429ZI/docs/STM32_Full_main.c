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
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* Printf redirect to USART3 */
int __io_putchar(int ch) {
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

volatile uint8_t button_pressed = 0;
uint32_t led_mode = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == USER_BUTTON_Pin) {
    button_pressed = 1;
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("\r\n========================================\r\n");
  printf("  STM32F429ZI NUCLEO Full Test\r\n");
  printf("========================================\r\n");
  printf("System Clock: 180 MHz\r\n");
  printf("Peripherals Initialized:\r\n");
  printf("  - GPIO: PB0(LED1), PB7(LED2), PB14(LED3)\r\n");
  printf("  - Button: PC13 (interrupt)\r\n");
  printf("  - USART3: PD8/PD9 @ 115200 baud\r\n");
  printf("  - SPI1: PA5/PA6/PA7 (5.6 Mbps)\r\n");
  printf("  - I2C1: PB8/PB9 (Fast mode)\r\n");
  printf("Press B1 to change LED mode\r\n");
  printf("========================================\r\n\r\n");
  
  uint32_t tick = 0;
  uint32_t last = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (button_pressed) {
      button_pressed = 0;
      led_mode = (led_mode + 1) % 4;
      printf("Button! Mode: %lu\r\n", led_mode);
      HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED2_BLUE_GPIO_Port, LED2_BLUE_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED3_RED_GPIO_Port, LED3_RED_Pin, GPIO_PIN_RESET);
    }
    
    if (HAL_GetTick() - last >= 200) {
      last = HAL_GetTick();
      tick++;
      
      switch (led_mode) {
        case 0: // All blink
          HAL_GPIO_TogglePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin);
          HAL_GPIO_TogglePin(LED2_BLUE_GPIO_Port, LED2_BLUE_Pin);
          HAL_GPIO_TogglePin(LED3_RED_GPIO_Port, LED3_RED_Pin);
          break;
        case 1: // Sequential
          HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, (tick%3==0)?GPIO_PIN_SET:GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED2_BLUE_GPIO_Port, LED2_BLUE_Pin, (tick%3==1)?GPIO_PIN_SET:GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED3_RED_GPIO_Port, LED3_RED_Pin, (tick%3==2)?GPIO_PIN_SET:GPIO_PIN_RESET);
          break;
        case 2: // Binary counter
          HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, (tick&1)?GPIO_PIN_SET:GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED2_BLUE_GPIO_Port, LED2_BLUE_Pin, (tick&2)?GPIO_PIN_SET:GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED3_RED_GPIO_Port, LED3_RED_Pin, (tick&4)?GPIO_PIN_SET:GPIO_PIN_RESET);
          break;
        case 3: // Green only
          HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_SET);
          break;
      }
      
      if (tick % 5 == 0) printf("[%05lu] Running... Mode=%lu\r\n", tick/5, led_mode);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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
