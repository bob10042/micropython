# PyBoard v1.1 Native C Firmware v3.0

Native C firmware for PyBoard v1.1 (STM32F405RG) - replaces MicroPython for maximum performance.

## Project Status: ✅ FULLY FUNCTIONAL - MicroPython Compatible

**Latest Version:** v3.0 (December 28, 2025)  
**Firmware Size:** 290KB (hex) / 103KB (binary)

### What's New in v3.0
- Multi-channel ADC (0-15 + temp sensor + VBAT)
- I2C data transfer (read/write bytes)
- Watchdog timer (IWDG)
- Power management (sleep/stop modes)
- Unique device ID and reset cause
- Viper-style cycle-accurate benchmark
- Full FatFS file system support

See [RELEASE_NOTES_v3.0.md](RELEASE_NOTES_v3.0.md) for complete details.

All peripherals matching MicroPython pyb module:
- USB CDC serial communication (REPL-like CLI)
- 4 LEDs (Red, Green, Yellow, Blue) with PWM intensity on LED4
- User button with EXTI interrupt
- MMA7660 3-axis accelerometer (I2C1)
- 12-bit ADC (X1/PA0)
- 12-bit DAC output (X5/PA4)
- PWM output (X2/PA1 via TIM2, Y11/Y12 via TIM3)
- I2C1 (X9/X10), I2C2 (Y9/Y10)
- SPI1 (X6-X8), SPI2 (Y5-Y8)
- UART2 (X3/X4), UART6 (Y1/Y2)
- CAN1 bus (Y3/Y4)
- SD card (SDIO 4-bit)
- RTC with 32.768kHz LSE crystal
- All X1-X12, X17-X22 and Y1-Y12 GPIO pins
- Runtime GPIO mode/pull configuration

## Build Requirements

### Toolchain
- STM32CubeIDE 1.14+ or ARM GCC 13.3.1+
- PowerShell (Windows)

### HAL Library (CRITICAL!)
**MUST use STM32CubeF4 V1.28.0 from STMicroelectronics**

Location: `C:\Users\bob43\STM32Cube\Repository\STM32Cube_FW_F4_V1.28.0`

⚠️ **DO NOT use MicroPython's HAL library** from `lib/stm32lib/` - it is an older version with incompatible USB and CAN APIs that will cause USB enumeration failure.

