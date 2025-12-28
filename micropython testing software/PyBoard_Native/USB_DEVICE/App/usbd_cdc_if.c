/**
  ******************************************************************************
  * @file    usbd_cdc_if.c
  * @brief   USB CDC Interface - Connects USB CDC to CLI
  ******************************************************************************
  */

#include "usbd_cdc_if.h"

/* Receive buffer */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USB Device handle */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Receive callback - will be set by main */
static void (*CDC_RxCallback)(uint8_t *buf, uint32_t len) = NULL;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
    CDC_Init_FS,
    CDC_DeInit_FS,
    CDC_Control_FS,
    CDC_Receive_FS,
    CDC_TransmitCplt_FS
};

static int8_t CDC_Init_FS(void)
{
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
    return USBD_OK;
}

static int8_t CDC_DeInit_FS(void)
{
    return USBD_OK;
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
    (void)pbuf;
    (void)length;
    
    switch (cmd)
    {
        case CDC_SEND_ENCAPSULATED_COMMAND:
        case CDC_GET_ENCAPSULATED_RESPONSE:
        case CDC_SET_COMM_FEATURE:
        case CDC_GET_COMM_FEATURE:
        case CDC_CLEAR_COMM_FEATURE:
        case CDC_SET_LINE_CODING:
        case CDC_GET_LINE_CODING:
        case CDC_SET_CONTROL_LINE_STATE:
        case CDC_SEND_BREAK:
        default:
            break;
    }
    return USBD_OK;
}

static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len)
{
    if (CDC_RxCallback != NULL)
    {
        CDC_RxCallback(Buf, *Len);
    }
    
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return USBD_OK;
}

static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
    (void)Buf;
    (void)Len;
    (void)epnum;
    return USBD_OK;
}

/**
  * @brief  CDC_Transmit_FS
  *         Transmit data over USB CDC
  * @param  Buf: Buffer of data to send
  * @param  Len: Number of bytes to send
  * @retval USBD_OK on success
  */
uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len)
{
    uint8_t result = USBD_OK;
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData;
    
    if (hcdc->TxState != 0)
    {
        return USBD_BUSY;
    }
    
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
    result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    return result;
}

/**
  * @brief  Set the receive callback function
  * @param  callback: Function to call when data is received
  */
void CDC_SetRxCallback(void (*callback)(uint8_t *buf, uint32_t len))
{
    CDC_RxCallback = callback;
}

/**
  * @brief  Transmit a null-terminated string over CDC
  * @param  str: String to transmit
  * @retval USBD_OK on success
  */
uint8_t CDC_Transmit_String(const char *str)
{
    return CDC_Transmit_FS((uint8_t *)str, strlen(str));
}
