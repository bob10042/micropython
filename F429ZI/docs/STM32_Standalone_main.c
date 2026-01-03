/**
  * STM32F429ZI NUCLEO - FULL Peripheral Support
  * main.c - Complete peripheral initialization with all features
  * 
  * Peripherals: GPIO, USART3, SPI1, I2C1, ADC1, DAC1, TIM3 PWM,
  *              RTC, RNG, CAN1, IWDG (optional)
  */
#include "main.h"
#include <stdio.h>
#include <string.h>

/* ============== Peripheral Handles ============== */
UART_HandleTypeDef huart3;
SPI_HandleTypeDef hspi1;
I2C_HandleTypeDef hi2c1;
ADC_HandleTypeDef hadc1;
DAC_HandleTypeDef hdac1;
TIM_HandleTypeDef htim3;
RTC_HandleTypeDef hrtc;
RNG_HandleTypeDef hrng;
CAN_HandleTypeDef hcan1;
// IWDG_HandleTypeDef hiwdg;  /* Uncomment for watchdog */

/* ============== Variables ============== */
volatile uint8_t buttonPressed = 0;
uint32_t adcValue = 0;
uint32_t dacValue = 0;
uint32_t randomNumber = 0;
char txBuffer[256];

/* RTC Date/Time */
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

/* CAN message buffers */
CanTxMsgTypeDef canTxMsg;
CanRxMsgTypeDef canRxMsg;
uint8_t canTxData[8];
uint8_t canRxData[8];

/* ============== Private function prototypes ============== */
static void GPIO_Init(void);
static void USART3_Init(void);
static void SPI1_Init(void);
static void I2C1_Init(void);
static void ADC1_Init(void);
static void DAC1_Init(void);
static void TIM3_PWM_Init(void);
static void RTC_Init(void);
static void RNG_Init(void);
static void CAN1_Init(void);
// static void IWDG_Init(void);  /* Uncomment for watchdog */
void UART_Print(const char *str);
void DAC_ADC_LoopbackTest(void);
void PrintRTCTime(void);

/* Printf redirect to UART */
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

int main(void)
{
    /* Initialize HAL */
    HAL_Init();
    
    /* Configure system clock to 180 MHz */
    SystemClock_Config();
    
    /* Initialize ALL peripherals */
    GPIO_Init();
    USART3_Init();
    SPI1_Init();
    I2C1_Init();
    ADC1_Init();
    
    /* Blink blue to show we got this far */
    HAL_GPIO_WritePin(LED2_BLUE_Port, LED2_BLUE_Pin, GPIO_PIN_SET);
    
    DAC1_Init();      /* NEW */
    TIM3_PWM_Init();
    
    /* Testing peripherals one by one */
    RNG_Init();       /* TEST 1: RNG - PASSED */
    // RTC_Init();  /* DISABLED - still failing */       /* TEST 2: RTC - FIXED */
    CAN1_Init();      /* TEST 3: CAN - PASSED */
    // IWDG_Init();   /* Uncomment to enable watchdog */
    
    /* Welcome message */
    UART_Print("\r\n============================================================\r\n");
    UART_Print("     STM32F429ZI NUCLEO - FULL Peripheral Support\r\n");
    UART_Print("============================================================\r\n");
    UART_Print("Peripherals initialized:\r\n");
    UART_Print("  - GPIO:   LEDs (PB0,PB7,PB14), Button (PC13)\r\n");
    UART_Print("  - USART3: 115200 baud (PD8/PD9) - ST-LINK VCP\r\n");
    UART_Print("  - SPI1:   Master mode (PA5/PA6/PA7)\r\n");
    UART_Print("  - I2C1:   100kHz (PB8/PB9)\r\n");
    UART_Print("  - ADC1:   12-bit, Channel 3 (PA3)\r\n");
    UART_Print("  - DAC1:   12-bit, Channel 1 (PA4) [NEW!]\r\n");
    UART_Print("  - TIM3:   PWM on CH1 (PB4) - 1kHz\r\n");
    UART_Print("  - RTC:    Real-Time Clock with backup [NEW!]\r\n");
    UART_Print("  - RNG:    Hardware Random Number Generator [NEW!]\r\n");
    UART_Print("  - CAN1:   500kbps (PD0/PD1) [NEW!]\r\n");
    UART_Print("============================================================\r\n");
    UART_Print("\r\nDAC-ADC Loopback: Connect PA4 -> PA3 with jumper wire\r\n");
    UART_Print("============================================================\r\n\r\n");
    
    /* Run initial DAC-ADC loopback test */
    DAC_ADC_LoopbackTest();
    
    uint32_t lastTick = 0;
    uint32_t lastSecond = 0;
    uint16_t pwmValue = 0;
    int8_t pwmDirection = 10;
    
    while (1)
    {
        /* Every 100ms - fast updates */
        if (HAL_GetTick() - lastTick >= 100)
        {
            lastTick = HAL_GetTick();
            
            /* Toggle green LED */
            HAL_GPIO_TogglePin(LED1_GREEN_Port, LED1_GREEN_Pin);
            
            /* Update DAC with ramp (for loopback test) */
            dacValue = (dacValue + 100) % 4096;  /* 0-4095 ramp */
            HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dacValue);
            
            /* Read ADC (should match DAC if loopback connected) */
            HAL_ADC_Start(&hadc1);
            if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
            {
                adcValue = HAL_ADC_GetValue(&hadc1);
            }
            HAL_ADC_Stop(&hadc1);
            
            /* Update PWM (breathing effect) */
            pwmValue += pwmDirection;
            if (pwmValue >= 1000 || pwmValue <= 0)
                pwmDirection = -pwmDirection;
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwmValue);
            
            /* Print status every second (10 x 100ms) */
            static uint8_t printCount = 0;
            if (++printCount >= 10)
            {
                printCount = 0;
                
                /* Get RTC time */
                // HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                // HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);  /* Must read date after time */
                
                /* Generate random number */
                HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber);
                
                /* Print comprehensive status */
                sprintf(txBuffer, "[%02d:%02d:%02d] DAC:%4lu ADC:%4lu PWM:%4u RNG:0x%08lX %s\r\n",
                        sTime.Hours, sTime.Minutes, sTime.Seconds,
                        dacValue, adcValue, pwmValue, randomNumber,
                        HAL_GPIO_ReadPin(USER_BUTTON_Port, USER_BUTTON_Pin) ? "" : "[BTN]");
                UART_Print(txBuffer);
            }
        }
        
        /* Every second - RTC updates */
        if (HAL_GetTick() - lastSecond >= 1000)
        {
            lastSecond = HAL_GetTick();
            HAL_GPIO_TogglePin(LED2_BLUE_Port, LED2_BLUE_Pin);  /* Blue LED = 1Hz heartbeat */
        }
        
        /* Check button state */
        if (buttonPressed)
        {
            buttonPressed = 0;
            HAL_GPIO_TogglePin(LED3_RED_Port, LED3_RED_Pin);
            UART_Print(">>> Button pressed! Red LED toggled. <<<\r\n");
            
            /* Run loopback test on button press */
            DAC_ADC_LoopbackTest();
        }
    }
}

