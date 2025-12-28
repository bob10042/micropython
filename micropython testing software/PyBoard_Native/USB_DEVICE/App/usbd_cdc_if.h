/**
  ******************************************************************************
  * @file    usbd_cdc_if.h
  * @brief   Header for USB CDC Interface
  ******************************************************************************
  */

#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

#include "usbd_cdc.h"

#define APP_RX_DATA_SIZE  512
#define APP_TX_DATA_SIZE  512

extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);
uint8_t CDC_Transmit_String(const char *str);
void CDC_SetRxCallback(void (*callback)(uint8_t *buf, uint32_t len));

#endif /* __USBD_CDC_IF_H */
