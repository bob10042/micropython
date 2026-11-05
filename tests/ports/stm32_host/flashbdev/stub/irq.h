#include <stdint.h>
#define IRQ_PRI_FLASH (6)
uint32_t raise_irq_pri(uint32_t pri);
void restore_irq_pri(uint32_t basepri);
uint32_t HAL_GetTick(void);
