# 03_gpio_input - GPIO Button Input Test

## Description

This program demonstrates GPIO input reading on the Raspberry Pi Pico 2. It monitors a button connected to GPIO 14 and reports press/release events via USB serial, with debouncing to prevent false triggers.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- Push button (normally open)
- Optional: External pull-up resistor (10kΩ) - internal pull-up is used by default

## Wiring

```
GPIO 14 ----[Button]---- GND

Optional (for external LED):
GPIO 25 ----[220Ω]----[LED]---- GND
```

**Note:** GPIO 25 is the onboard LED on most Pico boards, so no external wiring is needed for LED feedback.

## Features

- Button state detection with internal pull-up resistor
- Debouncing (50ms) to prevent false triggers
- Press/release event detection
- Press duration measurement
- Visual feedback via LED (mirrors button state)
- High-resolution timestamps (microseconds)
- Press counter

## Expected Output

Serial output:
```
========================================
  Pico 2 GPIO Button Input Test
========================================
Button Pin: GPIO 14 (with pull-up)
LED Pin: GPIO 25
Debounce Time: 50 ms
========================================

Waiting for button presses...
(Connect button between GPIO 14 and GND)

[PRESS #1] Button PRESSED at 5.234 seconds
[RELEASE #1] Button RELEASED at 5.789 seconds
              Press duration: 555.000 ms

[PRESS #2] Button PRESSED at 7.123 seconds
[RELEASE #2] Button RELEASED at 7.245 seconds
              Press duration: 122.000 ms
...
```

Visual output:
- Onboard LED turns on when button is pressed
- LED turns off when button is released

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 03_gpio_input
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/03_gpio_input/03_gpio_input.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

1. Upload the program
2. Connect to USB serial at 115200 baud
3. Connect a button between GPIO 14 and GND
4. Press and release the button
5. Verify press/release events are reported
6. Check that press duration is measured correctly
7. Observe that the onboard LED mirrors button state

## How It Works

**Pull-up Resistor:**
- Internal pull-up resistor keeps GPIO 14 HIGH when button is not pressed
- When button is pressed, GPIO 14 is pulled to GND (LOW)
- Logic is inverted in software: LOW = pressed, HIGH = not pressed

**Debouncing:**
- Mechanical buttons bounce (make/break contact rapidly) when pressed
- Software waits 50ms after detecting a change to ensure stable reading
- Prevents multiple false triggers from a single press

**State Detection:**
- Continuously monitors GPIO state in main loop
- Detects transitions (HIGH→LOW for press, LOW→HIGH for release)
- Measures duration using microsecond-resolution timer

## Customization

You can modify these parameters in the code:
- `BUTTON_PIN`: Change to use a different GPIO pin
- `DEBOUNCE_MS`: Adjust debounce time (increase for noisy buttons)
- `LED_PIN`: Change to use an external LED
- Poll interval in main loop (currently 10ms)
