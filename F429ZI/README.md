# STM32F429ZI NUCLEO-144 Complete IO Project

## Project Overview

This is a standalone bare-metal project for the **NUCLEO-F429ZI** development board that demonstrates multiple peripheral interfaces running simultaneously.

**Project Location:** open this folder in VS Code (this repo contains everything needed).

---

## Quick Start

### VS Code (recommended)

1. Open this folder in VS Code
2. Run **Terminal → Run Task…**
3. Choose one:
   - `STM32: Build (make all)`
   - `STM32: Build + Flash (build_and_flash.ps1)`

### PowerShell (repo root)

```powershell
# Build
pwsh -NoProfile -ExecutionPolicy Bypass -File .\build_and_flash.ps1 -BuildOnly

# Build + flash
pwsh -NoProfile -ExecutionPolicy Bypass -File .\build_and_flash.ps1
```

---

## Environment Setup

### STM32 Tools Locations

This project can be built with a standard **Arm GNU Toolchain** + **make**, and flashed with **STM32CubeProgrammer**.
Exact install locations vary per machine.

| Tool | Path |
|------|------|
| **STM32CubeIDE** | Typically under `C:\ST\STM32CubeIDE_*\STM32CubeIDE\` |
| **STM32CubeMX** | Typical user install under `%LOCALAPPDATA%\Programs\STM32CubeMX\` |
| **STM32CubeProgrammer** | Typical install: `C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\` |
| **ARM GCC Toolchain** | Any Arm GNU Toolchain containing `arm-none-eabi-gcc.exe` |
| **Make Utility** | Any `make.exe` (MSYS2 is common on Windows) |
| **STM32Cube Repository** | Optional (for CubeMX regeneration) |

### Environment Variables (optional)

The build/flash script supports these optional env vars:

- `ARM_GCC_BIN`: folder containing `arm-none-eabi-gcc.exe`
- `MAKE_BIN`: folder containing `make.exe`
- `STM32PROG_CLI`: full path to `STM32_Programmer_CLI.exe`

This repo also includes VS Code workspace settings that set these for integrated terminals/tasks in `.vscode/settings.json`.

---

## Firmware Functionality

### What the Firmware Does

The firmware initializes multiple peripherals and runs a main loop that:

1. **LED Blink (100ms interval):**
   - Green LED (PB0) toggles every 100ms

2. **PWM Breathing Effect:**
   - TIM3 generates PWM on PB4
   - PWM duty cycle ramps up and down (0-100%) creating a "breathing" effect

3. **ADC Reading:**
   - ADC1 reads analog voltage on PA3 (ADC Channel 3)
   - 12-bit resolution (0-4095)
   - Sampled every 100ms

4. **Button Interrupt:**
   - User button (PC13) triggers EXTI interrupt
   - Red LED (PB14) toggles on each button press

5. **Serial Output (UART):**
   - Every second prints status via USART3 (ST-LINK Virtual COM)
   - Format: `ADC: xxxx | PWM: xxxx | Button: Released/Pressed`
   - Baud rate: 115200, 8N1

---

## Hardware Configuration

### Target Board
- **Board:** NUCLEO-F429ZI
- **MCU:** STM32F429ZIT6
- **Core:** ARM Cortex-M4 @ 180 MHz
- **Flash:** 2 MB
- **RAM:** 256 KB

### Pin Mapping

#### GPIO Outputs (LEDs)
| Function | Pin | Port | Mode |
|----------|-----|------|------|
| LED1 Green | PB0 | GPIOB | Push-Pull Output |
| LED2 Blue | PB7 | GPIOB | Push-Pull Output |
| LED3 Red | PB14 | GPIOB | Push-Pull Output |

#### GPIO Input (Button)
| Function | Pin | Port | Mode |
|----------|-----|------|------|
| User Button | PC13 | GPIOC | Input, Falling Edge Interrupt (EXTI15_10) |

#### USART3 (ST-LINK VCP)
| Function | Pin | Port | Alternate Function |
|----------|-----|------|-------------------|
| TX | PD8 | GPIOD | AF7_USART3 |
| RX | PD9 | GPIOD | AF7_USART3 |

**Serial Settings:** 115200 baud, 8 data bits, No parity, 1 stop bit

#### SPI1 (Full Duplex Master)
| Function | Pin | Port | Alternate Function |
|----------|-----|------|-------------------|
| SCK | PA5 | GPIOA | AF5_SPI1 |
| MISO | PA6 | GPIOA | AF5_SPI1 |
| MOSI | PA7 | GPIOA | AF5_SPI1 |

**SPI Settings:** Master mode, 8-bit, CPOL=0, CPHA=0 (Mode 0)

#### I2C1 (Standard Mode)
| Function | Pin | Port | Alternate Function |
|----------|-----|------|-------------------|
| SCL | PB8 | GPIOB | AF4_I2C1 |
| SDA | PB9 | GPIOB | AF4_I2C1 |

**I2C Settings:** 100 kHz clock, 7-bit addressing

#### ADC1 (Analog Input)
| Function | Pin | Port | Channel |
|----------|-----|------|---------|
| Analog In | PA3 | GPIOA | ADC123_IN3 |

**ADC Settings:** 12-bit resolution, single conversion, software trigger

#### TIM3 PWM Output
| Function | Pin | Port | Alternate Function |
|----------|-----|------|-------------------|
| PWM CH1 | PB4 | GPIOB | AF2_TIM3 |

**PWM Settings:** 1 kHz frequency, variable duty cycle (0-100%)

---

## Clock Configuration

- **HSE:** 8 MHz (ST-LINK MCO bypass)
- **PLL:** HSE / 4 × 180 / 2 = 180 MHz
- **SYSCLK:** 180 MHz
- **AHB:** 180 MHz
- **APB1:** 45 MHz
- **APB2:** 90 MHz

---

## Project Structure

```
STM32_Standalone/
├── build/                      # Compiled output
│   ├── STM32_Standalone.elf   # Debug executable
│   ├── STM32_Standalone.hex   # Intel HEX format
│   └── STM32_Standalone.bin   # Raw binary
├── Drivers/
│   ├── CMSIS/
│   │   ├── Device/ST/STM32F4xx/Include/  # Device headers
│   │   └── Include/                       # Core headers
│   └── STM32F4xx_HAL_Driver/
│       ├── Inc/               # HAL headers
│       │   └── Legacy/        # Legacy compatibility
│       └── Src/               # HAL source files
├── Inc/
│   ├── main.h                 # Pin definitions
│   └── stm32f4xx_hal_conf.h   # HAL module configuration
├── Src/
│   ├── main.c                 # Main application code
│   ├── stm32f4xx_it.c        # Interrupt handlers
│   └── system_stm32f4xx.c    # System init
├── Startup/
│   └── startup_stm32f429xx.s  # Startup assembly
├── Makefile                   # Build configuration
├── STM32F429ZITX_FLASH.ld    # Linker script
└── README.md                  # This file
```

---

## Building the Project

### Using VS Code (recommended)

This repo includes tasks:

- `STM32: Build (make all)`
- `STM32: Clean (make clean)`
- `STM32: Build + Flash (build_and_flash.ps1)`

Run them via **Terminal → Run Task…**

### Using PowerShell (command line)

```powershell
# From the repo root
pwsh -NoProfile -ExecutionPolicy Bypass -File .\build_and_flash.ps1 -BuildOnly
```

### Build Output
- **Size:** ~18 KB
- **Output files:** `build/STM32_Standalone.elf`, `.hex`, `.bin`

---

## Flashing the Firmware

### Using the included flash script (recommended)

```powershell
# Build + flash
pwsh -NoProfile -ExecutionPolicy Bypass -File .\build_and_flash.ps1

