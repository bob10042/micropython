/**
  ******************************************************************************
  * @file    main.c
  * @brief   Comprehensive GPIO & Peripheral Test Program for NUCLEO-F429ZI
  * @author  Generated for board testing
  * @details Initializes all GPIO, LEDs, Button, USART, ADC, SPI, I2C
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
#include <stdio.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;
ADC_HandleTypeDef hadc1;
SPI_HandleTypeDef hspi1;
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim3;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void GPIO_Init_All(void);
static void USART3_Init(void);
static void ADC1_Init(void);
static void SPI1_Init(void);
static void I2C1_Init(void);
static void TIM3_PWM_Init(void);
static void Error_Handler(void);

/* Printf redirect to USART3 */
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

/* Global variables for button handling */
volatile uint8_t button_pressed = 0;
volatile uint8_t led_mode = 0;
uint32_t adc_value = 0;

/**
  * @brief  Main program
  */
int main(void)
{
    /* HAL Init */
    HAL_Init();

    /* Configure system clock to 180 MHz */
    SystemClock_Config();

    /* Initialize all GPIO pins */
    GPIO_Init_All();

    /* Initialize USART3 for serial communication (COM5) */
    USART3_Init();

    /* Initialize ADC1 */
    ADC1_Init();

    /* Initialize SPI1 */
    SPI1_Init();

    /* Initialize I2C1 */
    I2C1_Init();

    /* Initialize TIM3 for PWM */
    TIM3_PWM_Init();

    /* Initialize LEDs using BSP */
    BSP_LED_Init(LED1);  // Green - PB0
    BSP_LED_Init(LED2);  // Blue - PB7
    BSP_LED_Init(LED3);  // Red - PB14

    /* Initialize Button with interrupt */
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

    /* Print startup message */
    printf("\r\n");
    printf("========================================\r\n");
    printf("  STM32F429ZI NUCLEO-144 Test Program\r\n");
    printf("========================================\r\n");
    printf("System Clock: 180 MHz\r\n");
    printf("\r\n");
    printf("Initialized Peripherals:\r\n");
    printf("  - GPIO: All ports (PA-PG)\r\n");
    printf("  - LEDs: PB0(Green), PB7(Blue), PB14(Red)\r\n");
    printf("  - Button: PC13 (with interrupt)\r\n");
    printf("  - USART3: PD8(TX), PD9(RX) @ 115200 baud\r\n");
    printf("  - ADC1: PA3 (A0 on Arduino header)\r\n");
    printf("  - SPI1: PA5(SCK), PA6(MISO), PA7(MOSI)\r\n");
    printf("  - I2C1: PB8(SCL), PB9(SDA)\r\n");
    printf("  - TIM3: PB4 (PWM output)\r\n");
    printf("\r\n");
    printf("Press USER button (B1) to change LED mode\r\n");
    printf("========================================\r\n\r\n");

    uint32_t tick_count = 0;
    uint32_t last_tick = HAL_GetTick();
    uint8_t pwm_direction = 1;
    uint16_t pwm_value = 0;

    /* Main loop */
    while (1)
    {
        /* Handle button press */
        if (button_pressed)
        {
            button_pressed = 0;
            led_mode = (led_mode + 1) % 4;
            printf("Button pressed! LED Mode: %d\r\n", led_mode);
            
            /* Turn off all LEDs when mode changes */
            BSP_LED_Off(LED1);
            BSP_LED_Off(LED2);
            BSP_LED_Off(LED3);
        }

        /* LED pattern based on mode */
        if (HAL_GetTick() - last_tick >= 100)  // Every 100ms
        {
            last_tick = HAL_GetTick();
            tick_count++;

            switch (led_mode)
            {
                case 0:  // All LEDs blink together
                    if (tick_count % 5 == 0)
                    {
                        BSP_LED_Toggle(LED1);
                        BSP_LED_Toggle(LED2);
                        BSP_LED_Toggle(LED3);
                    }
                    break;

                case 1:  // Sequential LED pattern
                    BSP_LED_Off(LED1);
                    BSP_LED_Off(LED2);
                    BSP_LED_Off(LED3);
                    switch (tick_count % 3)
                    {
                        case 0: BSP_LED_On(LED1); break;
                        case 1: BSP_LED_On(LED2); break;
                        case 2: BSP_LED_On(LED3); break;
                    }
                    break;

                case 2:  // Binary counter on LEDs
                    if (tick_count % 5 == 0)
                    {
                        static uint8_t counter = 0;
                        counter = (counter + 1) % 8;
                        (counter & 0x01) ? BSP_LED_On(LED1) : BSP_LED_Off(LED1);
                        (counter & 0x02) ? BSP_LED_On(LED2) : BSP_LED_Off(LED2);
                        (counter & 0x04) ? BSP_LED_On(LED3) : BSP_LED_Off(LED3);
                    }
                    break;

                case 3:  // Only green LED breathing (PWM demo)
                    BSP_LED_Off(LED2);
                    BSP_LED_Off(LED3);
                    BSP_LED_On(LED1);
                    break;
            }

            /* Update PWM (breathing effect on TIM3) */
            if (pwm_direction)
            {
                pwm_value += 10;
                if (pwm_value >= 1000) pwm_direction = 0;
            }
            else
            {
                pwm_value -= 10;
                if (pwm_value <= 0) pwm_direction = 1;
            }
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_value);

            /* Read and display ADC every second */
            if (tick_count % 10 == 0)
            {
                HAL_ADC_Start(&hadc1);
                if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
                {
                    adc_value = HAL_ADC_GetValue(&hadc1);
                    float voltage = (adc_value * 3.3f) / 4096.0f;
                    printf("[%05lu] ADC(PA3): %4lu (%.2fV) | Mode: %d | PWM: %4d\r\n", 
                           tick_count/10, adc_value, voltage, led_mode, pwm_value);
                }
                HAL_ADC_Stop(&hadc1);
            }
        }
    }
}

