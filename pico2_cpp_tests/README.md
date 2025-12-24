# Raspberry Pi Pico 2 (RP2350) C++ Test Suite

A comprehensive collection of test programs demonstrating the hardware capabilities of the Raspberry Pi Pico 2 with the RP2350 microcontroller.

## Overview

This test suite provides ready-to-compile C++ examples for all major hardware features of the Pico 2, including GPIO, PWM, ADC, UART, I2C, SPI, multicore processing, and timer interrupts. Each test is self-contained with detailed documentation and wiring instructions.

## Hardware Requirements

- **Raspberry Pi Pico 2** (RP2350 chip)
- **USB cable** (for power and programming)
- **Optional components** depending on test:
  - LEDs and resistors
  - Push buttons
  - Potentiometer
  - I2C devices (sensors, displays)
  - USB-to-Serial adapter
  - Breadboard and jumper wires

## Test Programs

| # | Name | Description | External Hardware |
|---|------|-------------|-------------------|
| 01 | [blink](01_blink/) | Basic LED blink | None (onboard LED) |
| 02 | [serial_hello](02_serial_hello/) | USB serial communication | None |
| 03 | [gpio_input](03_gpio_input/) | Button input with debouncing | Push button |
| 04 | [pwm_led](04_pwm_led/) | PWM LED fading | LED + resistor (optional) |
| 05 | [adc_read](05_adc_read/) | Analog input + temp sensor | Potentiometer (optional) |
| 06 | [uart_echo](06_uart_echo/) | UART serial echo | USB-Serial or jumper wire |
| 07 | [i2c_scan](07_i2c_scan/) | I2C bus scanner | I2C device (optional) |
| 08 | [spi_test](08_spi_test/) | SPI loopback test | Jumper wire |
| 09 | [multicore](09_multicore/) | Dual-core processing | LED + resistor (optional) |
| 10 | [timer_interrupt](10_timer_interrupt/) | Hardware timer interrupts | None |

## Quick Start

### Prerequisites

1. **Pico SDK** installed and configured
2. **CMake** (version 3.13 or later)
3. **ARM GCC toolchain** (arm-none-eabi-gcc)
4. **Build tools** (Make or Ninja)

### Environment Setup

Set the Pico SDK path (if not already set):

**Linux/macOS:**
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

**Windows (Command Prompt):**
```cmd
set PICO_SDK_PATH=C:\path\to\pico-sdk
```

**Windows (PowerShell):**
```powershell
$env:PICO_SDK_PATH="C:\path\to\pico-sdk"
```

### Building All Tests

```bash
cd pico2_cpp_tests
mkdir build
cd build
cmake ..
make -j4
```

This will build all 10 test programs. Output files (.uf2) will be in:
```
build/01_blink/01_blink.uf2
build/02_serial_hello/02_serial_hello.uf2
...
build/10_timer_interrupt/10_timer_interrupt.uf2
```

### Building Individual Tests

```bash
cd pico2_cpp_tests
mkdir build
cd build
cmake ..
make 01_blink      # Build only the blink test
make 05_adc_read   # Build only the ADC test
```

### Uploading to Pico 2

1. Hold the **BOOTSEL** button on the Pico
2. Connect USB cable to computer (while holding BOOTSEL)
3. Release BOOTSEL - Pico appears as USB drive "RPI-RP2"
4. Copy the `.uf2` file to the RPI-RP2 drive
5. Pico automatically reboots and runs the program

**Example:**
```bash
cp build/01_blink/01_blink.uf2 /media/RPI-RP2/
```

## Detailed Documentation

Each test program has its own README.md with:
- Hardware requirements and wiring diagrams
- Expected output (serial and visual)
- Detailed explanation of concepts
- Customization options
- Troubleshooting guide

See individual test directories for details.

## Recommended Learning Path

**Beginner (Start Here):**
1. **01_blink** - Simplest program, verifies setup
2. **02_serial_hello** - Learn USB serial communication
3. **03_gpio_input** - Digital input with buttons
4. **04_pwm_led** - Analog output using PWM

**Intermediate:**
5. **05_adc_read** - Analog input and sensors
6. **06_uart_echo** - Serial communication protocols
7. **07_i2c_scan** - I2C bus and sensors

**Advanced:**
8. **08_spi_test** - SPI communication
9. **09_multicore** - Dual-core programming
10. **10_timer_interrupt** - Interrupt-driven design

## Common Issues and Solutions

### Build Issues

**Problem:** `PICO_SDK_PATH not defined`
```
Solution: Set environment variable pointing to Pico SDK
export PICO_SDK_PATH=/path/to/pico-sdk
```

**Problem:** `arm-none-eabi-gcc not found`
```
Solution: Install ARM GCC toolchain
- Ubuntu: sudo apt install gcc-arm-none-eabi
- macOS: brew install gcc-arm-embedded
- Windows: Download from ARM website
```

