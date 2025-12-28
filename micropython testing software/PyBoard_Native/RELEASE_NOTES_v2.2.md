# PyBoard Native Firmware v2.2 - Filesystem Release

**Release Date:** December 28, 2025  
**Firmware Size:** 94KB (94,520 bytes)  
**Target:** STM32F405RG @ 168MHz (PyBoard v1.1)

## What's New in v2.2

### Full FatFS Filesystem Support
- Real FatFS integration using MicroPython's oofatfs library
- 32GB SD card support (fixed 32-bit overflow bug)
- Partition and format commands
- Complete file operations

### New Commands
| Command | Description |
|---------|-------------|
| `mount` | Mount SD card filesystem |
| `umount` | Unmount filesystem |
| `partition` | Create MBR partition table (fdisk) |
| `format [fat\|fat32]` | Format SD card |
| `ls [path]` | List directory contents |
| `cat <file>` | Read file content |
| `write <file> <text>` | Write text to file |
| `append <file> <text>` | Append text to file |
| `log <file> <data>` | Append with automatic newline (for CSV logging) |
| `mkdir <dir>` | Create directory |
| `rm <file>` | Delete file or empty directory |
| `df` | Show disk free space |

## Problems Overcome

### 1. oofatfs API Differences
**Problem:** MicroPython's oofatfs library has a different API than standard FatFS.
- Functions take `FATFS*` as first parameter
- `f_mount()` takes only `FATFS*` (no path, no option)
- Directory type is `FF_DIR` not `DIR`
- `f_opendir()` takes `FATFS*` as first arg

**Solution:** Updated all FatFS calls to use oofatfs API pattern.

### 2. 32GB SD Card Overflow Bug
**Problem:** SD cards >4GB showed wrong capacity (32GB showed as 1190MB).
```c
// BROKEN - overflows at 4GB
uint32_t capacity_mb = info.BlockNbr * info.BlockSize / 1024 / 1024;
```

**Solution:** Divide first to avoid overflow:
```c
// FIXED - works for any size
uint32_t capacity_mb = info.BlockNbr / 2048;  // BlockNbr / (1M/512)
```

### 3. CSV Logging Newlines
**Problem:** `write` and `append` commands didn't add newlines, making CSV files unreadable.

**Solution:** Added `log` command that automatically appends newline after data.

### 4. ffconf.h Include Path
**Problem:** Standard FatFS uses `FFCONF_H` macro for config, but oofatfs expects direct include.

**Solution:** Modified ff.h to use `#include "ffconf.h"` directly.

### 5. Multi-Partition Support
**Problem:** Needed `f_fdisk()` for partitioning but it was disabled.

**Solution:** Enabled `FF_MULTI_PARTITION 1` in ffconf.h.

## Tested Features

### Filesystem Operations ✅
- Mount/unmount SD card
- Create partition table (MBR)
- Format as FAT32
- Create/delete files and directories
- Read/write/append files
- Nested directory support
- 32GB SD card (29858 MB usable)

### Data Logging ✅
- Accelerometer CSV logging (277 samples in 69 seconds)
- ADC voltage logging (108 samples with voltage calculation)
- Proper CSV format with headers and newlines

### Hardware Verified ✅
- All 4 LEDs (including PWM on LED4)
- MMA7660 accelerometer
- ADC (0-3.3V, 12-bit)
- DAC output
- USB CDC serial
- SD card via SDIO

## File Structure

```
PyBoard_Native/
├── Core/Src/main.c          # Main firmware (80KB source)
├── Middlewares/FatFs/src/
│   ├── ff.c                  # FatFS core (from oofatfs)
│   ├── ff.h                  # FatFS header
│   ├── ffconf.h              # Configuration
│   └── diskio.c              # SDIO disk driver
├── build.ps1                 # Build script
└── Debug/
    ├── PyBoard_Native.hex    # Flash image (266KB)
    └── PyBoard_Native.bin    # Raw binary (95KB)
```

## Build Information

- **Toolchain:** ARM GCC 13.3.1
- **Optimization:** -Os (size)
- **Flash Usage:** 94KB of 1MB
- **RAM Usage:** ~9KB static

## Flashing

```
# Using DFU (hold BOOT0 + reset)
dfu-util -a 0 -D PyBoard_Native.bin -s 0x08000000

# Using ST-Link
st-flash write PyBoard_Native.bin 0x08000000
```

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v2.2 | 2025-12-28 | FatFS filesystem, partition, format, 32GB fix |
| v2.1 | 2025-12-28 | LED4 PWM, intensity command |
| v2.0 | 2025-12-28 | Full peripheral support, MicroPython-style CLI |
| v1.0 | 2025-12-28 | Basic LED and accelerometer |