### STM32 HAL Library Reference (stm32lib/)
This project includes a local clone of [micropython/stm32lib](https://github.com/micropython/stm32lib) in the `stm32lib/` subdirectory. This repository contains:
- **CMSIS** device headers for all supported STM32 families
- **HAL Drivers** for STM32F0, F4, F7, G0, G4, H5, H7, L0, L1, L4, N6, U5, WB, WL

**Current Version (December 2025):** `work-F0-1.9.0+F4-1.16.0+F7-1.7.0+G0-1.5.1+G4-1.3.0+H5-1.0.0+H7-1.11.0+L0-1.11.2+L1-1.10.3+L4-1.17.0+N6-1.2.0+U5-1.8.0+WB-1.23.0+WL-1.1.0`

**Note:** The main MicroPython repository's `lib/stm32lib` submodule has been verified to be on the same version. For this PyBoard_Native project, we use the official STM32CubeF4 V1.28.0 from STMicroelectronics (not this stm32lib) due to USB/CAN API compatibility requirements. The stm32lib is included for reference and potential future migration.

## Building

### Option 1: STM32CubeIDE
1. Open `PyBoard_Native.ioc` in STM32CubeIDE
2. Generate code (Alt+K)
3. Build project (Ctrl+B)

### Option 2: PowerShell Script
```powershell
cd "c:\Users\bob43\Downloads\micropython\micropython testing software\PyBoard_Native"
.\build.ps1
```

Output: `build/pyboard_native.bin` (~80KB)

## Flashing

1. Connect PyBoard via USB
2. Enter DFU mode: Hold DFU button, press RST, release DFU
3. Flash using DFU-util:
```powershell
dfu-util -a 0 -s 0x08000000:leave -D build/pyboard_native.bin
```

## Serial Interface

Connect via any serial terminal at 115200 baud (USB CDC).

### Complete CLI Commands (MicroPython Equivalent)

| Command | Description | pyb Equivalent |
|---------|-------------|----------------|
| **LEDs** |||
| `led N [0\|1]` | Control LED 1-4 | `pyb.LED(N).on/off()` |
| `intensity N` | LED4 brightness 0-255 | `pyb.LED(4).intensity()` |
| `allon/alloff` | All LEDs on/off | - |
| `demo` | LED demo sequence | - |
| **Accelerometer** |||
| `accel [N]` | Read X/Y/Z (N times) | `pyb.Accel().x/y/z()` |
| `tilt` | Read tilt register | `pyb.Accel().tilt()` |
| **Analog I/O** |||
| `adc` | Read ADC (X1/PA0) | `pyb.ADC('X1').read()` |
| `dac N` | Set DAC 0-4095 (X5/PA4) | `pyb.DAC(1).write()` |
| `pwm N` | Set PWM 0-100% (X2/PA1) | `pyb.Timer().channel()` |
| **Digital I/O** |||
| `xN [0\|1]` | Read/write X1-X12, X17-X22 | `pyb.Pin('XN').value()` |
| `yN [0\|1]` | Read/write Y1-Y12 | `pyb.Pin('YN').value()` |
| `toggle xN` | Toggle pin | `pin.toggle()` |
| `mode xN in\|out\|od` | Set pin mode | `Pin('X1', Pin.OUT)` |
| `pull xN up\|down\|none` | Set pull resistor | `Pin.PULL_UP/DOWN` |
| `pins` | List all GPIO states | - |
| `button` | Read user button | `pyb.Switch()()` |
| **I2C** |||
| `i2c1` | Scan I2C1 bus (X9/X10) | `pyb.I2C(1).scan()` |
| `i2c2` | Scan I2C2 bus (Y9/Y10) | `pyb.I2C(2).scan()` |
| **SPI** |||
| `spi1 send <hex>` | Transfer on SPI1 | `pyb.SPI(1).send_recv()` |
| `spi2 send <hex>` | Transfer on SPI2 | `pyb.SPI(2).send_recv()` |
| **UART** |||
| `uart2 send <text>` | Send on UART2 (X3/X4) | `pyb.UART(2).write()` |
| `uart2 recv` | Receive from UART2 | `pyb.UART(2).read()` |
| `uart6 send <text>` | Send on UART6 (Y1/Y2) | `pyb.UART(6).write()` |
| `uart6 recv` | Receive from UART6 | `pyb.UART(6).read()` |
| **CAN** |||
| `can send <id> <bytes>` | Send CAN frame (Y3/Y4) | `pyb.CAN(1).send()` |
| `can recv` | Read CAN message | `pyb.CAN(1).recv()` |
| `can status` | CAN bus status | `pyb.CAN(1).state()` |
| **RTC** |||
| `rtc` | Get date/time | `pyb.RTC().datetime()` |
| `rtc set HH:MM:SS` | Set time | `pyb.RTC().datetime()` |
| `rtc date YY-MM-DD` | Set date | `pyb.RTC().datetime()` |
| **SD Card** |||
| `sd` | Card info | `pyb.SDCard().info()` |
| `sd init` | Initialize card | `pyb.SDCard()` |
| **System** |||
| `info` | System information | `sys.implementation` |
| `speed` | CPU speed test | - |
| `speedgpio` | GPIO toggle benchmark | - |
| `help` | Show all commands | `help()` |

---

# CRITICAL ISSUES FIXED (Documentation for Future Reference)

## Issue 1: USB Not Enumerating After DFU Flash

### Symptom
Device flashes successfully via DFU but USB doesn't enumerate. Windows shows "Unknown USB Device (Device Descriptor Request Failed)".

### Root Cause
**STM32CubeMX defaults to 8MHz HSE crystal, but PyBoard v1.1 has a 12MHz crystal.**

The PLL configuration was:
- PLLM = 8 (for 8MHz crystal)
- This produced 72MHz USB clock instead of required 48MHz
- USB requires exactly 48MHz ±0.25%

### Solution
Fix PLL configuration in `SystemClock_Config()`:

```c
// WRONG (8MHz crystal):
RCC_OscInitStruct.PLL.PLLM = 8;
RCC_OscInitStruct.PLL.PLLN = 336;

// CORRECT (12MHz crystal on PyBoard v1.1):
RCC_OscInitStruct.PLL.PLLM = 12;  // 12MHz / 12 = 1MHz VCO input
RCC_OscInitStruct.PLL.PLLN = 336; // 1MHz * 336 = 336MHz VCO
RCC_OscInitStruct.PLL.PLLP = 2;   // 336MHz / 2 = 168MHz SYSCLK
RCC_OscInitStruct.PLL.PLLQ = 7;   // 336MHz / 7 = 48MHz USB clock ✓
```

Also update `stm32f4xx_hal_conf.h`:
```c
#define HSE_VALUE    12000000U  // Not 8000000U!
```

### Additional USB Fix
Add soft disconnect/reconnect in `MX_USB_DEVICE_Init()` or `usbd_conf.c`:
```c
HAL_PCD_DevDisconnect(&hpcd_USB_OTG_FS);
HAL_Delay(50);
HAL_PCD_DevConnect(&hpcd_USB_OTG_FS);
```

---

## Issue 2: LEDs Not Working

### Symptom
LED commands don't turn on any LEDs.

### Root Cause
STM32CubeMX generated wrong LED pin assignments. Default was PA1, PA2, PA3 but PyBoard v1.1 uses:

| LED | Color | Actual Pin |
|-----|-------|------------|
| LED1 | Red | PA13 |
| LED2 | Green | PA14 |
| LED3 | Yellow | PA15 |
| LED4 | Blue | PB4 |

### Solution
Reference MicroPython's `ports/stm32/boards/PYBV11/mpconfigboard.h`:
```c
#define MICROPY_HW_LED1             pin_A13  // red
#define MICROPY_HW_LED2             pin_A14  // green
#define MICROPY_HW_LED3             pin_A15  // yellow
#define MICROPY_HW_LED4             pin_B4   // blue
```

Note: PA13/PA14 are SWDIO/SWCLK - reprogram GPIO after debug session!

---

## Issue 3: Button Pin Wrong

### Symptom
Button always reads as pressed or not pressed.

### Root Cause
Wrong button pin configuration.

### Solution
PyBoard v1.1 button is on **PB3** (active LOW with internal pull-up):
```c
#define USER_BUTTON_Pin GPIO_PIN_3
#define USER_BUTTON_GPIO_Port GPIOB
```

---

## Issue 4: Accelerometer Not Responding

### Symptom
I2C scan finds no devices at 0x4C.

### Root Cause
MMA7660 accelerometer has a power enable pin that must be set HIGH.

### Solution
MMA power enable is on **PB5**:
```c
// In GPIO init:
GPIO_InitStruct.Pin = GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);  // Power ON
HAL_Delay(10);  // Wait for power stabilization
```

---

## Issue 5: HAL Library Version Conflict

### Symptom
After copying HAL drivers from MicroPython's `lib/stm32lib/`, USB stops working. Build succeeds but USB doesn't enumerate.

### Root Cause
MicroPython uses an **older HAL version** with different APIs:
1. **CAN API differences**: Old HAL uses `CAN_SJW_1TQ`, `CAN_BS1_6TQ`, etc.
   New HAL uses `CAN_SYNC_JUMP_WIDTH_1TQ`, `CAN_TIME_SEG1_6TQ`, etc.
2. **TIM API differences**: Old HAL lacks `AutoReloadPreload` field.
3. **USB PCD differences**: Internal API changes break USB enumeration.

### Solution
**Always use STM32CubeF4 V1.28.0** from STMicroelectronics:
```
C:\Users\bob43\STM32Cube\Repository\STM32Cube_FW_F4_V1.28.0\Drivers\STM32F4xx_HAL_Driver\
```

If you must restore HAL, copy from the official STM32Cube repository, NOT from MicroPython!

---

## Issue 6: CAN Initialization Errors

### Symptom
CAN init fails to compile with "unknown field" errors.

### Root Cause
CAN HAL API changed between versions. Field names are different.

### Solution
Use the new API field names:
```c
// OLD (MicroPython lib - DON'T USE):
hcan1.Init.SJW = CAN_SJW_1TQ;
hcan1.Init.BS1 = CAN_BS1_6TQ;
hcan1.Init.BS2 = CAN_BS2_8TQ;
hcan1.Init.TTCM = DISABLE;
hcan1.Init.ABOM = DISABLE;

// NEW (STM32CubeF4 V1.28.0 - USE THIS):
hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
hcan1.Init.TimeSeg1 = CAN_BS1_6TQ;
hcan1.Init.TimeSeg2 = CAN_BS2_8TQ;
hcan1.Init.TimeTriggeredMode = DISABLE;
hcan1.Init.AutoBusOff = DISABLE;
hcan1.Init.AutoWakeUp = DISABLE;
hcan1.Init.AutoRetransmission = ENABLE;  // Note: inverted from NART
hcan1.Init.ReceiveFifoLocked = DISABLE;
hcan1.Init.TransmitFifoPriority = DISABLE;
```

---

## Issue 7: TIM Initialization Errors

### Symptom
TIM init fails to compile with "unknown field 'AutoReloadPreload'".

### Root Cause
Older HAL versions don't have `AutoReloadPreload` field in TIM_HandleTypeDef.

### Solution
When using STM32CubeF4 V1.28.0, include AutoReloadPreload:
```c
htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
```

---

## Issue 8: X/Y GPIO Pin Mapping Wrong

### Symptom
GPIO reads show wrong values or pins don't respond.

### Root Cause
STM32CubeMX doesn't know PyBoard pin naming. Must reference MicroPython's pins.csv.

### Solution
Correct mappings from `ports/stm32/boards/PYBV11/pins.csv`:

| PyBoard | STM32 | PyBoard | STM32 |
|---------|-------|---------|-------|
| X1 | PA0 | Y1 | PC6 |
| X2 | PA1 | Y2 | PC7 |
| X3 | PA2 | Y3 | PB8 |
| X4 | PA3 | Y4 | PB9 |
| X5 | PA4 | Y5 | PB12 |
| X6 | PA5 | Y6 | PB13 |
| X7 | PA6 | Y7 | PB14 |
| X8 | PA7 | Y8 | PB15 |
| X9 | PB6 | Y9 | PB10 |
| X10 | PB7 | Y10 | PB11 |
| X11 | PC4 | Y11 | PB0 |
| X12 | PC5 | Y12 | PB1 |
| X17 | PB3 | | |
| X18 | PC13 | | |
| X19 | PC0 | | |
| X20 | PC1 | | |
| X21 | PC2 | | |
| X22 | PC3 | | |

Note: Some pins are shared with peripherals:
- Y3/Y4: CAN1 (PB8/PB9)
- Y5-Y8: SPI2 (PB12-15)
- Y9/Y10: I2C2 (PB10/11)
- Y1/Y2: UART6 (PC6/7)

---

# Hardware Reference

## PyBoard v1.1 Specifications

| Parameter | Value |
|-----------|-------|
| MCU | STM32F405RGT6 |
| Core | ARM Cortex-M4 with FPU |
| Clock | 168 MHz |
| Flash | 1 MB |
| RAM | 192 KB |
| HSE Crystal | **12 MHz** |
| USB | Full Speed (FS) |

## Pin Functions

### LEDs
- PA13: LED1 (Red)
- PA14: LED2 (Green)  
- PA15: LED3 (Yellow)
- PB4: LED4 (Blue)

### Button
- PB3: User button (active LOW)

### Accelerometer (MMA7660)
- PB6: I2C1 SCL
- PB7: I2C1 SDA
- PB5: MMA Power Enable

### ADC/DAC
- PA0 (X1): ADC1 Channel 0
- PA4 (X5): DAC1 Channel 1

### I2C
- I2C1: PB6 (SCL), PB7 (SDA) - Accelerometer
- I2C2: PB10 (SCL), PB11 (SDA) - Y9, Y10

### SPI
- SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI) - X6, X7, X8
- SPI2: PB13 (SCK), PB14 (MISO), PB15 (MOSI), PB12 (NSS) - Y6, Y7, Y8, Y5

