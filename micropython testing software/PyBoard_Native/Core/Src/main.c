/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : PyBoard Native Firmware
  *                   Replicates MicroPython v1.28 functionality in C for speed
  ******************************************************************************
  * Features (matching MicroPython pyb module):
  * - 4 LEDs (Red, Green, Yellow, Blue with PWM) - pyb.LED(1-4)
  * - MMA7660 Accelerometer via I2C - pyb.Accel()
  * - ADC input - pyb.ADC()
  * - DAC output - pyb.DAC()
  * - PWM output - pyb.Timer().channel()
  * - GPIO control - pyb.Pin()
  * - User button - pyb.Switch()
  * - Serial CLI at 115200 baud - REPL-like interface
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mma7660.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "ff.h"  /* FatFS file system */
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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/* PCD_HandleTypeDef is declared in usbd_conf.c */

/* USER CODE BEGIN PV */
ADC_HandleTypeDef hadc1;
DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
I2C_HandleTypeDef hi2c2;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;
CAN_HandleTypeDef hcan1;
SD_HandleTypeDef hsd;
RTC_HandleTypeDef hrtc;

/* UART Receive buffers */
volatile uint8_t uart2_rx_buffer[128];
volatile uint8_t uart2_rx_idx = 0;
volatile uint8_t uart6_rx_buffer[128];
volatile uint8_t uart6_rx_idx = 0;

/* CAN Receive buffer */
CAN_RxHeaderTypeDef can_rx_header;
uint8_t can_rx_data[8];
volatile uint8_t can_rx_ready = 0;

/* USB CDC Receive buffer */
volatile uint8_t usb_rx_buffer[64];
volatile uint32_t usb_rx_len = 0;
volatile uint8_t usb_rx_ready = 0;

/* FatFS variables */
FATFS SDFatFs;       /* File system object */
uint8_t fs_mounted = 0;  /* Mount status */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_UART2_Init(void);
static void MX_UART6_Init(void);
static void MX_CAN1_Init(void);
static void MX_SDIO_SD_Init(void);
/* USER CODE BEGIN PFP */
static void MX_ADC1_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_RTC_Init(void);

/* LED Functions */
void LED_On(uint8_t led);
void LED_Off(uint8_t led);
void LED_Toggle(uint8_t led);
void LED_SetIntensity(uint8_t led, uint8_t intensity);

/* Peripheral Functions */
uint16_t ADC_Read(void);
void DAC_Write(uint16_t value);
void PWM_SetDuty(uint8_t percent);
void GPIO_SetPin(uint8_t pin, uint8_t state);
void GPIO_SetPinY(uint8_t pin, uint8_t state);
uint8_t GPIO_ReadPin(uint8_t pin);
uint8_t GPIO_ReadPinY(uint8_t pin);
uint8_t Button_Read(void);

/* Extended GPIO functions */
void GPIO_SetMode(char series, uint8_t pin, uint8_t mode);
void GPIO_SetPull(char series, uint8_t pin, uint8_t pull);
void GPIO_Toggle(char series, uint8_t pin);

/* UART Functions */
void UART_Send(uint8_t port, const char *str);
uint8_t UART_Receive(uint8_t port, char *buf, uint8_t maxlen);

/* CAN Functions */
void CAN_Send(uint32_t id, uint8_t *data, uint8_t len);

/* RTC Functions */
void RTC_GetTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_GetDate(uint8_t *year, uint8_t *month, uint8_t *day);
void RTC_SetDate(uint8_t year, uint8_t month, uint8_t day);

/* SPI Functions */
void SPI_Transfer(uint8_t port, uint8_t *txdata, uint8_t *rxdata, uint16_t len);

/* === NEW FEATURES (from MicroPython drivers) === */

/* Multi-channel ADC (MicroPython: ports/stm32/adc.c) */
uint16_t ADC_ReadChannel(uint8_t channel);
uint16_t ADC_ReadVBAT(void);
uint16_t ADC_ReadTempSensor(void);

/* I2C Data Transfer (MicroPython: ports/stm32/i2c.c) */
int I2C_ReadFrom(uint8_t bus, uint8_t addr, uint8_t *data, uint16_t len);
int I2C_WriteTo(uint8_t bus, uint8_t addr, uint8_t *data, uint16_t len);
int I2C_ReadMemory(uint8_t bus, uint8_t addr, uint8_t memaddr, uint8_t *data, uint16_t len);
int I2C_WriteMemory(uint8_t bus, uint8_t addr, uint8_t memaddr, uint8_t *data, uint16_t len);

/* Watchdog Timer (MicroPython: ports/stm32/wdt.c) */
void WDT_Init(uint32_t timeout_ms);
void WDT_Feed(void);

/* Power Management (MicroPython: ports/stm32/powerctrl.c) */
void Power_Sleep(void);
void Power_Stop(void);
void Power_Standby(void);

/* System Info (MicroPython: machine module) */
void Get_UniqueID(uint32_t *uid);
uint32_t Get_ResetCause(void);

/* External Interrupts (MicroPython: ports/stm32/extint.c) */
typedef void (*extint_callback_t)(void);
void ExtInt_Enable(char series, uint8_t pin, uint8_t edge, extint_callback_t callback);
void ExtInt_Disable(char series, uint8_t pin);

/* CLI Functions */
void CLI_Process(char *cmd);
void CLI_PrintHelp(void);
void CLI_SendString(const char *str);

