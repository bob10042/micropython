/**
  * MMA7660 Accelerometer Driver
  */

#ifndef __MMA7660_H
#define __MMA7660_H

#include "stm32f4xx_hal.h"

/* MMA7660 I2C Address */
#define MMA7660_I2C_ADDR    (0x4C << 1)

/* MMA7660 Registers */
#define MMA7660_XOUT        0x00
#define MMA7660_YOUT        0x01
#define MMA7660_ZOUT        0x02
#define MMA7660_TILT        0x03
#define MMA7660_SRST        0x04
#define MMA7660_SPCNT       0x05
#define MMA7660_INTSU       0x06
#define MMA7660_MODE        0x07
#define MMA7660_SR          0x08
#define MMA7660_PDET        0x09
#define MMA7660_PD          0x0A

/* Function Prototypes */
HAL_StatusTypeDef MMA7660_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef MMA7660_ReadXYZ(I2C_HandleTypeDef *hi2c, int8_t *x, int8_t *y, int8_t *z);
HAL_StatusTypeDef MMA7660_ReadTilt(I2C_HandleTypeDef *hi2c, uint8_t *tilt);

#endif /* __MMA7660_H */