### UART
- UART2: PA2 (TX), PA3 (RX) - X3, X4
- UART6: PC6 (TX), PC7 (RX) - Y1, Y2

### CAN
- CAN1: PB8 (RX), PB9 (TX) - Y3, Y4

### SD Card (SDIO)
- PC8: SDIO D0
- PC9: SDIO D1
- PC10: SDIO D2
- PC11: SDIO D3
- PC12: SDIO CK
- PD2: SDIO CMD

---

# Project Structure

```
PyBoard_Native/
├── Core/
│   ├── Inc/
│   │   ├── main.h              # Pin definitions
│   │   ├── stm32f4xx_hal_conf.h # HAL configuration (HSE=12MHz!)
│   │   ├── stm32f4xx_it.h      # Interrupt handlers
│   │   └── usbd_*.h            # USB CDC headers
│   └── Src/
│       ├── main.c              # Main firmware + CLI
│       ├── stm32f4xx_hal_msp.c # Peripheral MSP init
│       ├── stm32f4xx_it.c      # Interrupt handlers
│       ├── system_stm32f4xx.c  # System init
│       └── usbd_*.c            # USB CDC implementation
├── Drivers/
│   ├── CMSIS/                  # ARM CMSIS headers
│   └── STM32F4xx_HAL_Driver/   # STM32 HAL (V1.28.0!)
├── Middlewares/
│   └── ST/STM32_USB_Device_Library/  # USB middleware
├── build/                      # Build output
├── build.ps1                   # Build script
├── STM32F405RGTx_FLASH.ld      # Linker script
└── README.md                   # This file
```