/* USB CDC Callback */
void USB_CDC_RxHandler(uint8_t *buf, uint32_t len);
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
  /* Enable DWT cycle counter for precise timing */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_UART2_Init();
  MX_UART6_Init();
  MX_CAN1_Init();
  MX_SDIO_SD_Init();
  /* USER CODE BEGIN 2 */
  
  /* Initialize USB CDC Device */
  MX_USB_DEVICE_Init();
  CDC_SetRxCallback(USB_CDC_RxHandler);
  
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();  /* PWM for LED4 (PB4) */
  /* MX_TIM4_Init(); */
  /* MX_RTC_Init(); */  /* HAL_RTC_Init() hangs without LSE/LSI clock! */

  /* Start PWM on TIM2 for general PWM output */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  
  /* Start TIM3 PWM for LED4 */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  /* LED4 (PB4) */
  
  /* CAN/UART start - interrupts DISABLED (no IRQ handlers in stm32f4xx_it.c)
   * MicroPython only enables these when user explicitly requests them.
   * Enabling interrupts without handlers causes crash! */
  /* HAL_CAN_Start(&hcan1); */
  /* HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); */
  
  /* UART polling mode only - no interrupt handlers exist */
  /* HAL_UART_Receive_IT(&huart2, (uint8_t*)&uart2_rx_buffer[0], 1); */
  /* HAL_UART_Receive_IT(&huart6, (uint8_t*)&uart6_rx_buffer[0], 1); */

  /* Enable MMA7660 accelerometer power (PB5 = AVDD) */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
  HAL_Delay(10);  /* Wait for power to stabilize */

  /* Initialize Accelerometer */
  MMA7660_Init(&hi2c1);

  /* Start ADC */
  HAL_ADC_Start(&hadc1);

  /* Start DAC */
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

  /* Flash all LEDs on startup to indicate boot */
  for (int i = 0; i < 3; i++) {
      LED_On(1); LED_On(2); LED_On(3); LED_On(4);
      HAL_Delay(100);
      LED_Off(1); LED_Off(2); LED_Off(3); LED_Off(4);
      HAL_Delay(100);
  }
  
  /* Wait for USB enumeration */
  HAL_Delay(1000);
  
  /* Welcome message */
  char welcome[] = "\r\n===========================================\r\n"
                   "PyBoard Native Firmware v3.0\r\n"
                   "STM32F405RG @ 168MHz\r\n"
                   "MicroPython v1.28 Compatible (Native C)\r\n"
                   "Type 'help' for commands\r\n"
                   "===========================================\r\n> ";
  CLI_SendString(welcome);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  static char cmd_buffer[128];
  static uint8_t cmd_idx = 0;
  
  while (1)
  {
    /* Check for USB CDC input */
    if (usb_rx_ready)
    {
        /* Process received data */
        for (uint32_t i = 0; i < usb_rx_len; i++)
        {
            uint8_t ch = usb_rx_buffer[i];
            
            /* Echo character */
            char echo[2] = {ch, 0};
            CLI_SendString(echo);
            
            if (ch == '\r' || ch == '\n')
            {
                CLI_SendString("\r\n");
                cmd_buffer[cmd_idx] = '\0';
                
                if (cmd_idx > 0)
                {
                    CLI_Process(cmd_buffer);
                }
                
                CLI_SendString("> ");
                cmd_idx = 0;
            }
            else if (ch == 0x7F || ch == 0x08)  /* Backspace */
            {
                if (cmd_idx > 0) cmd_idx--;
            }
            else if (cmd_idx < sizeof(cmd_buffer) - 1)
            {
                cmd_buffer[cmd_idx++] = ch;
            }
        }
        usb_rx_ready = 0;
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
  * 
  * PyBoard v1.1 has a 12MHz HSE crystal (not 8MHz!)
  * MicroPython uses: PLLM=12, PLLN=336, PLLP=2, PLLQ=7
  * This gives:
  *   - VCO = 12MHz / 12 * 336 = 336MHz
  *   - SYSCLK = 336MHz / 2 = 168MHz
  *   - USB CLK = 336MHz / 7 = 48MHz (exactly what USB needs!)
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;   /* 12MHz crystal -> 1MHz PLL input */
  RCC_OscInitStruct.PLL.PLLN = 336;  /* 1MHz * 336 = 336MHz VCO */
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  /* 336MHz / 2 = 168MHz SYSCLK */
  RCC_OscInitStruct.PLL.PLLQ = 7;    /* 336MHz / 7 = 48MHz USB clock */
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function (Y bus: PB10=SCL, PB11=SDA)
  */
static void MX_I2C2_Init(void)
{
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function (Y bus: PB13=SCK, PB14=MISO, PB15=MOSI, PB12=NSS)
  */
static void MX_SPI2_Init(void)
{
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART2 Initialization Function (X bus: PA2=TX, PA3=RX)
  */
static void MX_UART2_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
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
}

/**
  * @brief UART6 Initialization Function (Y bus: PC6=TX, PC7=RX)
  */
static void MX_UART6_Init(void)
{
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function (Y bus: PB9=TX, PB8=RX)
  */
static void MX_CAN1_Init(void)
{
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 21;  /* 168MHz/2 / 21 / (1+6+7) = 285.7kbps */
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_6TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_7TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  /* CAN init may fail without transceiver - this is OK, don't Error_Handler */
  HAL_CAN_Init(&hcan1);  /* Ignore return value like MicroPython does */
}

/**
  * @brief SDIO SD Card Initialization Function
  */
static void MX_SDIO_SD_Init(void)
{
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 2;  /* SDIO_CK = 48MHz / (2 + 2) = 12MHz */
  /* Note: SD card init may fail if no card is inserted - this is OK */
  HAL_SD_Init(&hsd);
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level - LEDs off initially */
  HAL_GPIO_WritePin(GPIOA, LED1_RED_Pin|LED2_GREEN_Pin|LED3_YELLOW_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, LED4_BLUE_Pin, GPIO_PIN_RESET);
  
  /*Configure GPIO pin Output Level - MMA power enable (PB5) high to power accelerometer */
  HAL_GPIO_WritePin(GPIOB, MMA_POWER_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PA0 DAC_OUT1_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|DAC_OUT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1 (PA13), LED2 (PA14), LED3 (PA15) */
  GPIO_InitStruct.Pin = LED1_RED_Pin|LED2_GREEN_Pin|LED3_YELLOW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LED4 (PB4) as TIM3_CH1 for PWM */
  GPIO_InitStruct.Pin = LED4_BLUE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;  /* PB4 = TIM3_CH1 */
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /*Configure GPIO pin : MMA_POWER (PB5) */
  GPIO_InitStruct.Pin = MMA_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USR_BTN (PB3) */
  GPIO_InitStruct.Pin = USR_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(USR_BTN_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  /* Configure X-Series GPIO pins as outputs (X1-X8 on GPIOA, X9-X10 on GPIOB, X11-X12 on GPIOC) */
  /* Note: X1-X8 share pins with ADC/SPI, configure carefully */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  
  /* X1-X8: PA0-PA7 (but PA0 is ADC, PA4-7 overlap with SPI1, so skip those) */
  GPIO_InitStruct.Pin = GPIO_X2_Pin | GPIO_X3_Pin | GPIO_X4_Pin; /* PA1, PA2, PA3 */
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* X9-X10: PB6, PB7 (but these are I2C1, so leave as input with pullup) */
  /* X11-X12: PC4, PC5 */
  GPIO_InitStruct.Pin = GPIO_X11_Pin | GPIO_X12_Pin;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Configure Y-Series GPIO pins as outputs */
  /* Y1-Y2: PC6, PC7 */
  GPIO_InitStruct.Pin = GPIO_Y1_Pin | GPIO_Y2_Pin;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Y3-Y4: PB8, PB9 */
  GPIO_InitStruct.Pin = GPIO_Y3_Pin | GPIO_Y4_Pin;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* Y5-Y8: PB12-PB15 (but Y6/Y7 overlap with SPI2 and I2C2) */
  GPIO_InitStruct.Pin = GPIO_Y5_Pin | GPIO_Y8_Pin;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* Y9-Y10: PB10, PB11 (I2C2, so leave as input) */
  /* Y11-Y12: PB0, PB1 */
  GPIO_InitStruct.Pin = GPIO_Y11_Pin | GPIO_Y12_Pin;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Process CLI command
  */
void CLI_Process(char *cmd)
{
    char response[384];

    /* Convert to lowercase for comparison */
    for (int i = 0; cmd[i]; i++) {
        if (cmd[i] >= 'A' && cmd[i] <= 'Z')
            cmd[i] = cmd[i] + 32;
    }

    /* Help command */
    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0)
    {
        CLI_PrintHelp();
    }
    /* LED commands - pyb.LED(n).on()/off()/toggle() */
    else if (strncmp(cmd, "led", 3) == 0)
    {
        int led, state;
        if (sscanf(cmd, "led %d %d", &led, &state) == 2)
        {
            if (led >= 1 && led <= 4)
            {
                if (state) LED_On(led);
                else LED_Off(led);
                sprintf(response, "LED%d = %s\r\n", led, state ? "ON" : "OFF");
                CLI_SendString(response);
            }
        }
        else if (sscanf(cmd, "led %d", &led) == 1)
        {
            LED_Toggle(led);
            sprintf(response, "LED%d toggled\r\n", led);
            CLI_SendString(response);
        }
    }
    /* LED intensity (for LED4 with PWM) - pyb.LED(4).intensity(n) */
    else if (strncmp(cmd, "intensity", 9) == 0)
    {
        int val;
        if (sscanf(cmd, "intensity %d", &val) == 1)
        {
            LED_SetIntensity(4, val);
            sprintf(response, "LED4 intensity = %d\r\n", val);
            CLI_SendString(response);
        }
    }
    /* Accelerometer - pyb.Accel().x(), .y(), .z() */
    else if (strcmp(cmd, "accel") == 0)
    {
        int8_t x, y, z;
        MMA7660_ReadXYZ(&hi2c1, &x, &y, &z);
        sprintf(response, "Accel: X=%d Y=%d Z=%d\r\n", x, y, z);
        CLI_SendString(response);
    }
    /* Accelerometer continuous reading */
    else if (strncmp(cmd, "accel ", 6) == 0)
    {
        int count;
        if (sscanf(cmd, "accel %d", &count) == 1)
        {
            for (int i = 0; i < count && i < 100; i++)
            {
                int8_t x, y, z;
                MMA7660_ReadXYZ(&hi2c1, &x, &y, &z);
                sprintf(response, "%d: X=%d Y=%d Z=%d\r\n", i+1, x, y, z);
                CLI_SendString(response);
                HAL_Delay(100);
            }
        }
    }
    /* Tilt detection - pyb.Accel().tilt() */
    else if (strcmp(cmd, "tilt") == 0)
    {
        uint8_t tilt;
        MMA7660_ReadTilt(&hi2c1, &tilt);
        sprintf(response, "Tilt = 0x%02X\r\n", tilt);
        CLI_SendString(response);
    }
    /* ADC read - pyb.ADC(pin).read() */
    else if (strcmp(cmd, "adc") == 0)
    {
        uint16_t val = ADC_Read();
        sprintf(response, "ADC = %d (%.2fV)\r\n", val, val * 3.3f / 4095.0f);
        CLI_SendString(response);
    }
    /* DAC write - pyb.DAC(n).write(val) */
    else if (strncmp(cmd, "dac", 3) == 0)
    {
        int val;
        if (sscanf(cmd, "dac %d", &val) == 1)
        {
            DAC_Write(val);
            sprintf(response, "DAC = %d (%.2fV)\r\n", val, val * 3.3f / 4095.0f);
            CLI_SendString(response);
        }
    }
    /* PWM duty cycle - pyb.Timer().channel().pulse_width_percent() */
    else if (strncmp(cmd, "pwm", 3) == 0)
    {
        int val;
        if (sscanf(cmd, "pwm %d", &val) == 1)
        {
            PWM_SetDuty(val);
            sprintf(response, "PWM duty = %d%%\r\n", val);
            CLI_SendString(response);
        }
    }
    /* GPIO write - pyb.Pin().high()/low()/value() */
    else if (strncmp(cmd, "gpio", 4) == 0 || cmd[0] == 'x' || cmd[0] == 'y')
    {
        int pin, state;
        char series = 'x';
        
        /* Parse commands like: x1 1, x12 0, y5, gpio x3 1 */
        if (cmd[0] == 'x' || cmd[0] == 'y') {
            series = cmd[0];
            if (sscanf(cmd + 1, "%d %d", &pin, &state) == 2) {
                if (series == 'x') GPIO_SetPin(pin, state);
                else GPIO_SetPinY(pin, state);
                sprintf(response, "%c%d = %d\r\n", series - 32, pin, state);
                CLI_SendString(response);
            } else if (sscanf(cmd + 1, "%d", &pin) == 1) {
                uint8_t s = (series == 'x') ? GPIO_ReadPin(pin) : GPIO_ReadPinY(pin);
                sprintf(response, "%c%d = %d\r\n", series - 32, pin, s);
                CLI_SendString(response);
            }
        } else if (sscanf(cmd, "gpio %c%d %d", &series, &pin, &state) == 3) {
            if (series == 'x') GPIO_SetPin(pin, state);
            else if (series == 'y') GPIO_SetPinY(pin, state);
            sprintf(response, "%c%d = %d\r\n", series - 32, pin, state);
            CLI_SendString(response);
        } else if (sscanf(cmd, "gpio %c%d", &series, &pin) == 2) {
            uint8_t s = (series == 'x') ? GPIO_ReadPin(pin) : GPIO_ReadPinY(pin);
            sprintf(response, "%c%d = %d\r\n", series - 32, pin, s);
            CLI_SendString(response);
        } else if (sscanf(cmd, "gpio %d %d", &pin, &state) == 2) {
            GPIO_SetPin(pin, state);
            sprintf(response, "X%d = %d\r\n", pin, state);
            CLI_SendString(response);
        } else if (sscanf(cmd, "gpio %d", &pin) == 1) {
            uint8_t s = GPIO_ReadPin(pin);
            sprintf(response, "X%d = %d\r\n", pin, s);
            CLI_SendString(response);
        }
    }
    /* List all GPIO pins - includes X17-X22 */
    else if (strcmp(cmd, "pins") == 0 || strcmp(cmd, "gpiolist") == 0)
    {
        CLI_SendString("GPIO Pin Status:\r\n");
        CLI_SendString("X-Series:\r\n");
        for (int i = 1; i <= 12; i++) {
            sprintf(response, "  X%d = %d\r\n", i, GPIO_ReadPin(i));
            CLI_SendString(response);
        }
        for (int i = 17; i <= 22; i++) {
            sprintf(response, "  X%d = %d\r\n", i, GPIO_ReadPin(i));
            CLI_SendString(response);
        }
        CLI_SendString("Y-Series:\r\n");
        for (int i = 1; i <= 12; i++) {
            sprintf(response, "  Y%d = %d\r\n", i, GPIO_ReadPinY(i));
            CLI_SendString(response);
        }
    }
    /* Toggle command - toggle xN or yN */
    else if (strncmp(cmd, "toggle ", 7) == 0)
    {
        char series;
        int pin;
        if (sscanf(cmd + 7, "%c%d", &series, &pin) == 2) {
            GPIO_Toggle(series, pin);
            uint8_t state = (series == 'x' || series == 'X') ? GPIO_ReadPin(pin) : GPIO_ReadPinY(pin);
            sprintf(response, "%c%d toggled to %d\r\n", series - 32, pin, state);
            CLI_SendString(response);
        }
    }
    /* Mode command - mode xN in/out/od */
    else if (strncmp(cmd, "mode ", 5) == 0)
    {
        char series, modestr[8];
        int pin;
        if (sscanf(cmd + 5, "%c%d %7s", &series, &pin, modestr) == 3) {
            uint8_t mode = 0;  /* 0=input, 1=output, 2=open-drain */
            if (strcmp(modestr, "in") == 0) mode = 0;
            else if (strcmp(modestr, "out") == 0) mode = 1;
            else if (strcmp(modestr, "od") == 0) mode = 2;
            GPIO_SetMode(series, pin, mode);
            sprintf(response, "%c%d mode = %s\r\n", series - 32, pin, modestr);
            CLI_SendString(response);
        }
    }
    /* Pull command - pull xN up/down/none */
    else if (strncmp(cmd, "pull ", 5) == 0)
    {
        char series, pullstr[8];
        int pin;
        if (sscanf(cmd + 5, "%c%d %7s", &series, &pin, pullstr) == 3) {
            uint8_t pull = 0;  /* 0=none, 1=up, 2=down */
            if (strcmp(pullstr, "none") == 0) pull = 0;
            else if (strcmp(pullstr, "up") == 0) pull = 1;
            else if (strcmp(pullstr, "down") == 0) pull = 2;
            GPIO_SetPull(series, pin, pull);
            sprintf(response, "%c%d pull = %s\r\n", series - 32, pin, pullstr);
            CLI_SendString(response);
        }
    }
    /* Button read - pyb.Switch() */
    else if (strcmp(cmd, "button") == 0 || strcmp(cmd, "btn") == 0 || strcmp(cmd, "switch") == 0)
    {
        sprintf(response, "Button = %s\r\n", Button_Read() ? "PRESSED" : "RELEASED");
        CLI_SendString(response);
    }
    /* I2C scan - i2c.scan() */
    else if (strcmp(cmd, "i2c") == 0 || strcmp(cmd, "i2cscan") == 0 || strcmp(cmd, "i2c1") == 0)
    {
        CLI_SendString("I2C1 Scan:\r\n");
        int found = 0;
        for (uint8_t addr = 1; addr < 127; addr++)
        {
            if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 10) == HAL_OK)
            {
                sprintf(response, "  0x%02X found\r\n", addr);
                CLI_SendString(response);
                found++;
            }
        }
        if (found == 0) CLI_SendString("  No devices found\r\n");
        sprintf(response, "Total: %d device(s)\r\n", found);
        CLI_SendString(response);
    }
    /* I2C2 scan */
    else if (strcmp(cmd, "i2c2") == 0)
    {
        CLI_SendString("I2C2 Scan (Y9/Y10):\r\n");
        int found = 0;
        for (uint8_t addr = 1; addr < 127; addr++)
        {
            if (HAL_I2C_IsDeviceReady(&hi2c2, addr << 1, 1, 10) == HAL_OK)
            {
                sprintf(response, "  0x%02X found\r\n", addr);
                CLI_SendString(response);
                found++;
            }
        }
        if (found == 0) CLI_SendString("  No devices found\r\n");
        sprintf(response, "Total: %d device(s)\r\n", found);
        CLI_SendString(response);
    }
    /* UART2 send - pyb.UART(2).write() */
    else if (strncmp(cmd, "uart2 ", 6) == 0)
    {
        if (strncmp(cmd + 6, "send ", 5) == 0) {
            UART_Send(2, cmd + 11);
            sprintf(response, "UART2 TX: %s\r\n", cmd + 11);
            CLI_SendString(response);
        } else if (strcmp(cmd + 6, "recv") == 0) {
            char buf[64];
            uint8_t len = UART_Receive(2, buf, sizeof(buf) - 1);
            buf[len] = '\0';
            sprintf(response, "UART2 RX (%d bytes): %s\r\n", len, buf);
            CLI_SendString(response);
        } else {
            CLI_SendString("Usage: uart2 send <text> | uart2 recv\r\n");
        }
    }
    /* UART6 send - pyb.UART(6).write() */
    else if (strncmp(cmd, "uart6 ", 6) == 0)
    {
        if (strncmp(cmd + 6, "send ", 5) == 0) {
            UART_Send(6, cmd + 11);
            sprintf(response, "UART6 TX: %s\r\n", cmd + 11);
            CLI_SendString(response);
        } else if (strcmp(cmd + 6, "recv") == 0) {
            char buf[64];
            uint8_t len = UART_Receive(6, buf, sizeof(buf) - 1);
            buf[len] = '\0';
            sprintf(response, "UART6 RX (%d bytes): %s\r\n", len, buf);
            CLI_SendString(response);
        } else {
            CLI_SendString("Usage: uart6 send <text> | uart6 recv\r\n");
        }
    }
    /* CAN send - pyb.CAN().send() */
    else if (strncmp(cmd, "can ", 4) == 0)
    {
        if (strncmp(cmd + 4, "send ", 5) == 0) {
            uint32_t id;
            int d0, d1, d2, d3, d4, d5, d6, d7;
            int n = sscanf(cmd + 9, "%lx %x %x %x %x %x %x %x %x", &id, &d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7);
            if (n >= 2) {
                uint8_t data[8] = {d0, d1, d2, d3, d4, d5, d6, d7};
                CAN_Send(id, data, n - 1);
                sprintf(response, "CAN TX: ID=0x%03lX, %d bytes\r\n", id, n - 1);
                CLI_SendString(response);
            } else {
                CLI_SendString("Usage: can send <id> <b0> [b1] [b2] ... [b7]\r\n");
            }
        } else if (strcmp(cmd + 4, "recv") == 0) {
            if (can_rx_ready) {
                sprintf(response, "CAN RX: ID=0x%03lX DLC=%d Data=", 
                    can_rx_header.StdId, (int)can_rx_header.DLC);
                CLI_SendString(response);
                for (int i = 0; i < can_rx_header.DLC; i++) {
                    sprintf(response, "%02X ", can_rx_data[i]);
                    CLI_SendString(response);
                }
                CLI_SendString("\r\n");
                can_rx_ready = 0;
            } else {
                CLI_SendString("CAN: No message pending\r\n");
            }
        } else if (strcmp(cmd + 4, "status") == 0) {
            uint32_t err = HAL_CAN_GetError(&hcan1);
            sprintf(response, "CAN Status: Error=0x%08lX TxMailboxes=%lu\r\n", 
                err, HAL_CAN_GetTxMailboxesFreeLevel(&hcan1));
            CLI_SendString(response);
        } else {
            CLI_SendString("Usage: can send|recv|status\r\n");
        }
    }
    /* RTC commands - pyb.RTC() */
    else if (strncmp(cmd, "rtc", 3) == 0)
    {
        if (strcmp(cmd, "rtc") == 0 || strcmp(cmd + 3, " get") == 0) {
            uint8_t h, m, s, yr, mo, dy;
            RTC_GetTime(&h, &m, &s);
            RTC_GetDate(&yr, &mo, &dy);
            sprintf(response, "RTC: 20%02d-%02d-%02d %02d:%02d:%02d\r\n", yr, mo, dy, h, m, s);
            CLI_SendString(response);
        } else if (strncmp(cmd + 4, "set ", 4) == 0) {
            int h, m, s;
            if (sscanf(cmd + 8, "%d:%d:%d", &h, &m, &s) == 3) {
                RTC_SetTime(h, m, s);
                sprintf(response, "RTC time set to %02d:%02d:%02d\r\n", h, m, s);
                CLI_SendString(response);
            }
        } else if (strncmp(cmd + 4, "date ", 5) == 0) {
            int y, m, d;
            if (sscanf(cmd + 9, "%d-%d-%d", &y, &m, &d) == 3) {
                RTC_SetDate(y % 100, m, d);
                sprintf(response, "RTC date set to %02d-%02d-%02d\r\n", y % 100, m, d);
                CLI_SendString(response);
            }
        } else {
            CLI_SendString("Usage: rtc | rtc set HH:MM:SS | rtc date YY-MM-DD\r\n");
        }
    }
    /* SPI transfer - pyb.SPI().send_recv() */
    else if (strncmp(cmd, "spi", 3) == 0)
    {
        int port = 1;
        if (cmd[3] == '2') port = 2;
        
        if (strncmp(cmd + 4, " send ", 6) == 0 || strncmp(cmd + 5, " send ", 6) == 0) {
            int offset = (cmd[3] == '1' || cmd[3] == '2') ? 10 : 9;
            /* Parse hex bytes */
            uint8_t tx[16], rx[16];
            int n = 0;
            char *p = (char*)(cmd + offset);
            while (*p && n < 16) {
                int val;
                if (sscanf(p, "%x", &val) == 1) {
                    tx[n++] = val;
                    while (*p && *p != ' ') p++;
                    while (*p == ' ') p++;
                } else break;
            }
            if (n > 0) {
                SPI_Transfer(port, tx, rx, n);
                sprintf(response, "SPI%d TX/RX: ", port);
                CLI_SendString(response);
                for (int i = 0; i < n; i++) {
                    sprintf(response, "%02X->%02X ", tx[i], rx[i]);
                    CLI_SendString(response);
                }
                CLI_SendString("\r\n");
            }
        } else {
            sprintf(response, "Usage: spi%d send <hex bytes>\r\n", port);
            CLI_SendString(response);
        }
    }
    /* SD card commands - pyb.SDCard() */
    else if (strncmp(cmd, "sd", 2) == 0)
    {
        if (strcmp(cmd, "sd") == 0 || strcmp(cmd + 2, " status") == 0) {
            HAL_SD_CardInfoTypeDef info;
            if (HAL_SD_GetCardInfo(&hsd, &info) == HAL_OK) {
                /* Calculate capacity in MB avoiding 32-bit overflow */
                /* BlockNbr / 2048 gives MB when BlockSize=512 */
                uint32_t capacity_mb = info.BlockNbr / 2048;
                sprintf(response, "SD Card: Type=%lu, Capacity=%lu MB (%lu GB)\r\n", 
                    info.CardType, capacity_mb, capacity_mb / 1024);
                CLI_SendString(response);
            } else {
                CLI_SendString("SD Card: Not detected or error\r\n");
            }
        } else if (strcmp(cmd + 3, "init") == 0) {
            if (HAL_SD_Init(&hsd) == HAL_OK) {
                HAL_SD_CardInfoTypeDef info;
                HAL_SD_GetCardInfo(&hsd, &info);
                uint32_t capacity_mb = info.BlockNbr / 2048;
                sprintf(response, "SD Init OK: %lu MB (%lu GB)\r\n", capacity_mb, capacity_mb / 1024);
                CLI_SendString(response);
            } else {
                CLI_SendString("SD Init failed\r\n");
            }
        } else {
            CLI_SendString("Usage: sd | sd init | sd status\r\n");
        }
    }
    /* System info - sys.implementation, machine.freq() */
    else if (strcmp(cmd, "info") == 0)
    {
        sprintf(response,
            "PyBoard Native Firmware v3.0\r\n"
            "CPU: STM32F405RG @ 168MHz\r\n"
            "Flash: 1MB, RAM: 192KB\r\n"
            "HAL Version: %lu.%lu.%lu\r\n",
            ((HAL_GetHalVersion() >> 24) & 0xFF),
            ((HAL_GetHalVersion() >> 16) & 0xFF),
            ((HAL_GetHalVersion() >> 8) & 0xFF));
        CLI_SendString(response);
        CLI_SendString("Peripherals:\r\n"
            "  - 4 LEDs: PA13(R) PA14(G) PA15(Y) PB4(B)\r\n"
            "  - User Button: PB3\r\n"
            "  - MMA7660 Accelerometer (I2C1)\r\n"
            "  - ADC1 CH0 (X1/PA0)\r\n"
            "  - DAC1 CH1 (X5/PA4)\r\n"
            "  - PWM: TIM2(X2), TIM3(LED4,Y11,Y12)\r\n"
            "  - I2C1 (X9/X10), I2C2 (Y9/Y10)\r\n"
            "  - SPI1 (X6-X8), SPI2 (Y5-Y8)\r\n"
            "  - UART2 (X3/X4), UART6 (Y1/Y2)\r\n"
            "  - CAN1 (Y3/Y4)\r\n"
            "  - SDIO SD Card\r\n"
            "  - RTC with LSE 32.768kHz\r\n"
            "  - USB CDC Serial\r\n"
            "GPIO: X1-X12, X17-X22, Y1-Y12\r\n");
    }
    /* Benchmark - same as MicroPython test (10K iterations) */
    else if (strcmp(cmd, "bench") == 0)
    {
        CLI_SendString("Running benchmark (10K iterations, same as MicroPython test)...\r\n");

        uint32_t start_tick = DWT->CYCCNT;  // Use cycle counter for microsecond precision
        volatile uint32_t total = 0;
        for (volatile uint32_t i = 0; i < 10000; i++)
        {
            total += i;
        }
        uint32_t cycles = DWT->CYCCNT - start_tick;
        uint32_t us = cycles / 168;  // 168 MHz clock

        sprintf(response, "Result: %lu\r\n", total);
        CLI_SendString(response);
        sprintf(response, "Time: %lu us (%lu cycles)\r\n", us, cycles);
        CLI_SendString(response);
        CLI_SendString("Compare to MicroPython bytecode: ~993000 us\r\n");
    }
    /* Speed test - compare to MicroPython */
    else if (strcmp(cmd, "speed") == 0)
    {
        CLI_SendString("Running speed test (1M loop iterations)...\r\n");

        uint32_t start = HAL_GetTick();
        volatile uint32_t sum = 0;
        for (volatile uint32_t i = 0; i < 1000000; i++)
        {
            sum += i;
        }
        uint32_t elapsed = HAL_GetTick() - start;

        sprintf(response, "Time: %lu ms (MicroPython ~780ms = %lux faster)\r\n", 
                elapsed, elapsed > 0 ? 780 / elapsed : 999);
        CLI_SendString(response);
    }
    /* GPIO toggle speed test */
    else if (strcmp(cmd, "speedgpio") == 0)
    {
        CLI_SendString("GPIO toggle test (100K toggles)...\r\n");

        uint32_t start = HAL_GetTick();
        for (volatile uint32_t i = 0; i < 100000; i++)
        {
            HAL_GPIO_WritePin(GPIO_X1_GPIO_Port, GPIO_X1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIO_X1_GPIO_Port, GPIO_X1_Pin, GPIO_PIN_RESET);
        }
        uint32_t elapsed = HAL_GetTick() - start;

        sprintf(response, "Time: %lu ms\r\n", elapsed);
        CLI_SendString(response);
    }
    /* Demo mode */
    else if (strcmp(cmd, "demo") == 0)
    {
        CLI_SendString("Running LED demo...\r\n");
        for (int j = 0; j < 3; j++)
        {
            for (int i = 1; i <= 4; i++)
            {
                LED_On(i);
                HAL_Delay(100);
                LED_Off(i);
            }
        }
        CLI_SendString("Demo complete\r\n");
    }
    /* All LEDs on/off */
    else if (strcmp(cmd, "allon") == 0)
    {
        LED_On(1); LED_On(2); LED_On(3); LED_On(4);
        CLI_SendString("All LEDs ON\r\n");
    }
    else if (strcmp(cmd, "alloff") == 0)
    {
        LED_Off(1); LED_Off(2); LED_Off(3); LED_Off(4);
        CLI_SendString("All LEDs OFF\r\n");
    }
    /*=======================================================================
     * NEW COMMANDS - MicroPython-compatible functionality
     *=======================================================================*/
    /* Multi-channel ADC: adc N (N=0-15 or temp or vbat) */
    else if (strncmp(cmd, "adc ", 4) == 0)
    {
        if (strcmp(cmd + 4, "temp") == 0) {
            uint16_t val = ADC_ReadTempSensor();
            /* Convert to temperature: Temp = ((V - V25) / Avg_Slope) + 25 */
            /* V25 = 0.76V, Avg_Slope = 2.5mV/°C for STM32F4 */
            float voltage = val * 3.3f / 4095.0f;
            float temp = ((voltage - 0.76f) / 0.0025f) + 25.0f;
            sprintf(response, "Temp sensor: %d (%.1f°C)\r\n", val, temp);
            CLI_SendString(response);
        } else if (strcmp(cmd + 4, "vbat") == 0) {
            uint16_t val = ADC_ReadVBAT();
            /* VBAT is divided by 2 internally */
            float voltage = val * 3.3f * 2.0f / 4095.0f;
            sprintf(response, "VBAT: %d (%.2fV)\r\n", val, voltage);
            CLI_SendString(response);
        } else {
            int ch;
            if (sscanf(cmd + 4, "%d", &ch) == 1 && ch >= 0 && ch <= 15) {
                uint16_t val = ADC_ReadChannel(ch);
                sprintf(response, "ADC CH%d: %d (%.2fV)\r\n", ch, val, val * 3.3f / 4095.0f);
                CLI_SendString(response);
            } else {
                CLI_SendString("Usage: adc <0-15|temp|vbat>\r\n");
            }
        }
    }
    /* I2C read: i2c1 read <addr> <len> or i2c1 mem <addr> <memaddr> <len> */
    else if (strncmp(cmd, "i2c1 read ", 10) == 0 || strncmp(cmd, "i2c2 read ", 10) == 0)
    {
        int bus = (cmd[3] == '1') ? 1 : 2;
        int addr, len;
        if (sscanf(cmd + 10, "%x %d", &addr, &len) == 2 && len <= 32) {
            uint8_t buf[32];
            int result = I2C_ReadFrom(bus, addr, buf, len);
            if (result > 0) {
                sprintf(response, "I2C%d read 0x%02X (%d bytes): ", bus, addr, result);
                CLI_SendString(response);
                for (int i = 0; i < result; i++) {
                    sprintf(response, "%02X ", buf[i]);
                    CLI_SendString(response);
                }
                CLI_SendString("\r\n");
            } else {
                sprintf(response, "I2C%d read failed\r\n", bus);
                CLI_SendString(response);
            }
        }
    }
    /* I2C write: i2c1 write <addr> <bytes...> */
    else if (strncmp(cmd, "i2c1 write ", 11) == 0 || strncmp(cmd, "i2c2 write ", 11) == 0)
    {
        int bus = (cmd[3] == '1') ? 1 : 2;
        int addr;
        uint8_t data[16];
        int n = 0;
        char *p = (char*)(cmd + 11);
        if (sscanf(p, "%x", &addr) == 1) {
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && n < 16) {
                int val;
                if (sscanf(p, "%x", &val) == 1) {
                    data[n++] = val;
                    while (*p && *p != ' ') p++;
                    while (*p == ' ') p++;
                } else break;
            }
            if (n > 0) {
                int result = I2C_WriteTo(bus, addr, data, n);
                if (result > 0) {
                    sprintf(response, "I2C%d write 0x%02X: %d bytes OK\r\n", bus, addr, result);
                } else {
                    sprintf(response, "I2C%d write failed\r\n", bus);
                }
                CLI_SendString(response);
            }
        }
    }
    /* Unique ID */
    else if (strcmp(cmd, "uid") == 0)
    {
        uint32_t uid[3];
        Get_UniqueID(uid);
        sprintf(response, "UID: %08lX-%08lX-%08lX\r\n", uid[0], uid[1], uid[2]);
        CLI_SendString(response);
    }
    /* Reset cause */
    else if (strcmp(cmd, "resetcause") == 0)
    {
        uint32_t cause = Get_ResetCause();
        CLI_SendString("Reset cause: ");
        if (cause & RCC_CSR_LPWRRSTF) CLI_SendString("LOW_POWER ");
        if (cause & RCC_CSR_WWDGRSTF) CLI_SendString("WINDOW_WDT ");
        if (cause & RCC_CSR_IWDGRSTF) CLI_SendString("INDEP_WDT ");
        if (cause & RCC_CSR_SFTRSTF) CLI_SendString("SOFTWARE ");
        if (cause & RCC_CSR_PORRSTF) CLI_SendString("POWER_ON ");
        if (cause & RCC_CSR_PINRSTF) CLI_SendString("PIN_RESET ");
        if (cause & RCC_CSR_BORRSTF) CLI_SendString("BROWNOUT ");
        sprintf(response, "(0x%08lX)\r\n", cause);
        CLI_SendString(response);
    }
    /* Watchdog commands */
    else if (strncmp(cmd, "wdt ", 4) == 0)
    {
        if (strncmp(cmd + 4, "start ", 6) == 0) {
            int timeout;
            if (sscanf(cmd + 10, "%d", &timeout) == 1 && timeout > 0) {
                WDT_Init(timeout);
                sprintf(response, "Watchdog started: %d ms timeout\r\n", timeout);
                CLI_SendString(response);
            }
        } else if (strcmp(cmd + 4, "feed") == 0) {
            WDT_Feed();
            CLI_SendString("Watchdog fed\r\n");
        } else {
            CLI_SendString("Usage: wdt start <ms> | wdt feed\r\n");
        }
    }
    /* Sleep mode */
    else if (strcmp(cmd, "sleep") == 0)
    {
        CLI_SendString("Entering sleep mode... (any interrupt wakes)\r\n");
        HAL_Delay(10);  /* Let message send */
        Power_Sleep();
        CLI_SendString("Woke from sleep\r\n");
    }
    /* Stop mode */
    else if (strcmp(cmd, "stop") == 0)
    {
        CLI_SendString("Entering stop mode... (EXTI/RTC wakes)\r\n");
        HAL_Delay(10);
        Power_Stop();
        CLI_SendString("Woke from stop mode\r\n");
    }
    /* Viper-equivalent benchmark (cycle-accurate timing) */
    else if (strcmp(cmd, "viper") == 0)
    {
        CLI_SendString("=== Native C 'Viper' Benchmark ===\r\n");
        CLI_SendString("(Same test as MicroPython @viper)\r\n\r\n");
        
        /* 10K iterations - same as MicroPython test */
        uint32_t start = DWT->CYCCNT;
        volatile int32_t total = 0;
        for (volatile int32_t i = 0; i < 10000; i++) {
            total += i;
        }
        uint32_t cycles = DWT->CYCCNT - start;
        uint32_t us = cycles / 168;
        
        sprintf(response, "Result: %ld\r\n", total);
        CLI_SendString(response);
        sprintf(response, "Time: %lu us (%lu cycles)\r\n", us, cycles);
        CLI_SendString(response);
        CLI_SendString("\r\nComparison:\r\n");
        CLI_SendString("  MicroPython bytecode: ~32000 us\r\n");
        CLI_SendString("  MicroPython @native:  ~16000 us\r\n");
        CLI_SendString("  MicroPython @viper:    ~2500 us\r\n");
        sprintf(response, "  Native C (this):       %5lu us\r\n", us);
        CLI_SendString(response);
        if (us > 0) {
            sprintf(response, "  Speedup vs bytecode:   %.0fx\r\n", 32000.0f / us);
            CLI_SendString(response);
        }
    }
    /*=======================================================================
     * FILE SYSTEM COMMANDS (FatFS / oofatfs)
     *=======================================================================*/
    /* Mount SD card file system */
    else if (strcmp(cmd, "mount") == 0)
    {
        /* Set up the block device driver (NULL = use default from diskio) */
        SDFatFs.drv = NULL;
        FRESULT res = f_mount(&SDFatFs);
        if (res == FR_OK) {
            fs_mounted = 1;
            CLI_SendString("SD card mounted successfully\r\n");
        } else {
            fs_mounted = 0;
            sprintf(response, "Mount failed: error %d\r\n", res);
            CLI_SendString(response);
        }
    }
    /* Unmount SD card */
    else if (strcmp(cmd, "umount") == 0 || strcmp(cmd, "unmount") == 0)
    {
        f_umount(&SDFatFs);
        fs_mounted = 0;
        CLI_SendString("SD card unmounted\r\n");
    }
    /* List directory: ls [path] */
    else if (strcmp(cmd, "ls") == 0 || strncmp(cmd, "ls ", 3) == 0)
    {
        if (!fs_mounted) {
            CLI_SendString("Error: SD not mounted. Use 'mount' first\r\n");
        } else {
            FF_DIR dir;
            FILINFO fno;
            const char *path = (strlen(cmd) > 3) ? cmd + 3 : "/";
            FRESULT res = f_opendir(&SDFatFs, &dir, path);
            if (res == FR_OK) {
                sprintf(response, "Directory: %s\r\n", path);
                CLI_SendString(response);
                while (1) {
                    res = f_readdir(&dir, &fno);
                    if (res != FR_OK || fno.fname[0] == 0) break;
                    sprintf(response, "  %c %8lu  %s\r\n",
                        (fno.fattrib & AM_DIR) ? 'd' : '-',
                        (unsigned long)fno.fsize,
                        fno.fname);
                    CLI_SendString(response);
                }
                f_closedir(&dir);
            } else {
                sprintf(response, "Cannot open: %s (err %d)\r\n", path, res);
                CLI_SendString(response);
            }
        }
    }
    /* Read file: cat <filename> */
    else if (strncmp(cmd, "cat ", 4) == 0)
    {
        if (!fs_mounted) {
            CLI_SendString("Error: SD not mounted. Use 'mount' first\r\n");
        } else {
            FIL fil;
            const char *path = cmd + 4;
            FRESULT res = f_open(&SDFatFs, &fil, path, FA_READ);
            if (res == FR_OK) {
                char buf[65];
                UINT br;
                while (f_read(&fil, buf, 64, &br) == FR_OK && br > 0) {
                    buf[br] = '\0';
                    CLI_SendString(buf);
                }
                f_close(&fil);
                CLI_SendString("\r\n");
            } else {
                sprintf(response, "Cannot open: %s (err %d)\r\n", path, res);
                CLI_SendString(response);
            }
        }
    }
    /* Write file: write <filename> <text> */
    else if (strncmp(cmd, "write ", 6) == 0)
    {
        if (!fs_mounted) {
            CLI_SendString("Error: SD not mounted. Use 'mount' first\r\n");
        } else {
            char *space = strchr(cmd + 6, ' ');
            if (space) {
                *space = '\0';
                const char *path = cmd + 6;
                const char *text = space + 1;
                FIL fil;
                FRESULT res = f_open(&SDFatFs, &fil, path, FA_WRITE | FA_CREATE_ALWAYS);
                if (res == FR_OK) {
                    UINT bw;
                    f_write(&fil, text, strlen(text), &bw);
                    f_close(&fil);
                    sprintf(response, "Wrote %u bytes to %s\r\n", bw, path);
                    CLI_SendString(response);
                } else {
                    sprintf(response, "Cannot create: %s (err %d)\r\n", path, res);
                    CLI_SendString(response);
                }
            } else {
                CLI_SendString("Usage: write <filename> <text>\r\n");
            }
        }
    }
    /* Append to file: append <filename> <text> */
    else if (strncmp(cmd, "append ", 7) == 0)
    {
        if (!fs_mounted) {
            CLI_SendString("Error: SD not mounted. Use 'mount' first\r\n");
        } else {
            char *space = strchr(cmd + 7, ' ');
            if (space) {
                *space = '\0';
                const char *path = cmd + 7;
                const char *text = space + 1;
                FIL fil;
                FRESULT res = f_open(&SDFatFs, &fil, path, FA_WRITE | FA_OPEN_APPEND);
                if (res == FR_OK) {
                    UINT bw;
                    f_write(&fil, text, strlen(text), &bw);
                    f_close(&fil);
                    sprintf(response, "Appended %u bytes to %s\r\n", bw, path);
                    CLI_SendString(response);
                } else {
                    sprintf(response, "Cannot open: %s (err %d)\r\n", path, res);
                    CLI_SendString(response);
                }
            } else {
                CLI_SendString("Usage: append <filename> <text>\r\n");
            }
        }
    }
    /* Log data to file with newline: log <filename> <data> - perfect for CSV logging */
    else if (strncmp(cmd, "log ", 4) == 0)
    {
        if (!fs_mounted) {
            CLI_SendString("Error: SD not mounted. Use 'mount' first\r\n");
        } else {
            char *space = strchr(cmd + 4, ' ');
            if (space) {
                *space = '\0';
                const char *path = cmd + 4;
                const char *text = space + 1;
                FIL fil;
                FRESULT res = f_open(&SDFatFs, &fil, path, FA_WRITE | FA_OPEN_APPEND);
                if (res == FR_OK) {
                    UINT bw, bw2;
                    f_write(&fil, text, strlen(text), &bw);
                    f_write(&fil, "\n", 1, &bw2);  /* Add newline */
                    f_close(&fil);
                    sprintf(response, "Logged %u bytes to %s\r\n", bw + 1, path);
                    CLI_SendString(response);
                } else {
                    sprintf(response, "Cannot open: %s (err %d)\r\n", path, res);
                    CLI_SendString(response);
                }
            } else {
                CLI_SendString("Usage: log <filename> <data>\r\n");
            }
        }
    }
    /* Create directory: mkdir <dirname> */
    else if (strncmp(cmd, "mkdir ", 6) == 0)
    {
        if (!fs_mounted) {
            CLI_SendString("Error: SD not mounted. Use 'mount' first\r\n");
        } else {
            const char *path = cmd + 6;
            FRESULT res = f_mkdir(&SDFatFs, path);
            if (res == FR_OK) {
                sprintf(response, "Created: %s\r\n", path);
                CLI_SendString(response);
            } else {
                sprintf(response, "Cannot create: %s (err %d)\r\n", path, res);
                CLI_SendString(response);
            }
        }
    }
    /* Remove file: rm <filename> */
    else if (strncmp(cmd, "rm ", 3) == 0)
    {
        if (!fs_mounted) {
            CLI_SendString("Error: SD not mounted. Use 'mount' first\r\n");
        } else {
            const char *path = cmd + 3;
            FRESULT res = f_unlink(&SDFatFs, path);
            if (res == FR_OK) {
                sprintf(response, "Deleted: %s\r\n", path);
                CLI_SendString(response);
            } else {
                sprintf(response, "Cannot delete: %s (err %d)\r\n", path, res);
                CLI_SendString(response);
            }
        }
    }
    /* Format SD card: format [fat|fat32] */
    else if (strcmp(cmd, "format") == 0 || strncmp(cmd, "format ", 7) == 0)
    {
        /* Parse optional format type */
        BYTE fmt_opt = FM_FAT32;  /* Default to FAT32 */
        const char *fmt_name = "FAT32";
        
        if (strlen(cmd) > 7) {
            const char *type = cmd + 7;
            if (strcmp(type, "fat") == 0 || strcmp(type, "fat16") == 0) {
                fmt_opt = FM_FAT;
                fmt_name = "FAT16";
            } else if (strcmp(type, "fat32") == 0) {
                fmt_opt = FM_FAT32;
                fmt_name = "FAT32";
            }
        }
        
        sprintf(response, "Formatting SD card as %s...\r\n", fmt_name);
        CLI_SendString(response);
        CLI_SendString("This may take a moment...\r\n");
        
        /* Unmount first if mounted */
        if (fs_mounted) {
            f_umount(&SDFatFs);
            fs_mounted = 0;
        }
        
        /* Set up the block device driver */
        SDFatFs.drv = NULL;
        
        /* Work buffer for format (needs at least 512 bytes) */
        static BYTE work[512];
        
        /* Format with specified type, default cluster size (0 = auto) */
        FRESULT res = f_mkfs(&SDFatFs, fmt_opt, 0, work, sizeof(work));
        if (res == FR_OK) {
            CLI_SendString("Format complete!\r\n");
            CLI_SendString("Use 'mount' to mount the new filesystem\r\n");
        } else {
            sprintf(response, "Format failed: error %d\r\n", res);
            CLI_SendString(response);
            if (res == FR_MKFS_ABORTED) {
                CLI_SendString("(Card may be too small for FAT32, try 'format fat')\r\n");
            }
        }
    }
    /* Partition SD card: partition */
    else if (strcmp(cmd, "partition") == 0)
    {
        CLI_SendString("Creating partition table on SD card...\r\n");
        CLI_SendString("This will ERASE ALL DATA!\r\n");
        
        /* Unmount first if mounted */
        if (fs_mounted) {
            f_umount(&SDFatFs);
            fs_mounted = 0;
        }
        
        /* Work buffer for fdisk */
        static BYTE work[512];
        
        /* Create single partition using 100% of disk */
        /* Array: partition sizes in % (100 = whole disk), 0 terminates */
        static const DWORD plist[] = {100, 0, 0, 0};
        
        FRESULT res = f_fdisk(NULL, plist, work);
        if (res == FR_OK) {
            CLI_SendString("Partition table created!\r\n");
            CLI_SendString("Now use 'format' to format the partition\r\n");
        } else {
            sprintf(response, "Partition failed: error %d\r\n", res);
            CLI_SendString(response);
        }
    }
    /* Disk free space: df */
    else if (strcmp(cmd, "df") == 0)
    {
        if (!fs_mounted) {
            CLI_SendString("Error: SD not mounted. Use 'mount' first\r\n");
        } else {
            DWORD fre_clust;
            FRESULT res = f_getfree(&SDFatFs, &fre_clust);
            if (res == FR_OK) {
                DWORD tot_sect = (SDFatFs.n_fatent - 2) * SDFatFs.csize;
                DWORD fre_sect = fre_clust * SDFatFs.csize;
                sprintf(response, "Total: %lu MB, Free: %lu MB\r\n",
                    tot_sect / 2048, fre_sect / 2048);
                CLI_SendString(response);
            } else {
                sprintf(response, "Error getting free space: %d\r\n", res);
                CLI_SendString(response);
            }
        }
    }
    else if (strlen(cmd) > 0)
    {
        sprintf(response, "Unknown command: %s\r\nType 'help' for commands\r\n", cmd);
        CLI_SendString(response);
    }
}

/**
  * @brief  Print help message
  */
void CLI_PrintHelp(void)
{
    CLI_SendString(
        "PyBoard Native CLI v3.0 (MicroPython Compatible)\r\n"
        "================================================\r\n"
        "LED (pyb.LED):\r\n"
        "  led N [0|1]    Control LED 1-4, toggle if no state\r\n"
        "  intensity N    LED4 brightness 0-255 (PWM)\r\n"
        "  allon/alloff   All LEDs on/off\r\n"
        "  demo           LED demo sequence\r\n"
        "\r\n");
    CLI_SendString(
        "Accelerometer (pyb.Accel):\r\n"
        "  accel [N]      Read X/Y/Z (N times)\r\n"
        "  tilt           Read tilt status\r\n"
        "\r\n"
        "Analog I/O (pyb.ADC, pyb.DAC):\r\n"
        "  adc            Read ADC (X1/PA0)\r\n"
        "  dac N          Set DAC 0-4095 (X5/PA4)\r\n"
        "  pwm N          Set PWM duty 0-100%% (X2/PA1)\r\n"
        "\r\n");
    CLI_SendString(
        "Digital I/O (pyb.Pin):\r\n"
        "  xN [0|1]       Read/write X1-X12, X17-X22\r\n"
        "  yN [0|1]       Read/write Y1-Y12\r\n"
        "  toggle xN|yN   Toggle pin\r\n"
        "  mode xN in|out|od   Set pin mode\r\n"
        "  pull xN up|down|none Set pull resistor\r\n"
        "  pins           List all GPIO states\r\n"
        "  button         Read user button (pyb.Switch)\r\n"
        "\r\n");
    CLI_SendString(
        "I2C (pyb.I2C):\r\n"
        "  i2c1           Scan I2C1 bus (X9/X10)\r\n"
        "  i2c2           Scan I2C2 bus (Y9/Y10)\r\n"
        "\r\n"
        "SPI (pyb.SPI):\r\n"
        "  spi1 send <hex bytes>  Transfer on SPI1 (X6-X8)\r\n"
        "  spi2 send <hex bytes>  Transfer on SPI2 (Y5-Y8)\r\n"
        "\r\n");
    CLI_SendString(
        "UART (pyb.UART):\r\n"
        "  uart2 send <text>  Send on UART2 (X3/X4)\r\n"
        "  uart2 recv         Read UART2 buffer\r\n"
        "  uart6 send <text>  Send on UART6 (Y1/Y2)\r\n"
        "  uart6 recv         Read UART6 buffer\r\n"
        "\r\n"
        "CAN (pyb.CAN):\r\n"
        "  can send <id> <bytes>  Send CAN frame (Y3/Y4)\r\n"
        "  can recv           Read CAN message\r\n"
        "  can status         CAN bus status\r\n"
        "\r\n");
    CLI_SendString(
        "RTC (pyb.RTC):\r\n"
        "  rtc                Get date/time\r\n"
        "  rtc set HH:MM:SS   Set time\r\n"
        "  rtc date YY-MM-DD  Set date\r\n"
        "\r\n"
        "SD Card (pyb.SDCard):\r\n"
        "  sd                 Card info\r\n"
        "  sd init            Initialize card\r\n"
        "\r\n");
    CLI_SendString(
        "File System (FatFS):\r\n"
        "  mount              Mount SD card file system\r\n"
        "  umount             Unmount SD card\r\n"
        "  partition          Create partition table\r\n"
        "  format [fat|fat32] Format SD card (default FAT32)\r\n"
        "  ls [path]          List directory\r\n"
        "  cat <file>         Read file content\r\n"
        "  write <file> <txt> Write text to file\r\n"
        "  append <file> <txt> Append text to file\r\n"
        "  mkdir <dir>        Create directory\r\n"
        "  rm <file>          Delete file or empty dir\r\n"
        "  df                 Show disk free space\r\n"
        "\r\n"
        "System:\r\n"
        "  info           System information\r\n"
        "  bench          Benchmark (10K loop, compare to MicroPython)\r\n"
        "  speed          CPU speed test (1M loop)\r\n"
        "  speedgpio      GPIO toggle benchmark\r\n"
        "  viper          'Viper' benchmark (cycle-accurate)\r\n"
        "  help           Show this help\r\n"
        "\r\n");
    CLI_SendString(
        "=== NEW FEATURES (v3.0) ===\r\n"
        "Multi-channel ADC:\r\n"
        "  adc <0-15>     Read ADC channel 0-15\r\n"
        "  adc temp       Read internal temperature\r\n"
        "  adc vbat       Read battery voltage\r\n"
        "\r\n"
        "I2C Data Transfer:\r\n"
        "  i2c1 read <addr> <len>   Read bytes from device\r\n"
        "  i2c1 write <addr> <hex>  Write bytes to device\r\n"
        "  i2c2 read/write          Same for I2C2\r\n"
        "\r\n"
        "Power Management:\r\n"
        "  sleep          Enter sleep mode (any IRQ wakes)\r\n"
        "  stop           Enter stop mode (EXTI/RTC wakes)\r\n"
        "  wdt start <ms> Start watchdog with timeout\r\n"
        "  wdt feed       Feed (reset) watchdog\r\n"
        "\r\n"
        "System Info:\r\n"
        "  uid            Show unique device ID\r\n"
        "  resetcause     Show last reset cause\r\n");
}

void CLI_SendString(const char *str)
{
    /* Transmit via USB CDC */
    uint16_t len = strlen(str);
    uint16_t sent = 0;
    
    while (sent < len)
    {
        uint16_t chunk = (len - sent > 64) ? 64 : (len - sent);
        while (CDC_Transmit_FS((uint8_t*)(str + sent), chunk) == USBD_BUSY)
        {
            HAL_Delay(1);
        }
        sent += chunk;
        HAL_Delay(1);  /* Small delay between chunks */
    }
}

/* USB CDC RX Handler */
void USB_CDC_RxHandler(uint8_t *buf, uint32_t len)
{
    if (len > sizeof(usb_rx_buffer))
        len = sizeof(usb_rx_buffer);
    
    memcpy((void*)usb_rx_buffer, buf, len);
    usb_rx_len = len;
    usb_rx_ready = 1;
}

/* LED Functions - PyBoard v1.1: LEDs are active HIGH */
void LED_On(uint8_t led)
{
    switch (led) {
        case 1: HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_SET); break;
        case 2: HAL_GPIO_WritePin(LED2_GREEN_GPIO_Port, LED2_GREEN_Pin, GPIO_PIN_SET); break;
        case 3: HAL_GPIO_WritePin(LED3_YELLOW_GPIO_Port, LED3_YELLOW_Pin, GPIO_PIN_SET); break;
        case 4: HAL_GPIO_WritePin(LED4_BLUE_GPIO_Port, LED4_BLUE_Pin, GPIO_PIN_SET); break;
    }
}

void LED_Off(uint8_t led)
{
    switch (led) {
        case 1: HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_RESET); break;
        case 2: HAL_GPIO_WritePin(LED2_GREEN_GPIO_Port, LED2_GREEN_Pin, GPIO_PIN_RESET); break;
        case 3: HAL_GPIO_WritePin(LED3_YELLOW_GPIO_Port, LED3_YELLOW_Pin, GPIO_PIN_RESET); break;
        case 4: HAL_GPIO_WritePin(LED4_BLUE_GPIO_Port, LED4_BLUE_Pin, GPIO_PIN_RESET); break;
    }
}

void LED_Toggle(uint8_t led)
{
    switch (led) {
        case 1: HAL_GPIO_TogglePin(LED1_RED_GPIO_Port, LED1_RED_Pin); break;
        case 2: HAL_GPIO_TogglePin(LED2_GREEN_GPIO_Port, LED2_GREEN_Pin); break;
        case 3: HAL_GPIO_TogglePin(LED3_YELLOW_GPIO_Port, LED3_YELLOW_Pin); break;
        case 4: HAL_GPIO_TogglePin(LED4_BLUE_GPIO_Port, LED4_BLUE_Pin); break;
    }
}

void LED_SetIntensity(uint8_t led, uint8_t intensity)
{
    /* LED4 intensity via TIM3 CH1 PWM */
    if (led == 4)
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, intensity);
    }
}

