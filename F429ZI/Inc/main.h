/**
  * STM32F429ZI NUCLEO - FULL Peripheral Support Project
  * main.h - Complete pin definitions and function prototypes
  */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* === LED Definitions (NUCLEO-F429ZI) === */
#define LED1_GREEN_Pin       GPIO_PIN_0
#define LED1_GREEN_Port      GPIOB
#define LED2_BLUE_Pin        GPIO_PIN_7
#define LED2_BLUE_Port       GPIOB
#define LED3_RED_Pin         GPIO_PIN_14
#define LED3_RED_Port        GPIOB

/* === User Button === */
#define USER_BUTTON_Pin      GPIO_PIN_13
#define USER_BUTTON_Port     GPIOC
#define USER_BUTTON_EXTI_IRQn EXTI15_10_IRQn

/* === USART3 (ST-LINK Virtual COM Port) === */
#define USART3_TX_Pin        GPIO_PIN_8
#define USART3_TX_Port       GPIOD
#define USART3_RX_Pin        GPIO_PIN_9
#define USART3_RX_Port       GPIOD

/* === SPI1 === */
#define SPI1_SCK_Pin         GPIO_PIN_5
#define SPI1_SCK_Port        GPIOA
#define SPI1_MISO_Pin        GPIO_PIN_6
#define SPI1_MISO_Port       GPIOA
#define SPI1_MOSI_Pin        GPIO_PIN_7
#define SPI1_MOSI_Port       GPIOA

/* === I2C1 === */
#define I2C1_SCL_Pin         GPIO_PIN_8
#define I2C1_SCL_Port        GPIOB
#define I2C1_SDA_Pin         GPIO_PIN_9
#define I2C1_SDA_Port        GPIOB

/* === ADC1 (PA3 = ADC123_IN3) === */
#define ADC1_IN3_Pin         GPIO_PIN_3
#define ADC1_IN3_Port        GPIOA

/* === DAC (PA4 = DAC1_OUT1) === NEW! */
#define DAC1_OUT1_Pin        GPIO_PIN_4
#define DAC1_OUT1_Port       GPIOA

/* === TIM3 PWM (PB4 = TIM3_CH1) === */
#define TIM3_CH1_Pin         GPIO_PIN_4
#define TIM3_CH1_Port        GPIOB

/* === CAN1 (PD0/PD1) === NEW! */
#define CAN1_RX_Pin          GPIO_PIN_0
#define CAN1_RX_Port         GPIOD
#define CAN1_TX_Pin          GPIO_PIN_1
#define CAN1_TX_Port         GPIOD

/* === USB OTG FS (PA11/PA12) === */
#define USB_OTG_FS_DM_Pin    GPIO_PIN_11
#define USB_OTG_FS_DM_Port   GPIOA
#define USB_OTG_FS_DP_Pin    GPIO_PIN_12
#define USB_OTG_FS_DP_Port   GPIOA
#define USB_OTG_FS_ID_Pin    GPIO_PIN_10
#define USB_OTG_FS_ID_Port   GPIOA

/* === Ethernet RMII (LAN8742 PHY) === */
#define ETH_REF_CLK_Pin      GPIO_PIN_1   /* PA1 */
#define ETH_REF_CLK_Port     GPIOA
#define ETH_MDIO_Pin         GPIO_PIN_2   /* PA2 */
#define ETH_MDIO_Port        GPIOA
#define ETH_MDC_Pin          GPIO_PIN_1   /* PC1 */
#define ETH_MDC_Port         GPIOC
#define ETH_CRS_DV_Pin       GPIO_PIN_7   /* PA7 - conflicts with SPI1! */
#define ETH_CRS_DV_Port      GPIOA
#define ETH_RXD0_Pin         GPIO_PIN_4   /* PC4 */
#define ETH_RXD0_Port        GPIOC
#define ETH_RXD1_Pin         GPIO_PIN_5   /* PC5 */
#define ETH_RXD1_Port        GPIOC
#define ETH_TX_EN_Pin        GPIO_PIN_11  /* PG11 */
#define ETH_TX_EN_Port       GPIOG
#define ETH_TXD0_Pin         GPIO_PIN_13  /* PG13 */
#define ETH_TXD0_Port        GPIOG
#define ETH_TXD1_Pin         GPIO_PIN_13  /* PB13 */
#define ETH_TXD1_Port        GPIOB