---

# GPIO Functionality Status

## ✅ Implemented
| Feature | Commands | Notes |
|---------|----------|-------|
| Read X1-X12 | `x1`, `gpio x1` | Returns 0 or 1 |
| Read Y1-Y12 | `y1`, `gpio y1` | Returns 0 or 1 |
| Write X1-X12 | `x1 1`, `gpio x1 0` | Set pin high/low |
| Write Y1-Y12 | `y1 1`, `gpio y1 0` | Set pin high/low |
| List all pins | `pins`, `gpiolist` | Shows X1-12, Y1-12 states |

## ❌ Not Yet Implemented
| Feature | MicroPython Equivalent | CLI Command to Add |
|---------|------------------------|-------------------|
| Extended X pins | `Pin('X17')` to `Pin('X22')` | `x17`, `x18`, etc. |
| Pin mode setting | `Pin('X1', Pin.OUT)` | `mode x1 in/out/od` |
| Pull resistors | `Pin('X1', Pin.IN, Pin.PULL_UP)` | `pull x1 up/down/none` |
| Pin toggle | `pin.toggle()` | `toggle x1` |
| Pulse output | N/A (useful for debugging) | `pulse x1 <microseconds>` |
| Pin info | `Pin('X1')` repr | `pininfo x1` |
| Alternate function | `Pin('X1', Pin.AF_PP, af=1)` | `af x1 <af_num>` |
| Open drain mode | `Pin.OPEN_DRAIN` | Support in `mode` command |
| Interrupt on pin | `ExtInt(pin, ...)` | `irq x1 rising/falling` |