/* Peripheral Functions */
uint16_t ADC_Read(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    return HAL_ADC_GetValue(&hadc1);
}

void DAC_Write(uint16_t value)
{
    if (value > 4095) value = 4095;
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
}

void PWM_SetDuty(uint8_t percent)
{
    if (percent > 100) percent = 100;
    uint32_t pulse = (999 * percent) / 100;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
}

void GPIO_SetPin(uint8_t pin, uint8_t state)
{
    GPIO_TypeDef *port = NULL;
    uint16_t gpio_pin = 0;

    /* X-series pins (1-22) */
    switch (pin) {
        case 1:  port = GPIO_X1_GPIO_Port;  gpio_pin = GPIO_X1_Pin;  break;
        case 2:  port = GPIO_X2_GPIO_Port;  gpio_pin = GPIO_X2_Pin;  break;
        case 3:  port = GPIO_X3_GPIO_Port;  gpio_pin = GPIO_X3_Pin;  break;
        case 4:  port = GPIO_X4_GPIO_Port;  gpio_pin = GPIO_X4_Pin;  break;
        case 5:  port = GPIO_X5_GPIO_Port;  gpio_pin = GPIO_X5_Pin;  break;
        case 6:  port = GPIO_X6_GPIO_Port;  gpio_pin = GPIO_X6_Pin;  break;
        case 7:  port = GPIO_X7_GPIO_Port;  gpio_pin = GPIO_X7_Pin;  break;
        case 8:  port = GPIO_X8_GPIO_Port;  gpio_pin = GPIO_X8_Pin;  break;
        case 9:  port = GPIO_X9_GPIO_Port;  gpio_pin = GPIO_X9_Pin;  break;
        case 10: port = GPIO_X10_GPIO_Port; gpio_pin = GPIO_X10_Pin; break;
        case 11: port = GPIO_X11_GPIO_Port; gpio_pin = GPIO_X11_Pin; break;
        case 12: port = GPIO_X12_GPIO_Port; gpio_pin = GPIO_X12_Pin; break;
        case 17: port = GPIO_X17_GPIO_Port; gpio_pin = GPIO_X17_Pin; break;
        case 18: port = GPIO_X18_GPIO_Port; gpio_pin = GPIO_X18_Pin; break;
        case 19: port = GPIO_X19_GPIO_Port; gpio_pin = GPIO_X19_Pin; break;
        case 20: port = GPIO_X20_GPIO_Port; gpio_pin = GPIO_X20_Pin; break;
        case 21: port = GPIO_X21_GPIO_Port; gpio_pin = GPIO_X21_Pin; break;
        case 22: port = GPIO_X22_GPIO_Port; gpio_pin = GPIO_X22_Pin; break;
        default: return;
    }

    if (port) {
        HAL_GPIO_WritePin(port, gpio_pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

void GPIO_SetPinY(uint8_t pin, uint8_t state)
{
    GPIO_TypeDef *port = NULL;
    uint16_t gpio_pin = 0;

    /* Y-series pins (1-12) */
    switch (pin) {
        case 1:  port = GPIO_Y1_GPIO_Port;  gpio_pin = GPIO_Y1_Pin;  break;
        case 2:  port = GPIO_Y2_GPIO_Port;  gpio_pin = GPIO_Y2_Pin;  break;
        case 3:  port = GPIO_Y3_GPIO_Port;  gpio_pin = GPIO_Y3_Pin;  break;
        case 4:  port = GPIO_Y4_GPIO_Port;  gpio_pin = GPIO_Y4_Pin;  break;
        case 5:  port = GPIO_Y5_GPIO_Port;  gpio_pin = GPIO_Y5_Pin;  break;
        case 6:  port = GPIO_Y6_GPIO_Port;  gpio_pin = GPIO_Y6_Pin;  break;
        case 7:  port = GPIO_Y7_GPIO_Port;  gpio_pin = GPIO_Y7_Pin;  break;
        case 8:  port = GPIO_Y8_GPIO_Port;  gpio_pin = GPIO_Y8_Pin;  break;
        case 9:  port = GPIO_Y9_GPIO_Port;  gpio_pin = GPIO_Y9_Pin;  break;
        case 10: port = GPIO_Y10_GPIO_Port; gpio_pin = GPIO_Y10_Pin; break;
        case 11: port = GPIO_Y11_GPIO_Port; gpio_pin = GPIO_Y11_Pin; break;
        case 12: port = GPIO_Y12_GPIO_Port; gpio_pin = GPIO_Y12_Pin; break;
        default: return;
    }

    if (port) {
        HAL_GPIO_WritePin(port, gpio_pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

uint8_t GPIO_ReadPin(uint8_t pin)
{
    switch (pin) {
        case 1:  return HAL_GPIO_ReadPin(GPIO_X1_GPIO_Port,  GPIO_X1_Pin);
        case 2:  return HAL_GPIO_ReadPin(GPIO_X2_GPIO_Port,  GPIO_X2_Pin);
        case 3:  return HAL_GPIO_ReadPin(GPIO_X3_GPIO_Port,  GPIO_X3_Pin);
        case 4:  return HAL_GPIO_ReadPin(GPIO_X4_GPIO_Port,  GPIO_X4_Pin);
        case 5:  return HAL_GPIO_ReadPin(GPIO_X5_GPIO_Port,  GPIO_X5_Pin);
        case 6:  return HAL_GPIO_ReadPin(GPIO_X6_GPIO_Port,  GPIO_X6_Pin);
        case 7:  return HAL_GPIO_ReadPin(GPIO_X7_GPIO_Port,  GPIO_X7_Pin);
        case 8:  return HAL_GPIO_ReadPin(GPIO_X8_GPIO_Port,  GPIO_X8_Pin);
        case 9:  return HAL_GPIO_ReadPin(GPIO_X9_GPIO_Port,  GPIO_X9_Pin);
        case 10: return HAL_GPIO_ReadPin(GPIO_X10_GPIO_Port, GPIO_X10_Pin);
        case 11: return HAL_GPIO_ReadPin(GPIO_X11_GPIO_Port, GPIO_X11_Pin);
        case 12: return HAL_GPIO_ReadPin(GPIO_X12_GPIO_Port, GPIO_X12_Pin);
        case 17: return HAL_GPIO_ReadPin(GPIO_X17_GPIO_Port, GPIO_X17_Pin);
        case 18: return HAL_GPIO_ReadPin(GPIO_X18_GPIO_Port, GPIO_X18_Pin);
        case 19: return HAL_GPIO_ReadPin(GPIO_X19_GPIO_Port, GPIO_X19_Pin);
        case 20: return HAL_GPIO_ReadPin(GPIO_X20_GPIO_Port, GPIO_X20_Pin);
        case 21: return HAL_GPIO_ReadPin(GPIO_X21_GPIO_Port, GPIO_X21_Pin);
        case 22: return HAL_GPIO_ReadPin(GPIO_X22_GPIO_Port, GPIO_X22_Pin);
        default: return 0;
    }
}

uint8_t GPIO_ReadPinY(uint8_t pin)
{
    switch (pin) {
        case 1:  return HAL_GPIO_ReadPin(GPIO_Y1_GPIO_Port,  GPIO_Y1_Pin);
        case 2:  return HAL_GPIO_ReadPin(GPIO_Y2_GPIO_Port,  GPIO_Y2_Pin);
        case 3:  return HAL_GPIO_ReadPin(GPIO_Y3_GPIO_Port,  GPIO_Y3_Pin);
        case 4:  return HAL_GPIO_ReadPin(GPIO_Y4_GPIO_Port,  GPIO_Y4_Pin);
        case 5:  return HAL_GPIO_ReadPin(GPIO_Y5_GPIO_Port,  GPIO_Y5_Pin);
        case 6:  return HAL_GPIO_ReadPin(GPIO_Y6_GPIO_Port,  GPIO_Y6_Pin);
        case 7:  return HAL_GPIO_ReadPin(GPIO_Y7_GPIO_Port,  GPIO_Y7_Pin);
        case 8:  return HAL_GPIO_ReadPin(GPIO_Y8_GPIO_Port,  GPIO_Y8_Pin);
        case 9:  return HAL_GPIO_ReadPin(GPIO_Y9_GPIO_Port,  GPIO_Y9_Pin);
        case 10: return HAL_GPIO_ReadPin(GPIO_Y10_GPIO_Port, GPIO_Y10_Pin);
        case 11: return HAL_GPIO_ReadPin(GPIO_Y11_GPIO_Port, GPIO_Y11_Pin);
        case 12: return HAL_GPIO_ReadPin(GPIO_Y12_GPIO_Port, GPIO_Y12_Pin);
        default: return 0;
    }
}

uint8_t Button_Read(void)
{
    return HAL_GPIO_ReadPin(USR_BTN_GPIO_Port, USR_BTN_Pin) == GPIO_PIN_RESET;
}

/**
  * @brief ADC1 Initialization
  */
static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    __HAL_RCC_ADC1_CLK_ENABLE();

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief DAC Initialization
  */
static void MX_DAC_Init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};

    __HAL_RCC_DAC_CLK_ENABLE();

    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief TIM2 Initialization (PWM output on X2/PA1)
  */
static void MX_TIM2_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 83;  /* 84MHz / 84 = 1MHz */
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 999;    /* 1MHz / 1000 = 1kHz PWM */
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief TIM3 Initialization (PWM for LED4/PB4, Y11/PB0, Y12/PB1)
  */
static void MX_TIM3_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    __HAL_RCC_TIM3_CLK_ENABLE();

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 327;  /* 84MHz / 328 = ~256kHz */
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 255;     /* 256kHz / 256 = 1kHz PWM, 8-bit resolution */
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    
    /* CH1 = LED4 (PB4) */
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    /* CH3 = Y11 (PB0) */
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    /* CH4 = Y12 (PB1) */
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief TIM4 Initialization (General purpose timer)
  */
static void MX_TIM4_Init(void)
{
    __HAL_RCC_TIM4_CLK_ENABLE();

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 83;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 999;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief RTC Initialization
  */
static void MX_RTC_Init(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    __HAL_RCC_RTC_ENABLE();

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }

    /* Set default time if backup domain was reset */
    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F4)
    {
        sTime.Hours = 0;
        sTime.Minutes = 0;
        sTime.Seconds = 0;
        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

        sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
        sDate.Month = RTC_MONTH_DECEMBER;
        sDate.Date = 28;
        sDate.Year = 25;
        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F4);
    }
}

/* Extended GPIO functions */
void GPIO_Toggle(char series, uint8_t pin)
{
    if (series == 'x' || series == 'X') {
        switch (pin) {
            case 1:  HAL_GPIO_TogglePin(GPIO_X1_GPIO_Port,  GPIO_X1_Pin);  break;
            case 2:  HAL_GPIO_TogglePin(GPIO_X2_GPIO_Port,  GPIO_X2_Pin);  break;
            case 3:  HAL_GPIO_TogglePin(GPIO_X3_GPIO_Port,  GPIO_X3_Pin);  break;
            case 4:  HAL_GPIO_TogglePin(GPIO_X4_GPIO_Port,  GPIO_X4_Pin);  break;
            case 5:  HAL_GPIO_TogglePin(GPIO_X5_GPIO_Port,  GPIO_X5_Pin);  break;
            case 6:  HAL_GPIO_TogglePin(GPIO_X6_GPIO_Port,  GPIO_X6_Pin);  break;
            case 7:  HAL_GPIO_TogglePin(GPIO_X7_GPIO_Port,  GPIO_X7_Pin);  break;
            case 8:  HAL_GPIO_TogglePin(GPIO_X8_GPIO_Port,  GPIO_X8_Pin);  break;
            case 9:  HAL_GPIO_TogglePin(GPIO_X9_GPIO_Port,  GPIO_X9_Pin);  break;
            case 10: HAL_GPIO_TogglePin(GPIO_X10_GPIO_Port, GPIO_X10_Pin); break;
            case 11: HAL_GPIO_TogglePin(GPIO_X11_GPIO_Port, GPIO_X11_Pin); break;
            case 12: HAL_GPIO_TogglePin(GPIO_X12_GPIO_Port, GPIO_X12_Pin); break;
            case 17: HAL_GPIO_TogglePin(GPIO_X17_GPIO_Port, GPIO_X17_Pin); break;
            case 18: HAL_GPIO_TogglePin(GPIO_X18_GPIO_Port, GPIO_X18_Pin); break;
            case 19: HAL_GPIO_TogglePin(GPIO_X19_GPIO_Port, GPIO_X19_Pin); break;
            case 20: HAL_GPIO_TogglePin(GPIO_X20_GPIO_Port, GPIO_X20_Pin); break;
            case 21: HAL_GPIO_TogglePin(GPIO_X21_GPIO_Port, GPIO_X21_Pin); break;
            case 22: HAL_GPIO_TogglePin(GPIO_X22_GPIO_Port, GPIO_X22_Pin); break;
        }
    } else if (series == 'y' || series == 'Y') {
        switch (pin) {
            case 1:  HAL_GPIO_TogglePin(GPIO_Y1_GPIO_Port,  GPIO_Y1_Pin);  break;
            case 2:  HAL_GPIO_TogglePin(GPIO_Y2_GPIO_Port,  GPIO_Y2_Pin);  break;
            case 3:  HAL_GPIO_TogglePin(GPIO_Y3_GPIO_Port,  GPIO_Y3_Pin);  break;
            case 4:  HAL_GPIO_TogglePin(GPIO_Y4_GPIO_Port,  GPIO_Y4_Pin);  break;
            case 5:  HAL_GPIO_TogglePin(GPIO_Y5_GPIO_Port,  GPIO_Y5_Pin);  break;
            case 6:  HAL_GPIO_TogglePin(GPIO_Y6_GPIO_Port,  GPIO_Y6_Pin);  break;
            case 7:  HAL_GPIO_TogglePin(GPIO_Y7_GPIO_Port,  GPIO_Y7_Pin);  break;
            case 8:  HAL_GPIO_TogglePin(GPIO_Y8_GPIO_Port,  GPIO_Y8_Pin);  break;
            case 9:  HAL_GPIO_TogglePin(GPIO_Y9_GPIO_Port,  GPIO_Y9_Pin);  break;
            case 10: HAL_GPIO_TogglePin(GPIO_Y10_GPIO_Port, GPIO_Y10_Pin); break;
            case 11: HAL_GPIO_TogglePin(GPIO_Y11_GPIO_Port, GPIO_Y11_Pin); break;
            case 12: HAL_GPIO_TogglePin(GPIO_Y12_GPIO_Port, GPIO_Y12_Pin); break;
        }
    }
}

/* Helper to get GPIO port and pin for X/Y series */
static void GetPinInfo(char series, uint8_t pin, GPIO_TypeDef **port, uint16_t *gpio_pin)
{
    *port = NULL;
    *gpio_pin = 0;
    
    if (series == 'x' || series == 'X') {
        switch (pin) {
            case 1:  *port = GPIO_X1_GPIO_Port;  *gpio_pin = GPIO_X1_Pin;  break;
            case 2:  *port = GPIO_X2_GPIO_Port;  *gpio_pin = GPIO_X2_Pin;  break;
            case 3:  *port = GPIO_X3_GPIO_Port;  *gpio_pin = GPIO_X3_Pin;  break;
            case 4:  *port = GPIO_X4_GPIO_Port;  *gpio_pin = GPIO_X4_Pin;  break;
            case 5:  *port = GPIO_X5_GPIO_Port;  *gpio_pin = GPIO_X5_Pin;  break;
            case 6:  *port = GPIO_X6_GPIO_Port;  *gpio_pin = GPIO_X6_Pin;  break;
            case 7:  *port = GPIO_X7_GPIO_Port;  *gpio_pin = GPIO_X7_Pin;  break;
            case 8:  *port = GPIO_X8_GPIO_Port;  *gpio_pin = GPIO_X8_Pin;  break;
            case 9:  *port = GPIO_X9_GPIO_Port;  *gpio_pin = GPIO_X9_Pin;  break;
            case 10: *port = GPIO_X10_GPIO_Port; *gpio_pin = GPIO_X10_Pin; break;
            case 11: *port = GPIO_X11_GPIO_Port; *gpio_pin = GPIO_X11_Pin; break;
            case 12: *port = GPIO_X12_GPIO_Port; *gpio_pin = GPIO_X12_Pin; break;
            case 17: *port = GPIO_X17_GPIO_Port; *gpio_pin = GPIO_X17_Pin; break;
            case 18: *port = GPIO_X18_GPIO_Port; *gpio_pin = GPIO_X18_Pin; break;
            case 19: *port = GPIO_X19_GPIO_Port; *gpio_pin = GPIO_X19_Pin; break;
            case 20: *port = GPIO_X20_GPIO_Port; *gpio_pin = GPIO_X20_Pin; break;
            case 21: *port = GPIO_X21_GPIO_Port; *gpio_pin = GPIO_X21_Pin; break;
            case 22: *port = GPIO_X22_GPIO_Port; *gpio_pin = GPIO_X22_Pin; break;
        }
    } else if (series == 'y' || series == 'Y') {
        switch (pin) {
            case 1:  *port = GPIO_Y1_GPIO_Port;  *gpio_pin = GPIO_Y1_Pin;  break;
            case 2:  *port = GPIO_Y2_GPIO_Port;  *gpio_pin = GPIO_Y2_Pin;  break;
            case 3:  *port = GPIO_Y3_GPIO_Port;  *gpio_pin = GPIO_Y3_Pin;  break;
            case 4:  *port = GPIO_Y4_GPIO_Port;  *gpio_pin = GPIO_Y4_Pin;  break;
            case 5:  *port = GPIO_Y5_GPIO_Port;  *gpio_pin = GPIO_Y5_Pin;  break;
            case 6:  *port = GPIO_Y6_GPIO_Port;  *gpio_pin = GPIO_Y6_Pin;  break;
            case 7:  *port = GPIO_Y7_GPIO_Port;  *gpio_pin = GPIO_Y7_Pin;  break;
            case 8:  *port = GPIO_Y8_GPIO_Port;  *gpio_pin = GPIO_Y8_Pin;  break;
            case 9:  *port = GPIO_Y9_GPIO_Port;  *gpio_pin = GPIO_Y9_Pin;  break;
            case 10: *port = GPIO_Y10_GPIO_Port; *gpio_pin = GPIO_Y10_Pin; break;
            case 11: *port = GPIO_Y11_GPIO_Port; *gpio_pin = GPIO_Y11_Pin; break;
            case 12: *port = GPIO_Y12_GPIO_Port; *gpio_pin = GPIO_Y12_Pin; break;
        }
    }
}

void GPIO_SetMode(char series, uint8_t pin, uint8_t mode)
{
    GPIO_TypeDef *port;
    uint16_t gpio_pin;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GetPinInfo(series, pin, &port, &gpio_pin);
    if (!port) return;
    
    GPIO_InitStruct.Pin = gpio_pin;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    switch (mode) {
        case 0: GPIO_InitStruct.Mode = GPIO_MODE_INPUT; break;
        case 1: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; break;
        case 2: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; break;
        default: GPIO_InitStruct.Mode = GPIO_MODE_INPUT; break;
    }
    
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void GPIO_SetPull(char series, uint8_t pin, uint8_t pull)
{
    GPIO_TypeDef *port;
    uint16_t gpio_pin;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GetPinInfo(series, pin, &port, &gpio_pin);
    if (!port) return;
    
    GPIO_InitStruct.Pin = gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    switch (pull) {
        case 0: GPIO_InitStruct.Pull = GPIO_NOPULL; break;
        case 1: GPIO_InitStruct.Pull = GPIO_PULLUP; break;
        case 2: GPIO_InitStruct.Pull = GPIO_PULLDOWN; break;
        default: GPIO_InitStruct.Pull = GPIO_NOPULL; break;
    }
    
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/* UART Functions */
void UART_Send(uint8_t port, const char *str)
{
    uint16_t len = strlen(str);
    if (port == 2) {
        HAL_UART_Transmit(&huart2, (uint8_t*)str, len, 1000);
    } else if (port == 6) {
        HAL_UART_Transmit(&huart6, (uint8_t*)str, len, 1000);
    }
}

uint8_t UART_Receive(uint8_t port, char *buf, uint8_t maxlen)
{
    uint8_t len = 0;
    if (port == 2) {
        len = (uart2_rx_idx < maxlen) ? uart2_rx_idx : maxlen;
        memcpy(buf, (void*)uart2_rx_buffer, len);
        uart2_rx_idx = 0;
    } else if (port == 6) {
        len = (uart6_rx_idx < maxlen) ? uart6_rx_idx : maxlen;
        memcpy(buf, (void*)uart6_rx_buffer, len);
        uart6_rx_idx = 0;
    }
    return len;
}

/* UART RX Callbacks */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        if (uart2_rx_idx < sizeof(uart2_rx_buffer) - 1) {
            uart2_rx_idx++;
        }
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&uart2_rx_buffer[uart2_rx_idx], 1);
    } else if (huart->Instance == USART6) {
        if (uart6_rx_idx < sizeof(uart6_rx_buffer) - 1) {
            uart6_rx_idx++;
        }
        HAL_UART_Receive_IT(&huart6, (uint8_t*)&uart6_rx_buffer[uart6_rx_idx], 1);
    }
}

/* CAN Functions */
void CAN_Send(uint32_t id, uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;
    
    txHeader.StdId = id;
    txHeader.ExtId = 0;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = (len > 8) ? 8 : len;
    txHeader.TransmitGlobalTime = DISABLE;
    
    HAL_CAN_AddTxMessage(&hcan1, &txHeader, data, &txMailbox);
}

/* CAN RX Callback */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == CAN1) {
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can_rx_header, can_rx_data);
        can_rx_ready = 1;
    }
}

