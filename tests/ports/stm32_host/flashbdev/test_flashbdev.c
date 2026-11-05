// Host harness for ports/stm32/flashbdev.c: see run.sh.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "storage.h"
#include "flash.h"
#include "irq.h"
#include "led.h"

#define FLASH_BASE (0x08000000)
#define FLASH_SIZE (1024 * 1024)
#define CACHE_BASE (0x10000000)
#define CACHE_SIZE (64 * 1024)
#define FS_START (0x08004000) // filesystem block 0 = start of sector 1
#define BLK(n) ((uint8_t *)(uintptr_t)(FS_START + (n) * FLASH_BLOCK_SIZE))

/******************************************************************************/
// Fake HAL

static uint32_t fake_tick;
static int fake_led_red;
static int fake_erase_err;
static int fake_write_err;
static int fake_erase_calls;
static int fake_write_calls;

uint32_t HAL_GetTick(void) {
    return fake_tick;
}

uint32_t raise_irq_pri(uint32_t pri) {
    return 0;
}

void restore_irq_pri(uint32_t basepri) {
}

void led_state(int led, int state) {
    if (led == PYB_LED_RED) {
        fake_led_red = state;
    }
}

// STM32F405: 4 x 16K, 1 x 64K, 7 x 128K from 0x08000000 (flash.c flash_layout).
int32_t flash_get_sector_info(uint32_t addr, uint32_t *start_addr, uint32_t *size) {
    static const struct { uint32_t base, size, count; } layout[] = {
        { 0x08000000, 0x4000, 4 },
        { 0x08010000, 0x10000, 1 },
        { 0x08020000, 0x20000, 7 },
    };
    int32_t index = 0;
    for (int i = 0; i < 3; ++i) {
        for (uint32_t j = 0; j < layout[i].count; ++j, ++index) {
            uint32_t start = layout[i].base + j * layout[i].size;
            if (start <= addr && addr < start + layout[i].size) {
                if (start_addr != NULL) {
                    *start_addr = start;
                }
                if (size != NULL) {
                    *size = layout[i].size;
                }
                return index;
            }
        }
    }
    return -1; // outputs not set, as in flash.c
}

int flash_erase(uint32_t flash_dest) {
    uint32_t start, size;
    ++fake_erase_calls;
    if (flash_get_sector_info(flash_dest, &start, &size) < 0) {
        printf("FAIL: erase of address 0x%08x outside flash\n", (unsigned)flash_dest);
        exit(1);
    }
    if (fake_erase_err) {
        return fake_erase_err;
    }
    memset((void *)(uintptr_t)start, 0xff, size);
    return 0;
}

int flash_write(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32) {
    ++fake_write_calls;
    if (fake_write_err) {
        // a failed program leaves part of the data behind
        num_word32 /= 2;
    }
    uint32_t *dest = (uint32_t *)(uintptr_t)flash_dest;
    for (uint32_t i = 0; i < num_word32; ++i) {
        dest[i] &= src[i]; // NOR flash: programming can only clear bits
    }
    return fake_write_err;
}

/******************************************************************************/
// Helpers

static int failures;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __func__, __LINE__, #cond); ++failures; } } while (0)

static void fake_reset(void) {
    // flash: everything holds 0x11 ("old data"), cache RAM holds junk
    memset((void *)(uintptr_t)FLASH_BASE, 0x11, FLASH_SIZE);
    memset((void *)(uintptr_t)CACHE_BASE, 0xa5, CACHE_SIZE);
    fake_tick = 1000;
    fake_led_red = 0;
    fake_erase_err = fake_write_err = 0;
    fake_erase_calls = fake_write_calls = 0;
    flash_bdev_ioctl(BDEV_IOCTL_INIT, 0);
}

static int mem_is(const uint8_t *p, uint8_t val, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (p[i] != val) {
            return 0;
        }
    }
    return 1;
}