# Flash only (requires existing build\STM32_Standalone.elf)
pwsh -NoProfile -ExecutionPolicy Bypass -File .\build_and_flash.ps1 -FlashOnly
```

The script will use `STM32PROG_CLI` if set, otherwise it falls back to the standard STM32CubeProgrammer install location.

### Using STM32CubeProgrammer GUI
1. Open STM32CubeProgrammer
2. Connect via ST-LINK (SWD)
3. File → Open File → Select `STM32_Standalone.elf`
4. Click "Download"

---

## Monitoring Serial Output

Connect to ST-LINK Virtual COM Port:

- **Baud Rate:** 115200
- **Data Bits:** 8
- **Parity:** None
- **Stop Bits:** 1

**Tools:**
- PuTTY
- Tera Term
- VS Code Serial Monitor extension
- Arduino IDE Serial Monitor

Expected output:
```
========================================
  STM32F429ZI NUCLEO - Full IO Support
========================================
Peripherals initialized:
  - GPIO: LEDs (PB0,PB7,PB14), Button (PC13)
  - USART3: 115200 baud (PD8/PD9)
  - SPI1: Master mode (PA5/PA6/PA7)
  - I2C1: 100kHz (PB8/PB9)
  - ADC1: Channel 3 (PA3)
  - TIM3: PWM on CH1 (PB4)