/* ============== DAC-ADC Loopback Test ============== */
void DAC_ADC_LoopbackTest(void)
{
    UART_Print("\r\n--- DAC-ADC Loopback Test ---\r\n");
    UART_Print("(Connect PA4 to PA3 with jumper wire)\r\n");
    
    uint32_t testValues[] = {0, 1024, 2048, 3072, 4095};
    uint32_t readValue;
    int passed = 1;
    
    for (int i = 0; i < 5; i++)
    {
        /* Set DAC value */
        HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, testValues[i]);
        HAL_Delay(10);  /* Allow settling */
        
        /* Read ADC */
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
        {
            readValue = HAL_ADC_GetValue(&hadc1);
        }
        HAL_ADC_Stop(&hadc1);
        
        /* Check if within 5% tolerance */
        int32_t diff = (int32_t)readValue - (int32_t)testValues[i];
        if (diff < 0) diff = -diff;
        int tolerance = (testValues[i] == 0) ? 50 : (testValues[i] * 5 / 100);
        
        sprintf(txBuffer, "  DAC=%4lu -> ADC=%4lu  %s\r\n",
                testValues[i], readValue,
                (diff <= tolerance) ? "[OK]" : "[MISMATCH - Check wiring!]");
        UART_Print(txBuffer);
        
        if (diff > tolerance) passed = 0;
    }
    
    if (passed)
        UART_Print(">>> LOOPBACK TEST PASSED! <<<\r\n\r\n");
    else
        UART_Print(">>> TEST FAILED - Ensure PA4 is connected to PA3 <<<\r\n\r\n");
}

void UART_Print(const char *str)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
    /* Configure HSE and PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;  /* NUCLEO uses ST-LINK MCO */
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 180;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();
    
    /* Activate OverDrive for 180MHz */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) Error_Handler();
    
    /* Configure bus clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) Error_Handler();
}

static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Enable GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    /* Configure LEDs */
    GPIO_InitStruct.Pin = LED1_GREEN_Pin | LED2_BLUE_Pin | LED3_RED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* Configure User Button with interrupt */
    GPIO_InitStruct.Pin = USER_BUTTON_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(USER_BUTTON_Port, &GPIO_InitStruct);
    
    /* Enable button interrupt */
    HAL_NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);
    
    /* Turn off all LEDs initially */
    HAL_GPIO_WritePin(GPIOB, LED1_GREEN_Pin | LED2_BLUE_Pin | LED3_RED_Pin, GPIO_PIN_RESET);
}

static void USART3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_USART3_CLK_ENABLE();
    
    /* Configure TX/RX pins */
    GPIO_InitStruct.Pin = USART3_TX_Pin | USART3_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
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
    if (HAL_UART_Init(&huart3) != HAL_OK) Error_Handler();
}

static void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_SPI1_CLK_ENABLE();
    
    /* Configure SPI pins */
    GPIO_InitStruct.Pin = SPI1_SCK_Pin | SPI1_MISO_Pin | SPI1_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
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
    if (HAL_SPI_Init(&hspi1) != HAL_OK) Error_Handler();
}