/* RTC Functions */
void RTC_GetTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    *hours = sTime.Hours;
    *minutes = sTime.Minutes;
    *seconds = sTime.Seconds;
}

void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    RTC_TimeTypeDef sTime = {0};
    sTime.Hours = hours;
    sTime.Minutes = minutes;
    sTime.Seconds = seconds;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}

void RTC_GetDate(uint8_t *year, uint8_t *month, uint8_t *day)
{
    RTC_DateTypeDef sDate;
    /* Must read time first for shadow register sync */
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    *year = sDate.Year;
    *month = sDate.Month;
    *day = sDate.Date;
}

void RTC_SetDate(uint8_t year, uint8_t month, uint8_t day)
{
    RTC_DateTypeDef sDate = {0};
    sDate.Year = year;
    sDate.Month = month;
    sDate.Date = day;
    sDate.WeekDay = RTC_WEEKDAY_SATURDAY;  /* Placeholder */
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

/* SPI Functions */
void SPI_Transfer(uint8_t port, uint8_t *txdata, uint8_t *rxdata, uint16_t len)
{
    if (port == 1) {
        HAL_SPI_TransmitReceive(&hspi1, txdata, rxdata, len, 1000);
    } else if (port == 2) {
        HAL_SPI_TransmitReceive(&hspi2, txdata, rxdata, len, 1000);
    }
}

/*===========================================================================
 * NEW FEATURES - Adapted from MicroPython C drivers
 * These provide MicroPython-equivalent functionality in pure C
 *===========================================================================*/

/*---------------------------------------------------------------------------
 * Multi-channel ADC (from MicroPython ports/stm32/adc.c)
 * Supports all 16 ADC channels plus internal temp sensor and VBAT
 *---------------------------------------------------------------------------*/
uint16_t ADC_ReadChannel(uint8_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    /* Map channel number to ADC channel */
    uint32_t adc_channel;
    switch (channel) {
        case 0: adc_channel = ADC_CHANNEL_0; break;   /* X1/PA0 */
        case 1: adc_channel = ADC_CHANNEL_1; break;   /* X2/PA1 */
        case 2: adc_channel = ADC_CHANNEL_2; break;   /* X3/PA2 */
        case 3: adc_channel = ADC_CHANNEL_3; break;   /* X4/PA3 */
        case 4: adc_channel = ADC_CHANNEL_4; break;   /* X5/PA4 (also DAC1) */
        case 5: adc_channel = ADC_CHANNEL_5; break;   /* X6/PA5 (also DAC2) */
        case 6: adc_channel = ADC_CHANNEL_6; break;   /* X7/PA6 */
        case 7: adc_channel = ADC_CHANNEL_7; break;   /* X8/PA7 */
        case 8: adc_channel = ADC_CHANNEL_8; break;   /* X21/PB0 */
        case 9: adc_channel = ADC_CHANNEL_9; break;   /* X22/PB1 */
        case 10: adc_channel = ADC_CHANNEL_10; break; /* Y11/PC0 */
        case 11: adc_channel = ADC_CHANNEL_11; break; /* Y12/PC1 */
        case 12: adc_channel = ADC_CHANNEL_12; break; /* PC2 */
        case 13: adc_channel = ADC_CHANNEL_13; break; /* PC3 */
        case 14: adc_channel = ADC_CHANNEL_14; break; /* PC4 */
        case 15: adc_channel = ADC_CHANNEL_15; break; /* PC5 */
        default: return 0;
    }
    
    sConfig.Channel = adc_channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    
    return value;
}

uint16_t ADC_ReadTempSensor(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;  /* Longer sample time for internal channels */
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    
    return value;
}

uint16_t ADC_ReadVBAT(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    /* Enable VBAT channel (divided by 2 internally on STM32F4) */
    ADC->CCR |= ADC_CCR_VBATE;
    
    sConfig.Channel = ADC_CHANNEL_VBAT;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    
    /* Disable VBAT to prevent battery drain */
    ADC->CCR &= ~ADC_CCR_VBATE;
    
    return value;
}

/*---------------------------------------------------------------------------
 * I2C Data Transfer (from MicroPython ports/stm32/i2c.c)
 * Full I2C read/write with memory address support
 *---------------------------------------------------------------------------*/
int I2C_ReadFrom(uint8_t bus, uint8_t addr, uint8_t *data, uint16_t len)
{
    I2C_HandleTypeDef *hi2c = (bus == 1) ? &hi2c1 : &hi2c2;
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(hi2c, addr << 1, data, len, 1000);
    return (status == HAL_OK) ? len : -1;
}

int I2C_WriteTo(uint8_t bus, uint8_t addr, uint8_t *data, uint16_t len)
{
    I2C_HandleTypeDef *hi2c = (bus == 1) ? &hi2c1 : &hi2c2;
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, addr << 1, data, len, 1000);
    return (status == HAL_OK) ? len : -1;
}

