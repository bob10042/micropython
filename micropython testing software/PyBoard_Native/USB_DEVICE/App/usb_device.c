/**
  ******************************************************************************
  * @file    usb_device.c
  * @brief   USB Device initialization
  ******************************************************************************
  */

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

#define DEVICE_FS 0

USBD_HandleTypeDef hUsbDeviceFS;

/**
  * @brief  Initialize the USB Device
  */
void MX_USB_DEVICE_Init(void)
{
    /* Initialize Device Library */
    if (USBD_Init(&hUsbDeviceFS, &CDC_Desc, DEVICE_FS) != USBD_OK)
    {
        Error_Handler();
    }
    
    /* Register CDC Class */
    if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK)
    {
        Error_Handler();
    }
    
    /* Register CDC Interface callbacks */
    if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
    {
        Error_Handler();
    }
    
    /* Start USB Device */
    if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
    {
        Error_Handler();
    }
}
