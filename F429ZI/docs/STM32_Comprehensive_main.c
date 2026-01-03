/**
  ******************************************************************************
  * @file    main.c
  * @brief   Comprehensive GPIO & Peripheral Test for NUCLEO-F429ZI
  ******************************************************************************
  */

#include "main.h"

/* Private variables */
UART_HandleTypeDef huart3;
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim3;

/* Private function prototypes */
static void SystemClock_Config(void);
static void USART3_Init(void);
static void ADC1_Init(void);
static void TIM3_PWM_Init(void);
static void GPIO_Init_All(void);
static void Error_Handler(void);

/* Printf redirect */
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

/* Global variables */
volatile uint8_t button_pressed = 0;
volatile uint8_t led_mode = 0;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_Init_All();
    USART3_Init();
    ADC1_Init();
    TIM3_PWM_Init();

    /* Initialize LEDs */
    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);
    BSP_LED_Init(LED3);

    /* Initialize Button with interrupt */
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

    /* Startup message */
    printf("\r\n========================================\r\n");
    printf("  STM32F429ZI NUCLEO Test Program\r\n");
    printf("========================================\r\n");
    printf("Peripherals Initialized:\r\n");
    printf("  - GPIO: All ports enabled\r\n");
    printf("  - LEDs: PB0(Green), PB7(Blue), PB14(Red)\r\n");
    printf("  - Button: PC13 (interrupt mode)\r\n");
    printf("  - USART3: PD8/PD9 @ 115200 baud\r\n");
    printf("  - ADC1: PA3 (Analog input)\r\n");
    printf("  - TIM3: PB4 (PWM output)\r\n");
    printf("Press B1 button to change LED mode\r\n");
    printf("========================================\r\n\r\n");

    uint32_t tick = 0;
    uint32_t last = HAL_GetTick();
    uint16_t pwm = 0;
    uint8_t dir = 1;

    while (1)
    {
        if (button_pressed)
        {
            button_pressed = 0;
            led_mode = (led_mode + 1) % 4;
            printf("Button! Mode: %d\r\n", led_mode);
            BSP_LED_Off(LED1);
            BSP_LED_Off(LED2);
            BSP_LED_Off(LED3);
        }

        if (HAL_GetTick() - last >= 100)
        {
            last = HAL_GetTick();
            tick++;

            /* LED patterns */
            switch (led_mode)
            {
                case 0: // All blink
                    if (tick % 5 == 0) {
                        BSP_LED_Toggle(LED1);
                        BSP_LED_Toggle(LED2);
                        BSP_LED_Toggle(LED3);
                    }
                    break;
                case 1: // Sequential
                    BSP_LED_Off(LED1); BSP_LED_Off(LED2); BSP_LED_Off(LED3);
                    if (tick%3==0) BSP_LED_On(LED1);
                    else if (tick%3==1) BSP_LED_On(LED2);
                    else BSP_LED_On(LED3);
                    break;
                case 2: // Binary counter
                    if (tick % 5 == 0) {
                        static uint8_t cnt = 0;
                        cnt = (cnt + 1) % 8;
                        (cnt & 1) ? BSP_LED_On(LED1) : BSP_LED_Off(LED1);
                        (cnt & 2) ? BSP_LED_On(LED2) : BSP_LED_Off(LED2);
                        (cnt & 4) ? BSP_LED_On(LED3) : BSP_LED_Off(LED3);
                    }
                    break;
                case 3: // Green only
                    BSP_LED_On(LED1);
                    BSP_LED_Off(LED2);
                    BSP_LED_Off(LED3);
                    break;
            }

            /* PWM breathing */
            if (dir) { pwm += 20; if (pwm >= 1000) dir = 0; }
            else { pwm -= 20; if (pwm <= 0) dir = 1; }
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm);

            /* ADC reading every second */
            if (tick % 10 == 0)
            {
                HAL_ADC_Start(&hadc1);
                if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
                {
                    uint32_t adc = HAL_ADC_GetValue(&hadc1);
                    float volt = (adc * 3.3f) / 4096.0f;
                    printf("[%05lu] ADC(PA3)=%4lu (%.2fV) Mode=%d PWM=%d\r\n",
                           tick/10, adc, volt, led_mode, pwm);
                }
                HAL_ADC_Stop(&hadc1);
            }
        }
    }
}

static void GPIO_Init_All(void)
{
    /* Enable all GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
}

static void USART3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart3);
}

static void ADC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
    HAL_ADC_Init(&hadc1);

    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

static void TIM3_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 179;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 999;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

    if (HAL_PWREx_EnableOverDrive() != HAL_OK) Error_Handler();

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) Error_Handler();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == USER_BUTTON_PIN) button_pressed = 1;
}

static void Error_Handler(void)
{
    BSP_LED_On(LED3);
    while(1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { while(1); }
#endif