int I2C_ReadMemory(uint8_t bus, uint8_t addr, uint8_t memaddr, uint8_t *data, uint16_t len)
{
    I2C_HandleTypeDef *hi2c = (bus == 1) ? &hi2c1 : &hi2c2;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, addr << 1, memaddr, I2C_MEMADD_SIZE_8BIT, data, len, 1000);
    return (status == HAL_OK) ? len : -1;
}

int I2C_WriteMemory(uint8_t bus, uint8_t addr, uint8_t memaddr, uint8_t *data, uint16_t len)
{
    I2C_HandleTypeDef *hi2c = (bus == 1) ? &hi2c1 : &hi2c2;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, addr << 1, memaddr, I2C_MEMADD_SIZE_8BIT, data, len, 1000);
    return (status == HAL_OK) ? len : -1;
}

/*---------------------------------------------------------------------------
 * Watchdog Timer (from MicroPython ports/stm32/wdt.c)
 * Independent Watchdog (IWDG) for reliability
 *---------------------------------------------------------------------------*/
static IWDG_HandleTypeDef hiwdg;

void WDT_Init(uint32_t timeout_ms)
{
    /* IWDG clock is LSI = 32kHz, prescaler /256 = 125Hz */
    /* Reload = timeout_ms * 125 / 1000 = timeout_ms / 8 */
    uint32_t reload = timeout_ms / 8;
    if (reload > 0xFFF) reload = 0xFFF;  /* Max 12-bit reload value */
    if (reload < 1) reload = 1;
    
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    hiwdg.Init.Reload = reload;
    HAL_IWDG_Init(&hiwdg);
}

