#!/bin/sh
# Host harness for the boot-time flash filesystem setup in ports/stm32/main.c
# (AUD-014).  Takes the real MICROPY_HW_FLASH_MOUNT_AT_BOOT block (init_flash_fs
# and its helpers) out of main.c, builds it against stubs over a RAM model of
# the PYBV11 filesystem region (224 blocks of 512 bytes) and checks when it
# formats.  Usage: tests/ports/stm32_host/init_flash_fs/run.sh
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TOP=$HERE/../../../..
OUT=$(mktemp -d)
awk '/^#if MICROPY_HW_FLASH_MOUNT_AT_BOOT/{p=1} /^#if MICROPY_HW_SDCARD_MOUNT_AT_BOOT/{p=0} p'     "$TOP/ports/stm32/main.c" > "$OUT/init_flash_fs.inc"
${CC:-gcc} -std=gnu99 -Wall -I"$OUT" -o "$OUT/test_init_flash_fs" "$HERE/test_init_flash_fs.c"
"$OUT/test_init_flash_fs"