========================================

ADC: 2048 | PWM:  500 | Button: Released
ADC: 2051 | PWM:  510 | Button: Released
Button pressed! Red LED toggled.
ADC: 2047 | PWM:  520 | Button: Pressed
```

---

## HAL Modules Enabled

| Module | File | Purpose |
|--------|------|---------|
| HAL Core | stm32f4xx_hal.c | Base HAL functions |
| GPIO | stm32f4xx_hal_gpio.c | Digital I/O |
| RCC | stm32f4xx_hal_rcc.c | Clock configuration |
| PWR | stm32f4xx_hal_pwr.c | Power management |
| FLASH | stm32f4xx_hal_flash.c | Flash latency |
| CORTEX | stm32f4xx_hal_cortex.c | NVIC, SysTick |
| DMA | stm32f4xx_hal_dma.c | DMA support |
| UART | stm32f4xx_hal_uart.c | Serial communication |
| SPI | stm32f4xx_hal_spi.c | SPI interface |
| I2C | stm32f4xx_hal_i2c.c | I2C interface |
| ADC | stm32f4xx_hal_adc.c | Analog to digital |
| TIM | stm32f4xx_hal_tim.c | Timer/PWM |

---

## Extending the Project

### Adding a New GPIO
```c
// In main.c, inside GPIO_Init():
GPIO_InitStruct.Pin = GPIO_PIN_x;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
```

### Using SPI to Talk to a Device
```c
uint8_t txData = 0xAB;
uint8_t rxData;
HAL_SPI_TransmitReceive(&hspi1, &txData, &rxData, 1, HAL_MAX_DELAY);
```

### Using I2C to Read a Sensor
```c
uint8_t regAddr = 0x00;
uint8_t data;
HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, regAddr, 1, &data, 1, HAL_MAX_DELAY);
```

---

## Creating an STM32CubeMX .ioc File

If you want to regenerate this project using STM32CubeMX:

1. Open STM32CubeMX: `C:\Users\bob43\AppData\Local\Programs\STM32CubeMX\STM32CubeMX.exe`
2. File → New Project → Search "STM32F429ZIT"
3. Configure peripherals as documented above
4. Project Manager → Toolchain: STM32CubeIDE or Makefile
5. Generate Code

---

## Troubleshooting

### Build Error: "arm-none-eabi-gcc not found"
Ensure the toolchain is installed and either:

- `arm-none-eabi-gcc` is already on your PATH, or
- set `ARM_GCC_BIN` to the toolchain `bin` folder

### Flash Error: "No ST-LINK detected"
- Check USB connection
- Install ST-LINK drivers from STM32CubeProgrammer installation

### No Serial Output
- Check COM port in Device Manager
- Verify baud rate is 115200
- Ensure correct USB cable (must support data, not charge-only)

---

## Version Info

- **STM32CubeIDE:** 2.0.0
- **STM32Cube_FW_F4:** V1.28.0
- **ARM GCC:** depends on installed toolchain
- **Project Created:** December 31, 2025

---

## Repo Housekeeping (VS Code / STM32Cube)

This repo has a few quality-of-life changes to make building/flashing from *this* folder reliable:

- `build_and_flash.ps1` now builds/flashes relative to the repo (no hard-coded project directory)
- added VS Code tasks under `.vscode/tasks.json` for build/clean/flash
- added workspace env defaults under `.vscode/settings.json` so tool paths work in VS Code terminals/tasks
- added `.vscode/extensions.json` to recommend the STM32Cube VS Code extensions used with the `STM32CubeIDE/` project