static void irq_at(uint32_t tick) {
    fake_tick = tick;
    flash_bdev_ioctl(BDEV_IOCTL_IRQ_HANDLER, 0);
}

static uint8_t block_buf[FLASH_BLOCK_SIZE];

static int write_block(uint32_t block, uint8_t val) {
    memset(block_buf, val, sizeof(block_buf));
    return flash_bdev_writeblock(block_buf, block);
}

/******************************************************************************/
// AUD-022: a failed erase or write must be reported and keep the cache dirty.

static void test_aud022(void) {
    // A write that fails during a forced sync.
    fake_reset();
    CHECK(write_block(0, 0x22));
    fake_write_err = -5;
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) < 0);
    CHECK(fake_led_red == 1); // still dirty
    fake_write_err = 0;
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) == 0); // retried
    CHECK(mem_is(BLK(0), 0x22, FLASH_BLOCK_SIZE));
    CHECK(mem_is(BLK(1), 0x11, FLASH_BLOCK_SIZE));
    CHECK(fake_led_red == 0);

    // An erase that fails: nothing may be programmed over the unerased sector.
    fake_reset();
    CHECK(write_block(0, 0x22));
    fake_erase_err = -5;
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) < 0);
    CHECK(fake_write_calls == 0);
    CHECK(fake_led_red == 1);
    fake_erase_err = 0;
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) == 0);
    CHECK(mem_is(BLK(0), 0x22, FLASH_BLOCK_SIZE));
    CHECK(mem_is(BLK(1), 0x11, FLASH_BLOCK_SIZE));

    // A write to another sector forces the cached one out; if that fails the
    // cached sector must be kept and the new write refused.
    fake_reset();
    CHECK(write_block(0, 0x22)); // sector 1
    fake_write_err = -5;
    CHECK(!write_block(32, 0x33)); // block 32 is in sector 2
    fake_write_err = 0;
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) == 0);
    CHECK(mem_is(BLK(0), 0x22, FLASH_BLOCK_SIZE));
    CHECK(mem_is(BLK(32), 0x11, FLASH_BLOCK_SIZE));
    CHECK(write_block(32, 0x33));
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) == 0);
    CHECK(mem_is(BLK(0), 0x22, FLASH_BLOCK_SIZE));
    CHECK(mem_is(BLK(32), 0x33, FLASH_BLOCK_SIZE));

    // A failed write-back from the idle handler is retried later, not dropped.
    fake_reset();
    CHECK(write_block(0, 0x22));
    fake_write_err = -5;
    for (uint32_t t = 1512; t < 12000; t += 512) {
        irq_at(t);
    }
    CHECK(fake_led_red == 1);
    fake_write_err = 0;
    for (uint32_t t = 12000; t < 30000; t += 512) {
        irq_at(t);
    }
    CHECK(fake_led_red == 0);
    CHECK(mem_is(BLK(0), 0x22, FLASH_BLOCK_SIZE));
    CHECK(mem_is(BLK(1), 0x11, FLASH_BLOCK_SIZE));
}

/******************************************************************************/
// AUD-017: the offset of the extended block protocol must stay inside the
// block device; it must never reach the firmware in sector 5 (0x08020000).

#define SECTOR5 ((uint8_t *)(uintptr_t)0x08020000)

