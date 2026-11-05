#!/bin/sh
# Host harness for ports/stm32/flashbdev.c over a RAM "flash" that behaves like
# the STM32F405's: sectors of 16/64/128 KB, erase sets 0xFF, programming can
# only clear bits, and erase/write errors can be injected.
# The flash sits at 0x08000000 and the sector cache at 0x10000000, as on PYBV11
# (filesystem = sectors 1 to 4, 0x08004000..0x08020000, stm32f405.ld).
# Linux x86-64 only (fixed low mappings, non-PIE).
# Usage: tests/ports/stm32_host/flashbdev/run.sh [test-name ...]
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TOP=$HERE/../../../..
OUT=$(mktemp -d)
cp "$TOP/ports/stm32/flashbdev.c" "$OUT/"
${CC:-gcc} -std=gnu99 -O1 -g -Wall -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -no-pie \
    -I"$HERE/stub" \
    -Wl,--defsym,_micropy_hw_internal_flash_storage_start=0x08004000 \
    -Wl,--defsym,_micropy_hw_internal_flash_storage_end=0x08020000 \
    -Wl,--defsym,_micropy_hw_internal_flash_storage2_start=0 \
    -Wl,--defsym,_micropy_hw_internal_flash_storage2_end=0 \
    -Wl,--defsym,_micropy_hw_internal_flash_storage_ram_cache_start=0x10000000 \
    -Wl,--defsym,_micropy_hw_internal_flash_storage_ram_cache_end=0x10010000 \
    -o "$OUT/test_flashbdev" "$HERE/test_flashbdev.c" "$OUT/flashbdev.c"
"$OUT/test_flashbdev" "$@"
