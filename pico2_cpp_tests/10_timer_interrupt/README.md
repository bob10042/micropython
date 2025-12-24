# 10_timer_interrupt - Hardware Timer Interrupt Test

## Description

This program demonstrates hardware timer interrupts on the Raspberry Pi Pico 2. A repeating timer generates precise interrupts at 1 Hz, toggling an LED in the interrupt service routine (ISR) while the main loop continues running independently. This showcases interrupt-driven programming and the RP2350's accurate hardware timers.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- No external components required (uses onboard LED)

## Wiring

No external wiring required. The program uses GPIO 25 (onboard LED).

**Optional:** Add external LED for clearer visual feedback:
```
GPIO 25 ----[220Ω]----[LED]---- GND
```

## Features

- **Hardware timer interrupts** at precise 1 Hz interval
- **Non-blocking operation:** Main loop runs independently
- **Interrupt Service Routine (ISR):** Toggles LED
- **High-precision timing:** Microsecond-resolution timer
- **Timing statistics:** Average, min, max intervals
- **Jitter analysis:** Measures timing variation
- **Accuracy assessment:** Reports timing error percentage
- **Real-time monitoring:** Main loop displays interrupt status
- **Volatile variables:** Demonstrates proper ISR communication

## Expected Output

Serial output:
```
================================================
  Pico 2 Hardware Timer Interrupt Test
================================================
Timer Interval: 1000000 us (1.000 seconds)
Timer Frequency: 1.000 Hz
LED Pin: GPIO 25
================================================

Initializing hardware timer...
Timer started successfully!
Interrupt service routine (ISR) will toggle LED every 1.000 seconds.
Main loop continues running independently.

Press Ctrl+C to stop.
================================================

[Main Loop] Interrupts:     0 | LED: OFF | Last: 0.000 s
[Main Loop] Interrupts:     0 | LED: OFF | Last: 0.000 s
[Main Loop] Interrupts:     1 | LED: ON  | Last: 3.002 s
[Main Loop] Interrupts:     1 | LED: ON  | Last: 3.002 s
[Main Loop] Interrupts:     2 | LED: OFF | Last: 4.002 s
[Main Loop] Interrupts:     3 | LED: ON  | Last: 5.002 s
...
[Main Loop] Interrupts:    10 | LED: OFF | Last: 12.002 s

========== Interrupt Statistics ==========
Total interrupts: 10
LED state: OFF
Time since boot: 12.123 seconds

Timing Analysis:
  Expected interval: 1000000 us (1000.000 ms)
  Average interval:  999999.889 us (999.999 ms)
  Minimum interval:  999998 us (999.998 ms)
  Maximum interval:  1000002 us (1000.002 ms)
  Timing error:      -0.111 us (-0.000011%)
  Jitter:            4 us

Accuracy Assessment:
  EXCELLENT: Timing error < 0.001%
==========================================

[Main Loop] Interrupts:    11 | LED: ON  | Last: 13.002 s
[Main Loop] Interrupts:    12 | LED: OFF | Last: 14.002 s
[Main Loop] Still running... (iteration 150)
...
```

Visual output:
- Onboard LED blinks at exactly 1 Hz
- Very stable, precise timing (no drift)

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 10_timer_interrupt
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/10_timer_interrupt/10_timer_interrupt.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

### Test 1: Basic Functionality
1. Upload program
2. Connect to USB serial
3. Observe LED blinking at 1 Hz
4. Verify main loop messages continue printing
5. Confirm interrupt count increases

### Test 2: Timing Accuracy
1. Wait for 10+ interrupts
2. Review timing statistics
3. Verify timing error < 0.001%
4. Check jitter is minimal (< 10 µs typical)
5. Compare expected vs actual intervals

### Test 3: Long-Term Stability
1. Let program run for several minutes
2. Check that timing doesn't drift
3. Verify interrupt count matches elapsed time
4. Confirm no missed interrupts

### Test 4: Main Loop Independence
1. Observe "Still running..." messages
2. Verify they appear between interrupt messages
3. Confirms main loop and ISR run independently
4. Demonstrates non-blocking interrupt operation

## How It Works

**Hardware Timer:**
- RP2350 has one 64-bit microsecond timer
- Shared by both cores
- Can generate multiple alarms (interrupts)
- Extremely accurate (driven by crystal oscillator)
- Automatically reloads for repeating interrupts

**Interrupt Service Routine (ISR):**
- Function called automatically by hardware
- Runs in interrupt context (higher priority than main)
- Preempts main loop execution
- Should be fast (microseconds typical)
- Avoid blocking operations (no printf, sleep, etc.)

**This Program's Flow:**
1. Main sets up timer with 1-second interval
2. Timer hardware counts microseconds
3. When interval expires, interrupt fires
4. ISR executes: toggles LED, updates counters
5. ISR returns, main loop resumes
6. Timer automatically reloads and continues

**Timing Accuracy:**
- Hardware timer driven by 12 MHz crystal
- PLL multiplies to 125 MHz system clock
- Timer counts at 1 MHz (microsecond resolution)
- Jitter typically < 1 µs
- No cumulative drift (hardware-based)

