#!/bin/sh
# Host harness for the stm32 USB MSC SCSI layer (AUD-023).  Builds the real
# ports/stm32/usbdev/class/src/usbd_msc_scsi.c against stub config headers and
# a fake storage table, then runs it.  Usage: tests/ports/stm32_host/msc_scsi/run.sh
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
TOP=$HERE/../../../..
STM=$TOP/ports/stm32
OUT=$(mktemp -d)
cp "$STM/usbdev/class/src/usbd_msc_scsi.c" "$OUT/"
# Older trees have no Sync entry in USBD_StorageTypeDef: then the fake unit
# cannot be flushed through the SCSI layer and the test must fail.
HAVE_SYNC=0
if grep -q '(\* Sync)' "$STM/usbdev/class/inc/usbd_cdc_msc_hid.h"; then HAVE_SYNC=1; fi
${CC:-gcc} -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-parameter -DHAVE_SYNC=$HAVE_SYNC \
    -I"$HERE/stub" -I"$STM/usbdev/core/inc" -I"$STM/usbdev/class/inc" \
    -o "$OUT/test_msc_scsi" "$HERE/test_msc_scsi.c" "$OUT/usbd_msc_scsi.c"
"$OUT/test_msc_scsi"
