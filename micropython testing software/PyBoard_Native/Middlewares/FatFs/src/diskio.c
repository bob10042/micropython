/*-----------------------------------------------------------------------*/
/* Low level disk I/O module for FatFs - STM32 SDIO Implementation       */
/* Connects FatFs to STM32 HAL SD driver                                 */
/* Uses oofatfs API (void* drv instead of BYTE pdrv)                     */
/*-----------------------------------------------------------------------*/

#include "ff.h"
#include "diskio.h"
#include "stm32f4xx_hal.h"

/* External SD handle from main.c */
extern SD_HandleTypeDef hsd;

/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Initialize a Drive (called via disk_ioctl with IOCTL_INIT)            */
/*-----------------------------------------------------------------------*/
static DSTATUS sd_disk_init(void)
{
    /* Check if card is present */
    if (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
    {
        /* Try to initialize */
        if (HAL_SD_Init(&hsd) != HAL_OK)
        {
            Stat = STA_NOINIT;
            return Stat;
        }
    }
    
    Stat = 0;  /* Clear STA_NOINIT */
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(
    void *drv,      /* Physical drive (not used, we only have one) */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Start sector number (LBA) */
    UINT count      /* Number of sectors to read */
)
{
    (void)drv;  /* Unused */
    
    if (count == 0) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    
    /* Read sectors using HAL */
    if (HAL_SD_ReadBlocks(&hsd, buff, sector, count, 5000) != HAL_OK)
    {
        return RES_ERROR;
    }
    
    /* Wait for card to be ready */
    uint32_t timeout = HAL_GetTick() + 1000;
    while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
    {
        if (HAL_GetTick() > timeout) return RES_ERROR;
    }
    
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT disk_write(
    void *drv,          /* Physical drive (not used) */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Start sector number (LBA) */
    UINT count          /* Number of sectors to write */
)
{
    (void)drv;  /* Unused */
    
    if (count == 0) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    
    /* Write sectors using HAL */
    if (HAL_SD_WriteBlocks(&hsd, (uint8_t*)buff, sector, count, 5000) != HAL_OK)
    {
        return RES_ERROR;
    }
    
    /* Wait for card to be ready */
    uint32_t timeout = HAL_GetTick() + 1000;
    while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
    {
        if (HAL_GetTick() > timeout) return RES_ERROR;
    }
    
    return RES_OK;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(
    void *drv,      /* Physical drive (not used) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res = RES_ERROR;
    HAL_SD_CardInfoTypeDef cardInfo;
    
    (void)drv;  /* Unused */
    
    switch (cmd)
    {
        case IOCTL_INIT:
            /* Initialize disk - always allowed even if not yet initialized */
            if (sd_disk_init() == 0) {
                *(DSTATUS*)buff = Stat;
                res = RES_OK;
            } else {
                *(DSTATUS*)buff = STA_NOINIT;
                res = RES_OK;  /* Return OK, status in buff indicates init failed */
            }
            return res;
            
        case IOCTL_STATUS:
            /* Get disk status - always allowed */
            *(DSTATUS*)buff = Stat;
            return RES_OK;
            
        default:
            /* All other commands require initialized disk */
            break;
    }
    
    /* Check if disk is initialized for remaining commands */
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    
    switch (cmd)
    {
        case CTRL_SYNC:
            /* Make sure all data is written */
            if (HAL_SD_GetCardState(&hsd) == HAL_SD_CARD_TRANSFER)
            {
                res = RES_OK;
            }
            break;
            
        case GET_SECTOR_COUNT:
            /* Get number of sectors on the disk */
            if (HAL_SD_GetCardInfo(&hsd, &cardInfo) == HAL_OK)
            {
                *(DWORD*)buff = cardInfo.BlockNbr;
                res = RES_OK;
            }
            break;
            
        case GET_SECTOR_SIZE:
            /* Get sector size (always 512 for SD) */
            *(WORD*)buff = 512;
            res = RES_OK;
            break;
            
        case GET_BLOCK_SIZE:
            /* Get erase block size in sectors */
            if (HAL_SD_GetCardInfo(&hsd, &cardInfo) == HAL_OK)
            {
                *(DWORD*)buff = cardInfo.BlockSize / 512;
                res = RES_OK;
            }
            break;
            
        default:
            res = RES_PARERR;
    }
    
    return res;
}

/*-----------------------------------------------------------------------*/
/* Get current time for FAT timestamp                                    */
/*-----------------------------------------------------------------------*/
DWORD get_fattime(void)
{
    /* Return fixed date/time: 2025-12-28 12:00:00 */
    /* Format: bit[31:25]=Year-1980, [24:21]=Month, [20:16]=Day, 
               [15:11]=Hour, [10:5]=Min, [4:0]=Sec/2 */
    return ((2025 - 1980) << 25) | (12 << 21) | (28 << 16) |
           (12 << 11) | (0 << 5) | (0);
}
