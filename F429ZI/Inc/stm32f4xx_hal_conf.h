/**
  * STM32F4xx HAL Configuration - FULL Peripheral Support
  * Updated with DAC, RTC, CAN, IWDG, RNG, and USB modules
  */
#ifndef __STM32F4xx_HAL_CONF_H
#define __STM32F4xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================= Module Selection ========================= */
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CAN_MODULE_ENABLED        /* CAN bus */
#define HAL_DAC_MODULE_ENABLED        /* Digital-to-Analog */
#define HAL_DMA_MODULE_ENABLED
#define HAL_ETH_MODULE_ENABLED        /* Ethernet */
#define HAL_GPIO_MODULE_ENABLED
#define HAL_HCD_MODULE_ENABLED        /* USB Host */
#define HAL_I2C_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED       /* Independent Watchdog */
#define HAL_PCD_MODULE_ENABLED        /* USB Device */
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_RNG_MODULE_ENABLED        /* Random Number Generator */
#define HAL_RTC_MODULE_ENABLED        /* Real-Time Clock */
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_WWDG_MODULE_ENABLED       /* Window Watchdog */

/* ========================= Oscillator Values ========================= */
#if !defined(HSE_VALUE)
#define HSE_VALUE    8000000U  /* 8 MHz external crystal */
#endif

#if !defined(HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT    100U
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE    16000000U
#endif

#if !defined(LSI_VALUE)
#define LSI_VALUE    32000U
#endif

#if !defined(LSE_VALUE)
#define LSE_VALUE    32768U    /* 32.768 kHz for RTC */
#endif

#if !defined(LSE_STARTUP_TIMEOUT)
#define LSE_STARTUP_TIMEOUT    5000U
#endif

#if !defined(EXTERNAL_CLOCK_VALUE)
#define EXTERNAL_CLOCK_VALUE    12288000U
#endif

/* ========================= System Configuration ========================= */
#define VDD_VALUE                    3300U
#define TICK_INT_PRIORITY            0x0FU
#define USE_RTOS                     0U
#define PREFETCH_ENABLE              1U
#define INSTRUCTION_CACHE_ENABLE     1U
#define DATA_CACHE_ENABLE            1U

/* ========================= Ethernet Configuration ========================= */
#define ETH_TX_DESC_CNT              4U
#define ETH_RX_DESC_CNT              4U
#define ETH_MAX_PACKET_SIZE          1524U
#define ETH_MAC_ADDR0                0x02U
#define ETH_MAC_ADDR1                0x00U
#define ETH_MAC_ADDR2                0x00U
#define ETH_MAC_ADDR3                0x00U
#define ETH_MAC_ADDR4                0x00U
#define ETH_MAC_ADDR5                0x00U

/* LAN8742 PHY Register Definitions */
#define PHY_BCR                      0x00U    /* Basic Control Register */
#define PHY_BSR                      0x01U    /* Basic Status Register */
#define PHY_SR                       0x1FU    /* Special Status Register (LAN8742) */

#define PHY_RESET                    0x8000U  /* PHY Reset */
#define PHY_AUTONEGOTIATION          0x1000U  /* Enable auto-negotiation */
#define PHY_LINKED_STATUS            0x0004U  /* Link Status bit in BSR */
#define PHY_AUTONEGO_COMPLETE        0x0020U  /* Auto-negotiation Complete bit in BSR */
#define PHY_DUPLEX_STATUS            0x0010U  /* Full Duplex status (LAN8742 SR) */
#define PHY_SPEED_STATUS             0x0004U  /* 10Mbps status (LAN8742 SR) */

#define PHY_RESET_DELAY              500U     /* PHY reset delay in ms */
#define PHY_CONFIG_DELAY             500U     /* PHY config delay in ms */
#define PHY_READ_TO                  1000U    /* PHY read timeout in ms */
#define PHY_WRITE_TO                 1000U    /* PHY write timeout in ms */

/* ========================= Assert Selection ========================= */
/* #define USE_FULL_ASSERT    1U */

/* ========================= Include Files ========================= */
#ifdef HAL_RCC_MODULE_ENABLED
#include "stm32f4xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#include "stm32f4xx_hal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
#include "stm32f4xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
#include "stm32f4xx_hal_cortex.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
#include "stm32f4xx_hal_adc.h"
#endif

#ifdef HAL_CAN_MODULE_ENABLED
#include "stm32f4xx_hal_can.h"
#endif

#ifdef HAL_DAC_MODULE_ENABLED
#include "stm32f4xx_hal_dac.h"
#endif

#ifdef HAL_ETH_MODULE_ENABLED
#include "stm32f4xx_hal_eth.h"
#endif

#ifdef HAL_HCD_MODULE_ENABLED
#include "stm32f4xx_hal_hcd.h"
#endif

#ifdef HAL_I2C_MODULE_ENABLED
#include "stm32f4xx_hal_i2c.h"
#endif

#ifdef HAL_IWDG_MODULE_ENABLED
#include "stm32f4xx_hal_iwdg.h"
#endif

#ifdef HAL_PCD_MODULE_ENABLED
#include "stm32f4xx_hal_pcd.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
#include "stm32f4xx_hal_pwr.h"
#endif

#ifdef HAL_RNG_MODULE_ENABLED
#include "stm32f4xx_hal_rng.h"
#endif

#ifdef HAL_RTC_MODULE_ENABLED
#include "stm32f4xx_hal_rtc.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED
#include "stm32f4xx_hal_spi.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
#include "stm32f4xx_hal_tim.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
#include "stm32f4xx_hal_uart.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
#include "stm32f4xx_hal_flash.h"
#endif

#ifdef HAL_WWDG_MODULE_ENABLED
#include "stm32f4xx_hal_wwdg.h"
#endif

#ifdef USE_FULL_ASSERT
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_CONF_H */