**Volatile Keyword:**
```cpp
volatile uint32_t interrupt_count = 0;
```
- Tells compiler variable may change unexpectedly
- Prevents optimization that could hide changes
- Essential for ISR-modified variables
- Main loop always reads latest value

## Customization

You can modify these parameters in the code:
- `TIMER_INTERVAL_US`: Change interrupt frequency
- `LED_PIN`: Use different GPIO pin
- Add multiple timers with different callbacks
- Implement more complex ISR logic
- Add edge detection or pulse measurement

**Example: 10 Hz interrupt (100ms interval):**
```cpp
const uint32_t TIMER_INTERVAL_US = 100000;  // 100 ms
```

**Example: Fast interrupt (10 kHz):**
```cpp
const uint32_t TIMER_INTERVAL_US = 100;  // 100 µs (10 kHz)
```

## Interrupt Frequencies

**Practical ranges:**
- Very slow: > 1 second (battery monitoring)
- Slow: 1-100 Hz (user interface, sensors)
- Medium: 100-1000 Hz (control loops, audio)
- Fast: 1-10 kHz (PWM, signal processing)
- Very fast: 10-100 kHz (communication protocols)
- Maximum: < 1 MHz (ISR overhead limits)

**ISR Execution Time:**
- This ISR: ~2-3 µs (simple operations)
- Maximum safe frequency: ~100 kHz (1% CPU)
- At 1 kHz: negligible CPU usage
- At 10 kHz: ~0.1% CPU usage

## ISR Best Practices

**DO:**
- Keep ISR short and fast
- Use volatile for shared variables
- Set flags for main loop to process
- Update simple counters
- Toggle GPIO pins
- Read hardware registers

**DON'T:**
- Call printf() or other slow functions
- Use sleep() or busy-wait loops
- Perform complex calculations
- Access slow peripherals (flash writes)
- Allocate memory
- Call non-reentrant functions

**Example: Flag-based processing:**
```cpp
volatile bool data_ready = false;

bool isr_callback(struct repeating_timer *t) {
    // Just set flag
    data_ready = true;
    return true;
}

int main() {
    while (true) {
        if (data_ready) {
            // Process in main loop (not ISR)
            process_data();
            data_ready = false;
        }
    }
}
```

## Advanced Timer Features

### Multiple Timers:
```cpp
struct repeating_timer timer1, timer2;

add_repeating_timer_us(1000000, callback1, NULL, &timer1);  // 1 Hz
add_repeating_timer_us(500000, callback2, NULL, &timer2);   // 2 Hz
```

### One-Shot Timer:
```cpp
add_alarm_in_us(5000000, one_shot_callback, NULL, false);  // Fire once in 5s
```

### Absolute Time Alarm:
```cpp
uint64_t target_time = time_us_64() + 10000000;  // 10 seconds from now
add_alarm_at(target_time, callback, NULL, false);
```

### Cancel Timer:
```cpp
bool cancelled = cancel_repeating_timer(&timer);
```

## Timing Measurement Example

**Measure execution time:**
```cpp
uint64_t start = time_us_64();
// ... code to measure ...
uint64_t end = time_us_64();
uint64_t elapsed = end - start;
printf("Execution time: %llu us\n", elapsed);
```

**Precise delays:**
```cpp
absolute_time_t target = make_timeout_time_us(1000);  // 1 ms from now
sleep_until(target);  // Sleep until exact time
```

## Troubleshooting

**Interrupt doesn't fire:**
- Verify timer created successfully
- Check callback function signature
- Ensure callback returns true
- Verify timer not cancelled

**Timing inaccurate:**
- Check ISR execution time (should be short)
- Verify system clock frequency
- Look for other high-priority interrupts
- Disable unnecessary interrupts

**Main loop blocks:**
- Ensure sleep() not in ISR
- Check for infinite loops in ISR
- Verify printf() not in ISR
- Look for mutex deadlocks

**LED doesn't blink:**
- Check GPIO initialization
- Verify pin number correct
- Test LED separately
- Check ISR actually running (add counter)

**Jitter too high:**
- Reduce ISR execution time
- Disable interrupts during critical sections
- Check for DMA or other interrupt sources
- Use higher priority for timer interrupt

## Real-World Applications

**Example Use Cases:**

1. **Periodic Sampling:**
   - ADC sampling at fixed rate
   - Sensor reading
   - Data acquisition

2. **Control Loops:**
   - PID controllers
   - Motor control
   - Temperature regulation

3. **Communication Timing:**
   - Bit-banging protocols
   - Watchdog timers
   - Timeout detection

4. **Event Generation:**
   - PWM generation (though hardware PWM preferred)
   - Pulse generation
   - Clock outputs

5. **Time-Stamping:**
   - Data logging
   - Event recording
   - Performance profiling

## Comparison with Other Timing Methods

| Method | Accuracy | CPU Usage | Complexity |
|--------|----------|-----------|------------|
| sleep_ms() | Low (~1ms) | Blocks | Simple |
| Polling time_us_64() | Medium | High | Medium |
| Hardware Timer ISR | Excellent (<1µs) | Very Low | Medium |
| Hardware PWM | Perfect (0 jitter) | Zero | Low |
| DMA-triggered | Excellent | Zero | Complex |

**Recommendation:** Use hardware timer interrupts for precise, periodic tasks where timing is critical but frequency is < 100 kHz.
