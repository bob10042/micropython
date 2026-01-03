/**
  * STM32F4xx Interrupt Service Routines
  */
#include "main.h"

/* External handlers */
extern void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void NMI_Handler(void) { while(1) {} }
void HardFault_Handler(void) { while(1) {} }
void MemManage_Handler(void) { while(1) {} }
void BusFault_Handler(void) { while(1) {} }
void UsageFault_Handler(void) { while(1) {} }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_Pin);
}