/* === TIM1 PWM (Arduino D3, D5, D6) === */
#define TIM1_CH1_Pin         GPIO_PIN_9   /* PE9 = D6 */
#define TIM1_CH1_Port        GPIOE
#define TIM1_CH2_Pin         GPIO_PIN_11  /* PE11 = D5 */
#define TIM1_CH2_Port        GPIOE
#define TIM1_CH3_Pin         GPIO_PIN_13  /* PE13 = D3 */
#define TIM1_CH3_Port        GPIOE

/* === TIM4 PWM (Arduino D9, D10) === */
#define TIM4_CH3_Pin         GPIO_PIN_14  /* PD14 = D10 */
#define TIM4_CH3_Port        GPIOD
#define TIM4_CH4_Pin         GPIO_PIN_15  /* PD15 = D9 */
#define TIM4_CH4_Port        GPIOD

/* === TIM2 (PA0 = TIM2_CH1, Input Capture) === */
#define TIM2_CH1_Pin         GPIO_PIN_0
#define TIM2_CH1_Port        GPIOA

/* === Additional ADC Channels (Multi-channel) === */
#define ADC1_IN0_Pin         GPIO_PIN_0   /* PA0 - shared with TIM2 */
#define ADC1_IN0_Port        GPIOA
#define ADC1_IN1_Pin         GPIO_PIN_1   /* PA1 - shared with ETH_REF_CLK */
#define ADC1_IN1_Port        GPIOA
#define ADC1_IN10_Pin        GPIO_PIN_0   /* PC0 */
#define ADC1_IN10_Port       GPIOC
#define ADC1_IN11_Pin        GPIO_PIN_1   /* PC1 - shared with ETH_MDC */
#define ADC1_IN11_Port       GPIOC
#define ADC1_IN12_Pin        GPIO_PIN_2   /* PC2 */
#define ADC1_IN12_Port       GPIOC
#define ADC1_IN13_Pin        GPIO_PIN_3   /* PC3 */
#define ADC1_IN13_Port       GPIOC

/* ============================================================
 *              GENERAL PURPOSE GPIO PINS
 * ============================================================
 * Arduino Digital Pins D0-D10 (D11-D15 used by SPI/I2C)
 */

/* D0 = PG9 - UART RX compatible */
#define GPIO_D0_Pin          GPIO_PIN_9
#define GPIO_D0_Port         GPIOG

/* D1 = PG14 - UART TX compatible */
#define GPIO_D1_Pin          GPIO_PIN_14
#define GPIO_D1_Port         GPIOG

/* D2 = PF15 - General purpose */
#define GPIO_D2_Pin          GPIO_PIN_15
#define GPIO_D2_Port         GPIOF

/* D3 = PE13 - TIM1_CH3 PWM capable */
#define GPIO_D3_Pin          GPIO_PIN_13
#define GPIO_D3_Port         GPIOE

/* D4 = PF14 - General purpose */
#define GPIO_D4_Pin          GPIO_PIN_14
#define GPIO_D4_Port         GPIOF

/* D5 = PE11 - TIM1_CH2 PWM capable */
#define GPIO_D5_Pin          GPIO_PIN_11
#define GPIO_D5_Port         GPIOE

/* D6 = PE9 - TIM1_CH1 PWM capable */
#define GPIO_D6_Pin          GPIO_PIN_9
#define GPIO_D6_Port         GPIOE

/* D7 = PF13 - General purpose */
#define GPIO_D7_Pin          GPIO_PIN_13
#define GPIO_D7_Port         GPIOF

/* D8 = PF12 - General purpose */
#define GPIO_D8_Pin          GPIO_PIN_12
#define GPIO_D8_Port         GPIOF

/* D9 = PD15 - TIM4_CH4 PWM capable */
#define GPIO_D9_Pin          GPIO_PIN_15
#define GPIO_D9_Port         GPIOD

/* D10 = PD14 - TIM4_CH3 PWM capable, SPI CS */
#define GPIO_D10_Pin         GPIO_PIN_14
#define GPIO_D10_Port        GPIOD

/* ============================================================
 *              EXTRA MORPHO CONNECTOR GPIO
 * ============================================================
 */

/* PC0 - ADC123_IN10 or GPIO */
#define GPIO_PC0_Pin         GPIO_PIN_0
#define GPIO_PC0_Port        GPIOC