/**
  * @brief  Initialize ALL GPIO Ports
  */
static void GPIO_Init_All(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable all GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /*--- Configure GPIO pins for general use ---*/
    
    /* GPIOA Configuration */
    /* PA0-PA2: General GPIO Output (Arduino D0-D2 area) */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PA3: ADC Input (A0 on Arduino header) - configured in ADC1_Init */
    
    /* PA4: DAC Output / General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PA5-PA7: SPI1 - configured in SPI1_Init */
    
    /* PA8: MCO1 / USB OTG / General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PA9-PA12: USB OTG FS pins - leave as default */
    
    /* PA15: General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* GPIOB Configuration */
    /* PB0, PB7, PB14: LEDs - configured by BSP_LED_Init */
    
    /* PB1, PB2: General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PB3, PB5, PB6: General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PB4: TIM3_CH1 PWM - configured in TIM3_PWM_Init */
    
    /* PB8-PB9: I2C1 - configured in I2C1_Init */
    
    /* PB10-PB15: General GPIO (except PB14 which is LED3) */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* GPIOC Configuration */
    /* PC0-PC5: ADC inputs / General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;  /* Analog for ADC */
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* PC6-PC12: General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* PC13: User Button - configured by BSP_PB_Init */
    
    /* PC14-PC15: OSC32 - leave as default */

    /* GPIOD Configuration */
    /* PD0-PD7: General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* PD8-PD9: USART3 - configured in USART3_Init */
    
    /* PD10-PD15: General GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* GPIOE Configuration - All general GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* GPIOF Configuration - Mixed ADC and GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | 
                         GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    
    /* PF3, PF4, PF5, PF10: ADC3 inputs */
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* GPIOG Configuration - Ethernet and general GPIO */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                         GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    
    /* PG6, PG7: USB power control */
    /* PG11, PG13, PG14: Ethernet - leave for ETH peripheral */
}

/**
  * @brief  USART3 Initialization (PD8=TX, PD9=RX) - Virtual COM Port
  */
static void USART3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable clocks */
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* USART3 GPIO Configuration: PD8=TX, PD9=RX */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART3 configuration */
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  ADC1 Initialization (PA3 = A0 on Arduino header)
  */
static void ADC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    /* Enable clocks */
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* ADC1 GPIO Configuration: PA3 -> ADC1_IN3 */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 configuration */
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

    /* Configure ADC channel */
    sConfig.Channel = ADC_CHANNEL_3;  /* PA3 */
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  SPI1 Initialization (PA5=SCK, PA6=MISO, PA7=MOSI)
  */
static void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable clocks */
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* SPI1 GPIO Configuration: PA5=SCK, PA6=MISO, PA7=MOSI */
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI1 configuration */
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;

    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  I2C1 Initialization (PB8=SCL, PB9=SDA)
  */
static void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable clocks */
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* I2C1 GPIO Configuration: PB8=SCL, PB9=SDA */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 configuration */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
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
}

/**
  * @brief  TIM3 PWM Initialization (PB4 = TIM3_CH1)
  */
static void TIM3_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* Enable clocks */
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* TIM3 GPIO Configuration: PB4 -> TIM3_CH1 */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* TIM3 configuration for PWM */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 179;  /* 180MHz / 180 = 1MHz */
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 999;     /* 1MHz / 1000 = 1kHz PWM frequency */
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    /* PWM Channel 1 configuration */
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;  /* 50% duty cycle initially */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start PWM */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

/**
  * @brief  System Clock Configuration - 180 MHz
  */
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Voltage scaling */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Enable Over-drive mode */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }

    /* Select PLL as system clock source */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | 
                                   RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
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
  * @brief  EXTI line detection callback (Button interrupt)
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == USER_BUTTON_PIN)
    {
        button_pressed = 1;
    }
}

/**
  * @brief  Error Handler
  */
static void Error_Handler(void)
{
    BSP_LED_On(LED3);  /* Red LED on for error */
    while(1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    while (1) {}
}
#endif