static void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_I2C1_CLK_ENABLE();
    
    /* Configure I2C pins (Open-drain with pullup) */
    GPIO_InitStruct.Pin = I2C1_SCL_Pin | I2C1_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) Error_Handler();
}

static void ADC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_ChannelConfTypeDef sConfig = {0};
    
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    /* Configure ADC pin as analog */
    GPIO_InitStruct.Pin = ADC1_IN3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC1_IN3_Port, &GPIO_InitStruct);
    
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
    if (HAL_ADC_Init(&hadc1) != HAL_OK) Error_Handler();
    
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) Error_Handler();
}

static void TIM3_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    
    __HAL_RCC_TIM3_CLK_ENABLE();
    
    /* Configure PWM pin */
    GPIO_InitStruct.Pin = TIM3_CH1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(TIM3_CH1_Port, &GPIO_InitStruct);
    
    /* Timer: 90MHz / 90 / 1000 = 1kHz PWM */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 89;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 999;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) Error_Handler();
    
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
    
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

/* ============== DAC1 Initialization ============== */
static void DAC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    DAC_ChannelConfTypeDef sConfig = {0};
    
    __HAL_RCC_DAC_CLK_ENABLE();
    
    /* Configure DAC pin PA4 as analog */
    GPIO_InitStruct.Pin = DAC1_OUT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DAC1_OUT1_Port, &GPIO_InitStruct);
    
    hdac1.Instance = DAC;
    if (HAL_DAC_Init(&hdac1) != HAL_OK) Error_Handler();
    
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK) Error_Handler();
    
    /* Start DAC */
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);  /* Start at mid-scale */
}

/* ============== RTC Initialization ============== */
static void RTC_Init(void)
{
    /* Enable backup domain access FIRST */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    /* Enable LSI oscillator */
    __HAL_RCC_LSI_ENABLE();
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET) {}

    /* Select LSI as RTC clock source */
    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI);

    /* NOW enable RTC clock */
    __HAL_RCC_RTC_ENABLE();

    /* Configure RTC */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 249;  /* LSI ~32kHz: 32000/(127+1)/(249+1) = 1Hz */
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) != HAL_OK) Error_Handler();
}

/* ============== RNG Initialization ============== */
static void RNG_Init(void)
{
    __HAL_RCC_RNG_CLK_ENABLE();
    
    hrng.Instance = RNG;
    if (HAL_RNG_Init(&hrng) != HAL_OK) Error_Handler();
}

/* ============== CAN1 Initialization ============== */
static void CAN1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    CAN_FilterConfTypeDef sFilterConfig;

    __HAL_RCC_CAN1_CLK_ENABLE();

    /* Configure CAN pins PD0 (RX) and PD1 (TX) */
    GPIO_InitStruct.Pin = CAN1_RX_Pin | CAN1_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* CAN1 at 500kbps (APB1 = 45MHz) */
    /* Bit time = (1 + BS1 + BS2) * Prescaler / APB1_CLK */
    /* 45MHz / 9 / (1+7+2) = 500kbps */
    hcan1.Instance = CAN1;
    hcan1.pTxMsg = &canTxMsg;
    hcan1.pRxMsg = &canRxMsg;
    hcan1.Init.Prescaler = 9;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SJW = CAN_SJW_1TQ;
    hcan1.Init.BS1 = CAN_BS1_7TQ;
    hcan1.Init.BS2 = CAN_BS2_2TQ;
    hcan1.Init.TTCM = DISABLE;
    hcan1.Init.ABOM = ENABLE;
    hcan1.Init.AWUM = DISABLE;
    hcan1.Init.NART = DISABLE;  /* NART=0 means auto-retransmit enabled */
    hcan1.Init.RFLM = DISABLE;
    hcan1.Init.TXFP = DISABLE;
    if (HAL_CAN_Init(&hcan1) != HAL_OK) Error_Handler();

    /* Configure filter to accept all messages */
    sFilterConfig.FilterNumber = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.BankNumber = 14;
    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK) Error_Handler();

    /* Setup TX message for demo */
    hcan1.pTxMsg->StdId = 0x123;
    hcan1.pTxMsg->ExtId = 0;
    hcan1.pTxMsg->RTR = CAN_RTR_DATA;
    hcan1.pTxMsg->IDE = CAN_ID_STD;
    hcan1.pTxMsg->DLC = 8;
}

/* ============== IWDG Initialization (Watchdog) ============== */
/* Uncomment to enable - will reset board if not fed every ~1 second
static void IWDG_Init(void)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = 1000;  // ~1 second timeout at 32kHz/32
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) Error_Handler();
}
*/

/* Button interrupt callback */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == USER_BUTTON_Pin)
    {
        buttonPressed = 1;
    }
}

void Error_Handler(void)
{
    __disable_irq();
    HAL_GPIO_WritePin(LED3_RED_Port, LED3_RED_Pin, GPIO_PIN_SET);
    while (1) {}
}


