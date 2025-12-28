# FatFS Setup for PyBoard Native Firmware

## Files Added

All FatFS files are in `Middlewares/FatFs/src/`:

1. **ff.c** - FatFS core (copied from MicroPython oofatfs)
2. **ff.h** - FatFS header (modified for standalone use)
3. **ffconf.h** - Configuration file (custom for this project)
4. **diskio.c** - SDIO disk driver (custom implementation)
5. **diskio.h** - Disk I/O interface (copied from oofatfs)

## STM32CubeIDE Build Configuration

### Add Source Files to Project

1. Open the project in STM32CubeIDE
2. Right-click on the project → Properties
3. Go to **C/C++ General → Paths and Symbols**
4. Under **Source Location** tab, add:
   - `Middlewares/FatFs/src`

Or manually add each .c file to the build:
- Right-click each .c file → Resource Configurations → Include in Build

### Add Include Path

1. Project Properties → C/C++ Build → Settings
2. Under **MCU GCC Compiler → Include paths**, add:
   - `../Middlewares/FatFs/src`

### Alternative: Modify Makefile (if using Makefile project)

Add to your Makefile:
```makefile
# FatFS Sources
C_SOURCES += \
Middlewares/FatFs/src/ff.c \
Middlewares/FatFs/src/diskio.c

# FatFS Includes
C_INCLUDES += \
-IMiddlewares/FatFs/src
```

## Configuration Summary

The current ffconf.h settings:
- **FF_USE_LFN = 0** - No long filename support (8.3 only, saves ~20KB)
- **FF_FS_READONLY = 0** - Read/Write enabled
- **FF_USE_MKFS = 1** - Format function available
- **FF_FS_TINY = 1** - Smaller memory footprint
- **FF_CODE_PAGE = 437** - US ASCII code page

## CLI Commands Available

After building and flashing:

| Command | Description |
|---------|-------------|
| `mount` | Mount SD card file system |
| `umount` | Unmount SD card |
| `ls [path]` | List directory contents |
| `cat <file>` | Display file contents |
| `write <file> <text>` | Create/overwrite file |
| `append <file> <text>` | Append to file |
| `mkdir <dir>` | Create directory |
| `rm <file>` | Delete file or empty directory |
| `df` | Show disk space usage |

## Notes

- This uses oofatfs from MicroPython, which has a slightly different API
- The FATFS.drv pointer is used for block device context (set to NULL for single drive)
- SD card must be FAT32 formatted
- Maximum filename length is 8.3 format (e.g., FILENAME.TXT)
