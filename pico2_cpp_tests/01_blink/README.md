# 01_blink - Basic LED Blink Test

## Description

This is the simplest test program for the Raspberry Pi Pico 2. It blinks the onboard LED at a 1 Hz rate (500ms on, 500ms off) and outputs status messages via USB serial.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- No external components required (uses onboard LED)

## Wiring

No external wiring required. The program uses the onboard LED connected to GPIO 25.

## Features

- Blinks LED at 1 Hz (configurable interval)
- Prints LED state to USB serial
- Displays blink counter
- Simple error-free GPIO control

## Expected Output

Serial output:
```
=================================
Pico 2 LED Blink Test
=================================
LED Pin: GPIO 25
Blink Rate: 500 ms ON, 500 ms OFF
=================================

LED ON  (count: 0)
LED OFF (count: 0)
LED ON  (count: 1)
LED OFF (count: 1)
...
```

Visual output:
- Onboard LED blinks at 1 Hz

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 01_blink
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/01_blink/01_blink.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

1. Upload the program
2. Connect to USB serial (e.g., using `minicom`, PuTTY, or Arduino Serial Monitor)
3. Observe the LED blinking
4. Verify serial output matches expected format
