/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* PyBoard v1.1 LED Pin Assignments (from MicroPython mpconfigboard.h) */
#define LED1_RED_Pin GPIO_PIN_13
#define LED1_RED_GPIO_Port GPIOA
#define LED2_GREEN_Pin GPIO_PIN_14
#define LED2_GREEN_GPIO_Port GPIOA
#define LED3_YELLOW_Pin GPIO_PIN_15
#define LED3_YELLOW_GPIO_Port GPIOA
#define LED4_BLUE_Pin GPIO_PIN_4
#define LED4_BLUE_GPIO_Port GPIOB

/* MMA7660 Accelerometer Power Enable (PB5) */
#define MMA_POWER_Pin GPIO_PIN_5
#define MMA_POWER_GPIO_Port GPIOB

#define DAC_OUT1_Pin GPIO_PIN_4
#define DAC_OUT1_GPIO_Port GPIOA
#define USR_BTN_Pin GPIO_PIN_3
#define USR_BTN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* X-Series GPIO Pins (X1-X22) - PyBoard v1.1 from pins.csv */
#define GPIO_X1_Pin GPIO_PIN_0
#define GPIO_X1_GPIO_Port GPIOA
#define GPIO_X2_Pin GPIO_PIN_1
#define GPIO_X2_GPIO_Port GPIOA
#define GPIO_X3_Pin GPIO_PIN_2
#define GPIO_X3_GPIO_Port GPIOA
#define GPIO_X4_Pin GPIO_PIN_3
#define GPIO_X4_GPIO_Port GPIOA
#define GPIO_X5_Pin GPIO_PIN_4
#define GPIO_X5_GPIO_Port GPIOA
#define GPIO_X6_Pin GPIO_PIN_5
#define GPIO_X6_GPIO_Port GPIOA
#define GPIO_X7_Pin GPIO_PIN_6
#define GPIO_X7_GPIO_Port GPIOA
#define GPIO_X8_Pin GPIO_PIN_7
#define GPIO_X8_GPIO_Port GPIOA
#define GPIO_X9_Pin GPIO_PIN_6
#define GPIO_X9_GPIO_Port GPIOB
#define GPIO_X10_Pin GPIO_PIN_7
#define GPIO_X10_GPIO_Port GPIOB
#define GPIO_X11_Pin GPIO_PIN_4
#define GPIO_X11_GPIO_Port GPIOC
#define GPIO_X12_Pin GPIO_PIN_5
#define GPIO_X12_GPIO_Port GPIOC
#define GPIO_X17_Pin GPIO_PIN_3
#define GPIO_X17_GPIO_Port GPIOB
#define GPIO_X18_Pin GPIO_PIN_13
#define GPIO_X18_GPIO_Port GPIOC
#define GPIO_X19_Pin GPIO_PIN_0
#define GPIO_X19_GPIO_Port GPIOC
#define GPIO_X20_Pin GPIO_PIN_1
#define GPIO_X20_GPIO_Port GPIOC
#define GPIO_X21_Pin GPIO_PIN_2
#define GPIO_X21_GPIO_Port GPIOC
#define GPIO_X22_Pin GPIO_PIN_3
#define GPIO_X22_GPIO_Port GPIOC

/* Y-Series GPIO Pins (Y1-Y12) - PyBoard v1.1 from pins.csv */
#define GPIO_Y1_Pin GPIO_PIN_6
#define GPIO_Y1_GPIO_Port GPIOC
#define GPIO_Y2_Pin GPIO_PIN_7
#define GPIO_Y2_GPIO_Port GPIOC
#define GPIO_Y3_Pin GPIO_PIN_8
#define GPIO_Y3_GPIO_Port GPIOB
#define GPIO_Y4_Pin GPIO_PIN_9
#define GPIO_Y4_GPIO_Port GPIOB
#define GPIO_Y5_Pin GPIO_PIN_12
#define GPIO_Y5_GPIO_Port GPIOB
#define GPIO_Y6_Pin GPIO_PIN_13
#define GPIO_Y6_GPIO_Port GPIOB
#define GPIO_Y7_Pin GPIO_PIN_14
#define GPIO_Y7_GPIO_Port GPIOB
#define GPIO_Y8_Pin GPIO_PIN_15
#define GPIO_Y8_GPIO_Port GPIOB
#define GPIO_Y9_Pin GPIO_PIN_10
#define GPIO_Y9_GPIO_Port GPIOB
#define GPIO_Y10_Pin GPIO_PIN_11
#define GPIO_Y10_GPIO_Port GPIOB
#define GPIO_Y11_Pin GPIO_PIN_0
#define GPIO_Y11_GPIO_Port GPIOB
#define GPIO_Y12_Pin GPIO_PIN_1
#define GPIO_Y12_GPIO_Port GPIOB

/* MMA7660 Accelerometer Interrupt */
#define MMA_INT_Pin GPIO_PIN_2
#define MMA_INT_GPIO_Port GPIOB

/* SD Card Pins */
#define SD_D0_Pin GPIO_PIN_8
#define SD_D0_GPIO_Port GPIOC
#define SD_D1_Pin GPIO_PIN_9
#define SD_D1_GPIO_Port GPIOC
#define SD_D2_Pin GPIO_PIN_10
#define SD_D2_GPIO_Port GPIOC
#define SD_D3_Pin GPIO_PIN_11
#define SD_D3_GPIO_Port GPIOC
#define SD_CMD_Pin GPIO_PIN_2
#define SD_CMD_GPIO_Port GPIOD
#define SD_CK_Pin GPIO_PIN_12
#define SD_CK_GPIO_Port GPIOC
#define SD_SW_Pin GPIO_PIN_8
#define SD_SW_GPIO_Port GPIOA

/* USB Pins */
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
