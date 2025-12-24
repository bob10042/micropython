# 04_pwm_led - PWM LED Fading Test

## Description

This program demonstrates hardware PWM (Pulse Width Modulation) control on the Raspberry Pi Pico 2. It smoothly fades an LED in and out using the RP2350's PWM hardware, with optional gamma correction for perceptually linear brightness.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- LED (any color)
- Current-limiting resistor (220Ω - 1kΩ depending on LED)
- Breadboard and jumper wires

## Wiring

```
GPIO 15 ----[220Ω]----[LED]---- GND
              (Anode)  (Cathode)
```

**Alternative:** You can change `LED_PIN` to 25 to use the onboard LED (no external components needed).

**Pin Selection:**
- Any GPIO pin can be used for PWM
- Each PWM slice controls 2 GPIO pins
- GPIO 15 is on PWM slice 7, channel B

## Features

- Hardware PWM generation at 1 kHz
- Smooth LED fading (256 brightness levels)
- Gamma correction for perceptually linear brightness
- Configurable fade speed
- Duty cycle percentage reporting via USB serial
- Continuous fade in/out cycle

## Expected Output

Serial output:
```
========================================
  Pico 2 PWM LED Fading Test
========================================
LED Pin: GPIO 15
PWM Frequency: 1000 Hz
Brightness Steps: 256 (0-255)
Fade Step Delay: 10 ms
Gamma Correction: Enabled
========================================

PWM initialized on GPIO 15 (slice 7)
PWM Frequency: 1000 Hz
Clock Divider: 1.91

Starting fade cycle...

--- Cycle #0 ---
Fading IN:    0% brightness
Fading IN:   10% brightness
Fading IN:   20% brightness
...
Fading IN:  100% brightness
Fading OUT: 100% brightness
Fading OUT:  90% brightness
...
Fading OUT:   0% brightness

--- Cycle #1 ---
...
```

Visual output:
- LED smoothly fades from off to full brightness
- LED smoothly fades from full brightness to off
- Cycle repeats continuously

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 04_pwm_led
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/04_pwm_led/04_pwm_led.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

1. Upload the program
2. Connect to USB serial at 115200 baud
3. Connect an LED with resistor to GPIO 15 and GND
4. Observe smooth fading in and out
5. Verify serial output shows brightness percentages
6. Check that fading appears linear (thanks to gamma correction)

## How It Works

**PWM (Pulse Width Modulation):**
- Rapidly switches LED on/off at 1 kHz
- Duty cycle (% of time on) controls brightness
- 0% duty cycle = LED off
- 100% duty cycle = LED fully on
- Human eye perceives average brightness due to persistence of vision

**Hardware PWM:**
- RP2350 has 12 PWM slices (can control 24 GPIOs)
- Each slice has 16-bit resolution (65536 levels)
- Hardware-generated, no CPU overhead
- Precise timing, no jitter

**Gamma Correction:**
- Human eye perceives brightness logarithmically
- Linear duty cycle doesn't appear linear to eye
- Gamma correction (γ=2.2) makes fading appear smooth
- Formula: corrected = (linear)^2.2

**Fade Implementation:**
- 256 brightness steps (0-255)
- 10ms delay between steps
- Full fade in: 2.56 seconds
- Full fade out: 2.56 seconds
- Total cycle: ~5.6 seconds (including hold periods)

## Customization

You can modify these parameters in the code:
- `LED_PIN`: Change to use a different GPIO pin
- `PWM_FREQUENCY`: Adjust PWM frequency (typical range: 100 Hz - 10 kHz)
- `PWM_STEPS`: Number of brightness levels (more = smoother, slower)
- `FADE_STEP_MS`: Delay between steps (lower = faster fading)
- `USE_GAMMA_CORRECTION`: Enable/disable gamma correction
- Gamma value in `apply_gamma()` function (typical: 2.0 - 2.5)

## Technical Notes

**PWM Frequency Selection:**
- Too low (<100 Hz): Visible flickering
- Good range: 500 Hz - 2 kHz (flicker-free, efficient)
- Very high (>10 kHz): Reduced resolution, potential EMI

**LED Forward Voltage:**
- Red/Yellow: ~2.0V → use 220Ω resistor
- Green/Blue/White: ~3.0V → use 100Ω resistor
- Calculate: R = (3.3V - Vf) / 0.010A

**PWM Slice Mapping:**
- GPIO 0-1: Slice 0
- GPIO 2-3: Slice 1
- ...
- GPIO 14-15: Slice 7
- GPIO 16-17: Slice 0 (wraps around)