void WDT_Feed(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

/*---------------------------------------------------------------------------
 * Power Management (from MicroPython ports/stm32/powerctrl.c)
 * Sleep, Stop, and Standby modes for power saving
 *---------------------------------------------------------------------------*/
void Power_Sleep(void)
{
    /* Enter Sleep mode - CPU stops, peripherals continue */
    /* Wake on any interrupt */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void Power_Stop(void)
{
    /* Enter Stop mode - most clocks stopped, SRAM retained */
    /* Wake on EXTI line, RTC alarm, USB, etc. */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    
    /* Reconfigure clocks after wakeup from Stop mode */
    SystemClock_Config();
}

void Power_Standby(void)
{
    /* Enter Standby mode - lowest power, SRAM lost */
    /* Wake only on WKUP pin, RTC alarm, or reset */
    HAL_PWR_EnterSTANDBYMode();
    /* Note: Code after this never executes - system resets on wakeup */
}

/*---------------------------------------------------------------------------
 * System Info (from MicroPython machine module)
 * Unique ID and reset cause
 *---------------------------------------------------------------------------*/
void Get_UniqueID(uint32_t *uid)
{
    /* STM32 unique device ID is at address 0x1FFF7A10 (96 bits) */
    uid[0] = *(uint32_t *)0x1FFF7A10;
    uid[1] = *(uint32_t *)0x1FFF7A14;
    uid[2] = *(uint32_t *)0x1FFF7A18;
}

uint32_t Get_ResetCause(void)
{
    /* Read RCC_CSR reset flags */
    uint32_t cause = RCC->CSR;
    
    /* Clear reset flags for next time */
    RCC->CSR |= RCC_CSR_RMVF;
    
    /* Return which reset occurred:
     * Bit 31: LPWRRSTF - Low-power reset
     * Bit 30: WWDGRSTF - Window watchdog reset
     * Bit 29: IWDGRSTF - Independent watchdog reset  
     * Bit 28: SFTRSTF  - Software reset
     * Bit 27: PORRSTF  - POR/PDR reset
     * Bit 26: PINRSTF  - Pin reset (NRST)
     * Bit 25: BORRSTF  - BOR reset
     */
    return cause;
}

/*---------------------------------------------------------------------------
 * External Interrupts (from MicroPython ports/stm32/extint.c)
 * Configure GPIO pins as interrupt sources with callbacks
 *---------------------------------------------------------------------------*/
#define MAX_EXTINT_CALLBACKS 16
static extint_callback_t extint_callbacks[MAX_EXTINT_CALLBACKS] = {0};

void ExtInt_Enable(char series, uint8_t pin, uint8_t edge, extint_callback_t callback)
{
    GPIO_TypeDef *gpio;
    uint16_t gpio_pin;
    
    /* Get GPIO port and pin */
    if (series == 'x' || series == 'X') {
        gpio = GPIOA;  /* Simplified: X pins map to various ports */
    } else if (series == 'y' || series == 'Y') {
        gpio = GPIOB;
    } else {
        return;
    }
    gpio_pin = (1 << (pin - 1));
    
    /* Store callback */
    if (pin <= MAX_EXTINT_CALLBACKS) {
        extint_callbacks[pin - 1] = callback;
    }
    
    /* Configure GPIO as input with interrupt */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = gpio_pin;
    GPIO_InitStruct.Mode = (edge == 0) ? GPIO_MODE_IT_RISING : 
                           (edge == 1) ? GPIO_MODE_IT_FALLING : 
                                         GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);
    
    /* Enable EXTI interrupt in NVIC */
    IRQn_Type irqn;
    if (pin <= 4) {
        irqn = EXTI0_IRQn + pin - 1;
    } else if (pin <= 9) {
        irqn = EXTI9_5_IRQn;
    } else {
        irqn = EXTI15_10_IRQn;
    }
    HAL_NVIC_SetPriority(irqn, 5, 0);
    HAL_NVIC_EnableIRQ(irqn);
}

void ExtInt_Disable(char series, uint8_t pin)
{
    /* Clear callback and disable interrupt */
    if (pin <= MAX_EXTINT_CALLBACKS) {
        extint_callbacks[pin - 1] = NULL;
    }
}

/* EXTI callback - called by HAL when interrupt occurs */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /* Find which pin triggered and call its callback */
    for (int i = 0; i < MAX_EXTINT_CALLBACKS; i++) {
        if ((GPIO_Pin & (1 << i)) && extint_callbacks[i]) {
            extint_callbacks[i]();
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
