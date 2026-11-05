// Host stub of ports/stm32/storage.h: the parts flashbdev.c uses.
#include <stdbool.h>
#include <stdint.h>
#define FLASH_BLOCK_SIZE (512)
enum {
    BDEV_IOCTL_INIT = 1,
    BDEV_IOCTL_SYNC = 3,
    BDEV_IOCTL_NUM_BLOCKS = 4,
    BDEV_IOCTL_IRQ_HANDLER = 7,
};
int32_t flash_bdev_ioctl(uint32_t op, uint32_t arg);
bool flash_bdev_readblock(uint8_t *dest, uint32_t block);
bool flash_bdev_writeblock(const uint8_t *src, uint32_t block);
int flash_bdev_readblocks_ext(uint8_t *dest, uint32_t block, uint32_t offset, uint32_t len);
int flash_bdev_writeblocks_ext(const uint8_t *src, uint32_t block, uint32_t offset, uint32_t len);