static void test_aud017(void) {
    static uint8_t buf[32];

    // Offset that points at the firmware: refused, nothing cached or erased.
    fake_reset();
    memset(buf, 0x55, sizeof(buf));
    CHECK(flash_bdev_writeblocks_ext(buf, 0, 0x1C000, 16) < 0);
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) == 0);
    CHECK(fake_erase_calls == 0);
    CHECK(mem_is(SECTOR5, 0x11, 0x20000));
    CHECK(flash_bdev_readblocks_ext(buf, 0, 0x1C000, 16) < 0);

    // Offset of exactly one block is refused too (it is not inside block 0).
    CHECK(flash_bdev_writeblocks_ext(buf, 0, FLASH_BLOCK_SIZE, 16) < 0);
    CHECK(flash_bdev_readblocks_ext(buf, 0, FLASH_BLOCK_SIZE, 16) < 0);
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) == 0);
    CHECK(mem_is(BLK(0), 0x11, 2 * FLASH_BLOCK_SIZE));

    // A write that starts inside a block and runs into the next still works.
    fake_reset();
    memset(buf, 0x44, 30);
    CHECK(flash_bdev_writeblocks_ext(buf, 0, 500, 30) == 0);
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) == 0);
    CHECK(mem_is(BLK(0), 0x11, 500));
    CHECK(mem_is(BLK(0) + 500, 0x44, 30));
    CHECK(mem_is(BLK(0) + 530, 0x11, FLASH_BLOCK_SIZE));
    memset(buf, 0, sizeof(buf));
    CHECK(flash_bdev_readblocks_ext(buf, 0, 500, 30) == 0);
    CHECK(mem_is(buf, 0x44, 30));
}

/******************************************************************************/
// AUD-005: the cached sector must not be erased until it is written back, so
// a reset while the data is only in the RAM cache leaves the old data intact.

static void test_aud005(void) {
    fake_reset();
    CHECK(write_block(0, 0x22)); // at tick 1000
    // idle ticks during the 5 s write-back delay: flash keeps the old data
    for (uint32_t t = 1512; t < 1000 + 5000; t += 512) {
        irq_at(t);
        CHECK(mem_is(BLK(0), 0x11, FLASH_BLOCK_SIZE));
        CHECK(mem_is(BLK(31), 0x11, FLASH_BLOCK_SIZE)); // rest of sector 1
    }
    CHECK(fake_erase_calls == 0);
    CHECK(fake_led_red == 1);
    // the write-back is due: erase and write happen together
    irq_at(6100);
    CHECK(fake_erase_calls == 1);
    CHECK(fake_write_calls == 1);
    CHECK(mem_is(BLK(0), 0x22, FLASH_BLOCK_SIZE));
    CHECK(mem_is(BLK(1), 0x11, 31 * FLASH_BLOCK_SIZE));
    CHECK(fake_led_red == 0);

    // A script that keeps writing to the sector keeps the last good copy on flash.
    for (uint32_t t = 7000; t < 20000; t += 512) {
        fake_tick = t;
        CHECK(write_block(1, 0x33));
        irq_at(t + 256);
        CHECK(mem_is(BLK(0), 0x22, FLASH_BLOCK_SIZE));
        CHECK(mem_is(BLK(1), 0x11, FLASH_BLOCK_SIZE));
    }

    // A forced sync writes everything at once.
    CHECK(flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0) == 0);
    CHECK(mem_is(BLK(0), 0x22, FLASH_BLOCK_SIZE));
    CHECK(mem_is(BLK(1), 0x33, FLASH_BLOCK_SIZE));
    CHECK(mem_is(BLK(2), 0x11, 30 * FLASH_BLOCK_SIZE));
    CHECK(fake_led_red == 0);
}

/******************************************************************************/

static const struct {
    const char *name;
    void (*fun)(void);
} tests[] = {
    { "aud022", test_aud022 },
    { "aud017", test_aud017 },
    { "aud005", test_aud005 },
};

int main(int argc, char **argv) {
    if (mmap((void *)FLASH_BASE, FLASH_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0) != (void *)FLASH_BASE
        || mmap((void *)CACHE_BASE, CACHE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0) != (void *)CACHE_BASE) {
        printf("SKIP: cannot map fake flash/cache\n");
        return 2;
    }
    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        int run = argc < 2;
        for (int j = 1; j < argc; ++j) {
            run |= strcmp(argv[j], tests[i].name) == 0;
        }
        if (run) {
            int before = failures;
            tests[i].fun();
            printf("%s: %s\n", tests[i].name, failures == before ? "OK" : "FAILED");
        }
    }
    return failures ? 1 : 0;
}
