/**
  ******************************************************************************
  * @file    usb_device.h
  * @brief   USB Device initialization header
  ******************************************************************************
  */

#ifndef __USB_DEVICE_H
#define __USB_DEVICE_H

#include "stm32f4xx_hal.h"
#include "usbd_def.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

void MX_USB_DEVICE_Init(void);
void Error_Handler(void);

#endif /* __USB_DEVICE_H */
