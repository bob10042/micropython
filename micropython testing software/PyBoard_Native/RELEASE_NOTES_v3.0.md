# PyBoard Native C Firmware v3.0 - Release Notes

**Release Date:** December 28, 2025  
**Target:** STM32F405RG @ 168MHz (PyBoard v1.1)  
**Size:** 290,153 bytes (hex) / 103,132 bytes (binary)

## Overview

Version 3.0 is a major release that adds MicroPython-compatible functionality to the native C firmware. This allows the C firmware to be used as a drop-in replacement for MicroPython development and testing, with full access to all PyBoard hardware through a CLI interface.

## v3.0.1 Update (December 31, 2025)

### STM32 HAL/CMSIS Library Synchronization
- Added local clone of [micropython/stm32lib](https://github.com/micropython/stm32lib) in `stm32lib/` subdirectory
- Updated to latest work branch: `work-F0-1.9.0+F4-1.16.0+F7-1.7.0+G0-1.5.1+G4-1.3.0+H5-1.0.0+H7-1.11.0+L0-1.11.2+L1-1.10.3+L4-1.17.0+N6-1.2.0+U5-1.8.0+WB-1.23.0+WL-1.1.0`
- Verified synchronization with main MicroPython repository's `lib/stm32lib` submodule (commit `0761975`)
- Contains HAL drivers and CMSIS headers for all supported STM32 families: F0, F4, F7, G0, G4, H5, H7, L0, L1, L4, N6, U5, WB, WL

## New Features in v3.0

### Multi-Channel ADC
- Read any ADC channel 0-15: `adc <0-15>`
- Internal temperature sensor: `adc temp`
- Battery voltage monitoring: `adc vbat`

### I2C Data Transfer
- Read bytes from device: `i2c1 read <addr> <len>`
- Write bytes to device: `i2c1 write <addr> <hex>`
- Memory address operations: `i2c1 readmem <addr> <memaddr> <len>`
- Same commands available for I2C2

### Watchdog Timer (IWDG)
- Start watchdog: `wdt start <timeout_ms>`
- Feed watchdog: `wdt feed`
- Timeout range: 125ms to 32768ms

### Power Management
- Sleep mode: `sleep` (any interrupt wakes)
- Stop mode: `stop` (EXTI/RTC wakes)
- Standby mode available via code

### System Information
- Unique device ID: `uid`
- Last reset cause: `resetcause`

### Viper Benchmark
- Cycle-accurate benchmark: `viper`
- Compares directly to MicroPython @viper decorator
- Native C: 2,262 µs vs MicroPython @viper: ~2,500 µs

## All Available Commands

### LED Control (pyb.LED)
| Command | Description |
|---------|-------------|
| `led N [0\|1]` | Control LED 1-4, toggle if no state |
| `intensity N` | LED4 brightness 0-255 (PWM) |
| `allon` / `alloff` | All LEDs on/off |
| `demo` | LED demo sequence |

### Accelerometer (pyb.Accel)
| Command | Description |
|---------|-------------|
| `accel [N]` | Read X/Y/Z (N times) |
| `tilt` | Read tilt status |

### Analog I/O (pyb.ADC, pyb.DAC)
| Command | Description |
|---------|-------------|
| `adc` | Read ADC (X1/PA0) |
| `adc <0-15>` | Read specific ADC channel |
| `adc temp` | Internal temperature sensor |
| `adc vbat` | Battery voltage |
| `dac N` | Set DAC 0-4095 (X5/PA4) |
| `pwm N` | Set PWM duty 0-100% (X2/PA1) |

### Digital I/O (pyb.Pin)
| Command | Description |
|---------|-------------|
| `xN [0\|1]` | Read/write X1-X12, X17-X22 |
| `yN [0\|1]` | Read/write Y1-Y12 |
| `toggle xN\|yN` | Toggle pin |
| `mode xN in\|out\|od` | Set pin mode |
| `pull xN up\|down\|none` | Set pull resistor |
| `pins` | List all GPIO states |
| `button` | Read user button |

### I2C (pyb.I2C)
| Command | Description |
|---------|-------------|
| `i2c1` | Scan I2C1 bus (X9/X10) |
| `i2c2` | Scan I2C2 bus (Y9/Y10) |
| `i2c1 read <addr> <len>` | Read bytes from device |
| `i2c1 write <addr> <hex>` | Write bytes to device |

### SPI (pyb.SPI)
| Command | Description |
|---------|-------------|
| `spi1 send <hex>` | Transfer on SPI1 (X6-X8) |
| `spi2 send <hex>` | Transfer on SPI2 (Y5-Y8) |

### UART (pyb.UART)
| Command | Description |
|---------|-------------|
| `uart2 send <text>` | Send on UART2 (X3/X4) |
| `uart2 recv` | Read UART2 buffer |
| `uart6 send <text>` | Send on UART6 (Y1/Y2) |
| `uart6 recv` | Read UART6 buffer |

### CAN (pyb.CAN)
| Command | Description |
|---------|-------------|
| `can send <id> <bytes>` | Send CAN frame (Y3/Y4) |
| `can recv` | Read CAN message |
| `can status` | CAN bus status |

### RTC (pyb.RTC)
| Command | Description |
|---------|-------------|
| `rtc` | Get date/time |
| `rtc set HH:MM:SS` | Set time |
| `rtc date YY-MM-DD` | Set date |

### SD Card & File System
| Command | Description |
|---------|-------------|
| `sd` | Card info |
| `mount` | Mount SD card |
| `ls [path]` | List directory |
| `cat <file>` | Read file |
| `write <file> <txt>` | Write to file |
| `df` | Disk free space |

### System
| Command | Description |
|---------|-------------|
| `info` | System information |
| `uid` | Unique device ID |
| `resetcause` | Last reset cause |
| `bench` | Benchmark (10K loop) |
| `speed` | CPU speed test (1M loop) |
| `speedgpio` | GPIO toggle benchmark |
| `viper` | Viper-style benchmark |
| `wdt start <ms>` | Start watchdog |
| `wdt feed` | Feed watchdog |
| `sleep` | Enter sleep mode |
| `stop` | Enter stop mode |
| `help` | Show all commands |

## Benchmark Results

| Test | Native C | MicroPython | Speedup |
|------|----------|-------------|---------|
| 10K loop | 2,262 µs | 993,000 µs | 439x |
| 1M loop | 238 ms | 780 ms | 3.3x |
| GPIO toggle (100K) | 60 ms | - | - |
| Viper benchmark | 2,262 µs | 2,500 µs | 1.1x |

## Hardware Pin Mapping

### X-Series Pins (directly accessible via `xN`)
| Pin | Function | Alternate |
|-----|----------|-----------|
| X1 | PA0 | ADC CH0 |
| X2 | PA1 | PWM (TIM2) |
| X3 | PA2 | UART2 TX |
| X4 | PA3 | UART2 RX |
| X5 | PA4 | DAC CH1 |
| X6 | PA5 | SPI1 SCK |
| X7 | PA6 | SPI1 MISO |
| X8 | PA7 | SPI1 MOSI |
| X9 | PB6 | I2C1 SCL |
| X10 | PB7 | I2C1 SDA |

### Y-Series Pins
| Pin | Function | Alternate |
|-----|----------|-----------|
| Y1 | PC6 | UART6 TX |
| Y2 | PC7 | UART6 RX |
| Y3 | PB8 | CAN1 RX |
| Y4 | PB9 | CAN1 TX |
| Y9 | PB10 | I2C2 SCL |
| Y10 | PB11 | I2C2 SDA |

### LEDs
| LED | Pin | Color |
|-----|-----|-------|
| LED1 | PA13 | Red |
| LED2 | PA14 | Green |
| LED3 | PA15 | Yellow |
| LED4 | PB4 | Blue (PWM) |

## Build Requirements

- STM32CubeIDE 2.0.0 or ARM GCC 13.3.1
- STM32Cube_FW_F4_V1.28.0 (for USB middleware)
- PowerShell (for build.ps1)

### Build Command
```powershell
cd "micropython testing software\PyBoard_Native"
.\build.ps1 -Rebuild
```

## Flashing

Use ST-Link, DFU bootloader, or STM32CubeProgrammer:
```
PyBoard_Native_v3.0_MicroPython_Compatible.hex
```

## Version History

| Version | Date | Size | Changes |
|---------|------|------|---------|
| v1.0 | 2025-12-28 | 174KB | Initial release |
| v2.0 | 2025-12-28 | 208KB | Added SPI, UART, CAN, RTC |
| v2.1 | 2025-12-28 | 209KB | LED4 PWM, intensity control |
| v2.2 | 2025-12-28 | 266KB | FatFS file system |
| v3.0 | 2025-12-28 | 290KB | MicroPython compatible features |
| v3.0.1 | 2025-12-31 | 290KB | Added stm32lib reference library |

## Files Location

```
micropython testing software/PyBoard_Native/
├── Core/Src/main.c           # Main source code
├── Core/Inc/                  # Header files
├── Debug/                     # Build outputs
│   ├── PyBoard_Native.hex
│   ├── PyBoard_Native.bin
│   └── PyBoard_Native.elf
├── stm32lib/                  # MicroPython STM32 HAL library (reference)
│   ├── CMSIS/                 # CMSIS device headers
│   ├── STM32F4xx_HAL_Driver/  # F4 HAL (used by PyBoard)
│   ├── STM32F0xx_HAL_Driver/  # Other MCU families...
│   └── README.md
├── firmware_releases/         # Release archives
│   ├── PyBoard_Native_v1.0_working.hex
│   ├── PyBoard_Native_v2.0_working.hex
│   ├── PyBoard_Native_v2.1_LED4_PWM.hex
│   └── PyBoard_Native_v3.0_MicroPython_Compatible.hex
├── backups/                   # Timestamped backups
│   └── v3.0_20251228_*/
├── build.ps1                  # PowerShell build script
└── Middlewares/               # USB, FatFS libraries
```