/* PC1 - ADC123_IN11 or GPIO */
#define GPIO_PC1_Pin         GPIO_PIN_1
#define GPIO_PC1_Port        GPIOC

/* PC2 - ADC123_IN12 or GPIO */
#define GPIO_PC2_Pin         GPIO_PIN_2
#define GPIO_PC2_Port        GPIOC

/* PC3 - ADC123_IN13 or GPIO */
#define GPIO_PC3_Pin         GPIO_PIN_3
#define GPIO_PC3_Port        GPIOC

/* PE2 - General purpose */
#define GPIO_PE2_Pin         GPIO_PIN_2
#define GPIO_PE2_Port        GPIOE

/* PE3 - General purpose */
#define GPIO_PE3_Pin         GPIO_PIN_3
#define GPIO_PE3_Port        GPIOE

/* PE4 - General purpose */
#define GPIO_PE4_Pin         GPIO_PIN_4
#define GPIO_PE4_Port        GPIOE

/* PE5 - General purpose / TIM9_CH1 */
#define GPIO_PE5_Pin         GPIO_PIN_5
#define GPIO_PE5_Port        GPIOE

/* PE6 - General purpose / TIM9_CH2 */
#define GPIO_PE6_Pin         GPIO_PIN_6
#define GPIO_PE6_Port        GPIOE

/* PF0 - I2C2_SDA or GPIO */
#define GPIO_PF0_Pin         GPIO_PIN_0
#define GPIO_PF0_Port        GPIOF

/* PF1 - I2C2_SCL or GPIO */
#define GPIO_PF1_Pin         GPIO_PIN_1
#define GPIO_PF1_Port        GPIOF

/* PF2 - General purpose */
#define GPIO_PF2_Pin         GPIO_PIN_2
#define GPIO_PF2_Port        GPIOF

/* ============================================================
 *              NUCLEO-F429ZI PIN MAPPING SUMMARY
 * ============================================================
 *
 * CONNECTOR CN7 (Arduino A0-A5):
 *   A0 = PA3  -> ADC1_IN3 (configured)
 *   A1 = PA4  -> DAC1_OUT1 (NEW!)
 *   A2 = PA5  -> SPI1_SCK (conflict - cannot use DAC2)
 *   A3 = PA6  -> SPI1_MISO
 *   A4 = PA7  -> SPI1_MOSI
 *   A5 = PB0  -> LED1 (Green)
 *
 * CONNECTOR CN10 (Arduino D0-D15):
 *   D0  = PG9  -> (available)
 *   D1  = PG14 -> (available)
 *   D2  = PF15 -> (available)
 *   D3  = PE13 -> (available, TIM1_CH3)
 *   D4  = PF14 -> (available)
 *   D5  = PE11 -> (available, TIM1_CH2)
 *   D6  = PE9  -> (available, TIM1_CH1)
 *   D7  = PF13 -> (available)
 *   D8  = PF12 -> (available)
 *   D9  = PD15 -> (available, TIM4_CH4)
 *   D10 = PD14 -> (available, TIM4_CH3)
 *   D11 = PA7  -> SPI1_MOSI
 *   D12 = PA6  -> SPI1_MISO
 *   D13 = PA5  -> SPI1_SCK
 *   D14 = PB9  -> I2C1_SDA
 *   D15 = PB8  -> I2C1_SCL
 *
 * MORPHO CONNECTOR CN11/CN12:
 *   PD0  -> CAN1_RX (NEW!)
 *   PD1  -> CAN1_TX (NEW!)
 *   PD8  -> USART3_TX
 *   PD9  -> USART3_RX
 *   PA11 -> USB_OTG_FS_DM (NEW!)
 *   PA12 -> USB_OTG_FS_DP (NEW!)
 *
 * INTERNAL:
 *   RNG  -> Hardware Random Number Generator (NEW!)
 *   RTC  -> Real-Time Clock with LSE 32.768kHz (NEW!)
 *   IWDG -> Independent Watchdog (NEW!)
 *
 * ============================================================
 *            DAC-to-ADC LOOPBACK TEST WIRING
 * ============================================================
 *
 *   Connect with a jumper wire:
 *     PA4 (DAC output) -----> PA3 (ADC input)
 *
 *   These are CN7 pins 1 and 2 (right next to each other!)
 *
 * ============================================================
 */

/* === Function Prototypes === */
void Error_Handler(void);
void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
