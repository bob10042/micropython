# 02_serial_hello - USB Serial "Hello World" Test

## Description

This program demonstrates USB serial communication on the Raspberry Pi Pico 2. It prints system information at startup and then continuously outputs formatted messages demonstrating various printf() formatting options.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- USB cable for power and serial communication
- No external components required

## Wiring

No external wiring required. Uses USB connection for both power and serial communication.

## Features

- System information display (chip type, clock frequency, unique board ID)
- ROM version detection
- High-resolution timestamp (microseconds since boot)
- Multiple printf() formatting demonstrations:
  - Decimal, hexadecimal, and binary number formatting
  - Floating-point formatting
  - String and character output
- Running counter with timestamps

## Expected Output

Serial output:
```
========================================
  Raspberry Pi Pico 2 - System Info
========================================
Chip: RP2350
ROM Version: 2
System Clock: 125000000 Hz (125 MHz)
Board ID: E66058A513B7A52E
========================================

Starting continuous output...
Press Ctrl+C to stop (if connected via terminal)

--- Message #0 ---
Hello from Raspberry Pi Pico 2!
Time since boot: 2000000 us (2.000 seconds)
Counter (decimal): 0
Counter (hex): 0x00000000
Counter (binary): 0b00000000_00000000_00000000_00000000
Reference voltage: 3.30 V
Status: O (Running)

--- Message #1 ---
...
```

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 02_serial_hello
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/02_serial_hello/02_serial_hello.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

1. Upload the program
2. Connect to USB serial at 115200 baud (e.g., using `minicom`, PuTTY, or Arduino Serial Monitor)
3. Observe system information printed once at startup
4. Verify continuous messages every 2 seconds
5. Check that timestamps increase correctly
6. Verify all formatting options display correctly

## Notes

- The program waits 2 seconds after stdio initialization to ensure USB serial connection is established
- Timestamps are in microseconds since boot, providing high-resolution timing
- The unique board ID is specific to each Pico 2 and can be used for identification
