/**
  * MMA7660 Accelerometer Driver Implementation
  */

#include "mma7660.h"

/**
  * @brief  Initialize the MMA7660 accelerometer
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
HAL_StatusTypeDef MMA7660_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t data;
    HAL_StatusTypeDef status;

    /* Put device in standby mode first */
    data = 0x00;
    status = HAL_I2C_Mem_Write(hi2c, MMA7660_I2C_ADDR, MMA7660_MODE, 1, &data, 1, 100);
    if (status != HAL_OK) return status;

    /* Set sample rate to 120 samples/sec */
    data = 0x00;
    status = HAL_I2C_Mem_Write(hi2c, MMA7660_I2C_ADDR, MMA7660_SR, 1, &data, 1, 100);
    if (status != HAL_OK) return status;

    /* Enable active mode */
    data = 0x01;
    status = HAL_I2C_Mem_Write(hi2c, MMA7660_I2C_ADDR, MMA7660_MODE, 1, &data, 1, 100);

    HAL_Delay(10);  /* Wait for device to stabilize */

    return status;
}

/**
  * @brief  Read X, Y, Z acceleration values
  * @param  hi2c: I2C handle
  * @param  x, y, z: Pointers to store acceleration values (-32 to +31)
  * @retval HAL status
  */
HAL_StatusTypeDef MMA7660_ReadXYZ(I2C_HandleTypeDef *hi2c, int8_t *x, int8_t *y, int8_t *z)
{
    uint8_t data[3];
    HAL_StatusTypeDef status;
    int retries = 10;

    do {
        status = HAL_I2C_Mem_Read(hi2c, MMA7660_I2C_ADDR, MMA7660_XOUT, 1, data, 3, 100);
        if (status != HAL_OK) return status;

        /* Check alert bit (bit 6) - if set, data is invalid */
        if (!(data[0] & 0x40) && !(data[1] & 0x40) && !(data[2] & 0x40))
            break;

        retries--;
    } while (retries > 0);

    /* Convert 6-bit signed values to 8-bit signed */
    *x = (data[0] & 0x20) ? (data[0] | 0xC0) : (data[0] & 0x3F);
    *y = (data[1] & 0x20) ? (data[1] | 0xC0) : (data[1] & 0x3F);
    *z = (data[2] & 0x20) ? (data[2] | 0xC0) : (data[2] & 0x3F);

    return HAL_OK;
}

/**
  * @brief  Read tilt status
  * @param  hi2c: I2C handle
  * @param  tilt: Pointer to store tilt value
  * @retval HAL status
  */
HAL_StatusTypeDef MMA7660_ReadTilt(I2C_HandleTypeDef *hi2c, uint8_t *tilt)
{
    return HAL_I2C_Mem_Read(hi2c, MMA7660_I2C_ADDR, MMA7660_TILT, 1, tilt, 1, 100);
}
