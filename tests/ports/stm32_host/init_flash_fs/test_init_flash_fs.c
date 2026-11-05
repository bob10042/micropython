// Host harness for init_flash_fs() in ports/stm32/main.c (AUD-014).
// The real code is included from init_flash_fs.inc, which run.sh cuts out of
// main.c.  The flash filesystem region is modelled as 224 blocks of 512 bytes
// (PYBV11: sectors 1 to 4, 112 KB), seen through storage_read_blocks() at
// FLASH_PART1_START_BLOCK as on the board.  Mounting succeeds when block 0
// holds a boot-sector signature; "formatting" writes one and is counted.
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Stubs for what the extracted block uses.
typedef unsigned int uint;
typedef void *mp_obj_t;
#define MP_NOINLINE __attribute__((noinline))
#define MP_OBJ_FROM_PTR(p) ((mp_obj_t)(p))
#define MP_OBJ_NEW_QSTR(q) ((mp_obj_t)(uintptr_t)(q))
#define MP_QSTR__slash_flash (1)
#define MP_ENODEV (19)
#define MICROPY_VFS_LFS1 (0)
#define MICROPY_VFS_LFS2 (0)
#define MICROPY_HW_FLASH_MOUNT_AT_BOOT (1)
#define MICROPY_HW_FLASH_FS_FORMAT_IF_UNREADABLE (0) // default, mpconfigboard_common.h
#define BOARDCTRL_RESET_MODE_NORMAL (1)
#define BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM (3)
#define FLASH_BLOCK_SIZE (512)
#define FLASH_PART1_START_BLOCK (0x100)
#define mp_plat_print (NULL)
#define mp_printf(print, ...) (++printed, printf(__VA_ARGS__))

#define FS_BLOCKS (224)
static uint8_t fs[FS_BLOCKS][FLASH_BLOCK_SIZE];
static int printed, format_calls, fail_alloc;
static const int pyb_flash_obj = 0;

#define m_new_maybe(type, num) (fail_alloc ? NULL : (type *)malloc(sizeof(type) * (num)))
#define m_del(type, ptr, num) free(ptr)

// (unused in trees without the AUD-014 probe)
__attribute__((unused)) static uint32_t storage_get_block_count(void) {
    return FLASH_PART1_START_BLOCK + FS_BLOCKS;
}

__attribute__((unused)) static int storage_read_blocks(uint8_t *dest, uint32_t block, uint32_t num) {
    if (block < FLASH_PART1_START_BLOCK || block + num > FLASH_PART1_START_BLOCK + FS_BLOCKS) {
        return -1;
    }
    memcpy(dest, fs[block - FLASH_PART1_START_BLOCK], num * FLASH_BLOCK_SIZE);
    return 0;
}

static bool has_signature(void) {
    return fs[0][510] == 0x55 && fs[0][511] == 0xaa;
}

static int mp_vfs_mount_and_chdir_protected(mp_obj_t bdev, mp_obj_t mount_point) {
    return has_signature() ? 0 : -MP_ENODEV;
}

static int factory_reset_create_filesystem(void) {
    ++format_calls;
    memset(fs, 0, sizeof(fs));
    fs[0][510] = 0x55;
    fs[0][511] = 0xaa;
    return 0;
}

#include "init_flash_fs.inc"

static int failures;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d: %s\n", __func__, __LINE__, #c); ++failures; } } while (0)

static void setup(void) {
    memset(fs, 0xff, sizeof(fs));
    printed = format_calls = fail_alloc = 0;
}

int main(void) {
    bool mounted;

    // 1. Never written (all 0xff): first-boot provisioning formats and mounts.
    setup();
    mounted = init_flash_fs(BOARDCTRL_RESET_MODE_NORMAL);
    CHECK(format_calls == 1 && mounted);

    // 2. Sector 1 (blocks 0-31: boot sector, FATs, root dir) erased by an
    //    interrupted write-back, files still in sector 2: must not format.
    setup();
    memset(fs[40], 0x5a, FLASH_BLOCK_SIZE);
    mounted = init_flash_fs(BOARDCTRL_RESET_MODE_NORMAL);
    CHECK(format_calls == 0 && !mounted && printed);
    CHECK(fs[40][0] == 0x5a);

    // 3. Only one byte of the last block written: the whole region is checked.
    setup();
    fs[FS_BLOCKS - 1][FLASH_BLOCK_SIZE - 1] = 0x00;
    mounted = init_flash_fs(BOARDCTRL_RESET_MODE_NORMAL);
    CHECK(format_calls == 0 && !mounted);

    // 4. Garbage in block 0 (no signature): must not format.
    setup();
    memset(fs[0], 0x00, FLASH_BLOCK_SIZE);
    mounted = init_flash_fs(BOARDCTRL_RESET_MODE_NORMAL);
    CHECK(format_calls == 0 && !mounted);

    // 5. No memory for the probe buffer: must not format a non-blank flash.
    setup();
    memset(fs[40], 0x5a, FLASH_BLOCK_SIZE);
    fail_alloc = 1;
    mounted = init_flash_fs(BOARDCTRL_RESET_MODE_NORMAL);
    CHECK(format_calls == 0 && !mounted);

    // 6. A good filesystem mounts and is left alone.
    setup();
    factory_reset_create_filesystem();
    format_calls = 0;
    mounted = init_flash_fs(BOARDCTRL_RESET_MODE_NORMAL);
    CHECK(format_calls == 0 && mounted && !printed);

    // 7. The factory-reset button sequence still formats a damaged filesystem.
    setup();
    memset(fs[40], 0x5a, FLASH_BLOCK_SIZE);
    mounted = init_flash_fs(BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM);
    CHECK(format_calls == 1 && mounted);

    printf("init_flash_fs: %s\n", failures ? "FAILED" : "OK");
    return failures ? 1 : 0;
}