## Code Changes Required

### 1. Extend GPIO_SetPin() and GPIO_ReadPin() in main.c
Add cases 17-22 for X17-X22 pins (definitions already exist in main.h):
```c
case 17: port = GPIO_X17_GPIO_Port; gpio_pin = GPIO_X17_Pin; break;
case 18: port = GPIO_X18_GPIO_Port; gpio_pin = GPIO_X18_Pin; break;
case 19: port = GPIO_X19_GPIO_Port; gpio_pin = GPIO_X19_Pin; break;
case 20: port = GPIO_X20_GPIO_Port; gpio_pin = GPIO_X20_Pin; break;
case 21: port = GPIO_X21_GPIO_Port; gpio_pin = GPIO_X21_Pin; break;
case 22: port = GPIO_X22_GPIO_Port; gpio_pin = GPIO_X22_Pin; break;
```

### 2. Add new CLI commands in CLI_Process()
```c
/* Pin mode command */
else if (strncmp(cmd, "mode ", 5) == 0) {
    // Parse: mode x1 in/out/od
    // Reconfigure GPIO_InitStruct.Mode
}

/* Pull resistor command */
else if (strncmp(cmd, "pull ", 5) == 0) {
    // Parse: pull x1 up/down/none
    // Reconfigure GPIO_InitStruct.Pull
}

/* Toggle command */
else if (strncmp(cmd, "toggle ", 7) == 0) {
    // Parse: toggle x1
    // HAL_GPIO_TogglePin()
}
```

### 3. Update MX_GPIO_Init()
Initialize X17-X22 pins as GPIO (currently only X1-X12 are initialized).

### 4. Update CLI_PrintHelp()
Add documentation for new GPIO commands.

### 5. Extend `pins` command
Show X17-X22 in addition to X1-X12.

---

# Remaining Work (TODO)

## High Priority
1. **GPIO Extended Pins** - Add X17-X22 support to read/write functions
2. **GPIO Mode/Pull Commands** - Runtime pin configuration
3. **UART CLI Commands** - Add `uart2 send/recv`, `uart6 send/recv` commands
4. **CAN CLI Commands** - Add `can send/recv` for CAN bus messaging
5. **SD Card Operations** - Add `sd init/read/write/dir` commands

## Medium Priority
6. **SPI Device Commands** - Add `spi1/spi2 transfer` commands
7. **GPIO Toggle/Pulse** - Quick pin manipulation commands
8. **PWM Output** - Add PWM generation on timer pins
9. **RTC** - Add real-time clock support

## Low Priority
10. **GPIO Interrupts** - ExtInt equivalent for pin change detection
11. **Power Management** - Sleep modes, low power
12. **Bootloader** - Custom DFU bootloader
13. **Flash Storage** - EEPROM emulation in flash

---

# Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2024 | Initial release - all core peripherals working |

---

# License

This project is provided as-is for educational purposes.
Hardware design is copyright © Damien George (MicroPython).