**Problem:** `CMake version too old`
```
Solution: Install CMake 3.13 or later
- Ubuntu: sudo apt install cmake
- macOS: brew install cmake
- Windows: Download from cmake.org
```

### Upload Issues

**Problem:** Pico doesn't appear as USB drive
```
Solution:
1. Unplug USB cable
2. Hold BOOTSEL button
3. Plug in USB while holding BOOTSEL
4. Release BOOTSEL after 1 second
```

**Problem:** .uf2 file doesn't upload
```
Solution:
1. Verify file is .uf2 format (not .elf or .bin)
2. Check file isn't corrupted (rebuild if necessary)
3. Try different USB cable/port
```

### Runtime Issues

**Problem:** No serial output
```
Solution:
1. Connect to correct COM port (check Device Manager)
2. Set baud rate to 115200
3. Wait 2-3 seconds after reset for USB to initialize
```

**Problem:** Program doesn't run after upload
```
Solution:
1. Press RESET button on Pico
2. Re-upload .uf2 file
3. Check for compile errors in original build
```

## Serial Terminal Setup

To view program output, connect a serial terminal:

**Linux:**
```bash
sudo apt install minicom
minicom -D /dev/ttyACM0 -b 115200
```

**macOS:**
```bash
screen /dev/tty.usbmodem* 115200
```

**Windows:**
- PuTTY: Set COM port, 115200 baud, connection type: Serial
- Arduino IDE: Tools → Serial Monitor → 115200 baud
- Tera Term: Setup → Serial Port → COM port, 115200 baud

## Project Structure

```
pico2_cpp_tests/
├── pico_sdk_import.cmake    # Pico SDK integration
├── CMakeLists.txt            # Root build configuration
├── README.md                 # This file
├── BUILD_INSTRUCTIONS.md     # Detailed build guide
├── 01_blink/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   └── README.md
├── 02_serial_hello/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   └── README.md
... (etc for all 10 tests)
```

## Features Demonstrated

### GPIO (General Purpose I/O)
- Digital output (LED control)
- Digital input (button reading)
- Internal pull-up/pull-down resistors
- Debouncing techniques

### PWM (Pulse Width Modulation)
- LED brightness control
- Hardware PWM generation
- Gamma correction
- Frequency and duty cycle control

### ADC (Analog-to-Digital Converter)
- Analog voltage reading
- Internal temperature sensor
- Multi-sample averaging
- Voltage conversion

### UART (Serial Communication)
- Asynchronous serial communication
- Configurable baud rates
- Echo/loopback testing
- Dual serial ports (USB + UART)

### I2C (Inter-Integrated Circuit)
- Device scanning
- Pull-up resistors
- Standard and fast mode
- Common device identification

### SPI (Serial Peripheral Interface)
- Full-duplex communication
- Loopback testing
- Multiple SPI modes
- High-speed data transfer

### Multicore Processing
- Dual-core ARM Cortex-M33
- Independent task execution
- Inter-core FIFO communication
- Shared memory access

### Timer Interrupts
- Hardware timer precision
- Interrupt service routines (ISR)
- Non-blocking operation
- Timing accuracy analysis

## RP2350 Specifications

- **CPU:** Dual ARM Cortex-M33 @ 150 MHz (default 125 MHz)
- **RAM:** 520 KB SRAM
- **Flash:** 2 MB (on Pico 2 board)
- **GPIO:** 30 programmable I/O pins
- **ADC:** 12-bit, 4 channels + temperature sensor
- **PWM:** 12 PWM channels
- **UART:** 2 peripherals
- **I2C:** 2 peripherals
- **SPI:** 2 peripherals
- **USB:** 1.1 host/device
- **Security:** Arm TrustZone, secure boot

## Additional Resources

### Official Documentation
- [Raspberry Pi Pico 2 Datasheet](https://datasheets.raspberrypi.com/pico/pico-2-datasheet.pdf)
- [RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf)
- [Pico SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/)
- [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)

### Community Resources
- [Raspberry Pi Forums](https://forums.raspberrypi.com/)
- [GitHub Pico Examples](https://github.com/raspberrypi/pico-examples)
- [Pico SDK GitHub](https://github.com/raspberrypi/pico-sdk)

## License

These test programs are provided as educational examples for the Raspberry Pi Pico 2.
Feel free to use, modify, and distribute as needed.

## Contributing

Improvements and additional tests are welcome! Consider adding:
- More complex peripheral examples
- DMA demonstrations
- PIO (Programmable I/O) examples
- Networking examples (if using Pico 2 W)
- Power management examples
- Security features (TrustZone)

## Acknowledgments

- Raspberry Pi Foundation for the Pico 2 and RP2350
- ARM for Cortex-M33 architecture
- Community contributors to Pico SDK

## Version History

- **v1.0** (2025-11-22): Initial release with 10 core test programs

---

**Happy coding with Raspberry Pi Pico 2!**
