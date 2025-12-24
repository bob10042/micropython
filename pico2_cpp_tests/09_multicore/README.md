# 09_multicore - Multicore (Dual-Core) Test

## Description

This program demonstrates dual-core programming on the Raspberry Pi Pico 2's RP2350 chip, which features two ARM Cortex-M33 cores. Each core runs an independent task, blinking an LED at a different rate, while demonstrating inter-core communication via the hardware FIFO.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- Optional: External LED + 220Ω resistor for Core 1 (for visual distinction)

## Wiring

### Basic (Core 0 only):
No external wiring required - uses onboard LED on GPIO 25.

### Enhanced (Both cores visible):
```
GPIO 25: Onboard LED (Core 0) - no wiring needed
GPIO 16: ----[220Ω]----[LED]---- GND  (Core 1)
               (Anode)   (Cathode)
```

## Features

- **Dual-core execution:** Separate tasks on Core 0 and Core 1
- **Independent LED blinking:** Core 0 at 1 Hz, Core 1 at 2 Hz
- **Inter-core communication:** FIFO-based message passing
- **Core identification:** Runtime detection of which core is executing
- **Performance metrics:** Blink counts and iteration tracking
- **Ratio analysis:** Verifies expected 2:1 blinking ratio
- **Synchronization demonstration:** Shows cores running simultaneously
- **Status reporting:** Detailed multi-core status display

## Expected Output

Serial output:
```
================================================
  Pico 2 Multicore (Dual-Core) Test
================================================
Chip: RP2350 (Dual ARM Cortex-M33 cores)
Current Core: Core 0
================================================

Hardware Configuration:
  Core 0 LED: GPIO 25 (onboard LED)
  Core 1 LED: GPIO 16 (external LED, optional)

If you connect an LED to GPIO 16:
  GPIO 16 ----[220Ω]----[LED]---- GND

Starting Core 1...

[CORE 1] Started successfully!
[CORE 1] Blinking LED on GPIO 16 at 500 ms intervals

[CORE 0] Core 1 launched successfully!
[CORE 0] Blinking LED on GPIO 25 at 1000 ms intervals

Both cores now running independently...
================================================
[CORE 0] LED ON  (blink #0)
[CORE 0] LED OFF (blink #0)
[CORE 0] LED ON  (blink #1)
[CORE 0] LED OFF (blink #1)
...
[CORE 0] Received from Core 1: 10 blinks completed
...

========== Status ==========
Time: 5.234 seconds

Core 0:
  Running on: Core 0
  LED Pin: GPIO 25
  Blink Rate: 1000 ms
  Blinks: 5

Core 1:
  LED Pin: GPIO 16
  Blink Rate: 500 ms
  Blinks: 10
  Iterations: 10
============================

[CORE 0] Communication Test:
  Core 0 has blinked 10 times
  Core 1 has blinked 20 times (ratio: 2.00:1)
  Expected ratio: 2:1 (Core 1 blinks twice as fast)
```

Visual output:
- GPIO 25 (onboard LED): Blinks at 1 Hz
- GPIO 16 (external LED): Blinks at 2 Hz (twice as fast)
- Both LEDs blink simultaneously and independently

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 09_multicore
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/09_multicore/09_multicore.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

### Test 1: Core 0 Only (Basic)
1. Upload program without external LED
2. Connect to USB serial
3. Observe onboard LED blinking at 1 Hz
4. Verify serial output shows both cores running
5. Check that Core 1 blink count increases (even without visible LED)

### Test 2: Both Cores (Visual)
1. Connect external LED to GPIO 16
2. Upload program
3. Observe both LEDs blinking at different rates
4. Verify 2:1 ratio visually (Core 1 blinks twice for each Core 0 blink)
5. Check serial output confirms ratio

### Test 3: Inter-Core Communication
1. Monitor serial output
2. Wait for "Received from Core 1" messages
3. Verify messages arrive every 10 Core 1 blinks
4. Check that blink counts match expected values

## How It Works

**RP2350 Dual-Core Architecture:**
- Two ARM Cortex-M33 processors
- Cores operate independently
- Shared memory space (RAM, peripherals)
- Each core can access all GPIOs
- Separate stack for each core
- Hardware synchronization primitives

**Core Startup Sequence:**
1. Core 0 boots first (runs main())
2. Core 0 initializes hardware
3. Core 0 launches Core 1 with `multicore_launch_core1()`
4. Core 1 begins executing at specified entry point
5. Both cores run simultaneously

**Inter-Core FIFO:**
- Hardware FIFO connects cores
- 8 entries deep (32-bit each)
- Bidirectional communication
- Blocking and non-blocking access
- Atomic operations (no locks needed)
- Used for passing messages, synchronization

**Memory Sharing:**
- Both cores access same RAM
- Careful with shared variables
- Use `volatile` for shared data
- Consider race conditions
- Hardware FIFO provides safe communication

**This Program's Architecture:**
- **Core 0 (main):**
  - Initializes system
  - Launches Core 1
  - Blinks LED at 1 Hz
  - Receives status from Core 1
  - Prints debug output
- **Core 1 (core1_entry):**
  - Blinks LED at 2 Hz
  - Updates shared counters
  - Sends periodic status to Core 0
  - Runs indefinitely

## Customization

You can modify these parameters in the code:
- `LED_CORE0_PIN` / `LED_CORE1_PIN`: Use different GPIO pins
- `CORE0_BLINK_MS` / `CORE1_BLINK_MS`: Change blink rates
- Add more complex tasks to each core
- Implement bidirectional FIFO communication
- Add critical sections for shared data
- Use hardware spinlocks for synchronization

**Example: Different blink rates:**
```cpp
const uint32_t CORE0_BLINK_MS = 2000;  // 0.5 Hz
const uint32_t CORE1_BLINK_MS = 200;   // 5 Hz
```

## Common Multicore Patterns

### Pattern 1: Task Distribution
```cpp
// Core 0: User interface, communication
// Core 1: Heavy computation, signal processing
```

### Pattern 2: Parallel Processing
```cpp
// Core 0: Process first half of data
// Core 1: Process second half of data
// Combine results after both complete
```

### Pattern 3: Producer-Consumer
```cpp
// Core 0: Acquires data (ADC, sensors)
// Core 1: Processes data
// FIFO: Data buffer between cores
```

### Pattern 4: Real-Time + Background
```cpp
// Core 0: Time-critical interrupt handling
// Core 1: Background tasks, housekeeping
```

## Synchronization Primitives

**Available on RP2350:**

1. **Inter-Core FIFO** (this program)
   - Simple message passing
   - No locking needed
   - Limited capacity (8 entries)

2. **Hardware Spinlocks**
   - Mutual exclusion
   - Protect shared resources
   - 32 available locks

3. **Semaphores** (via SDK)
   - Higher-level synchronization
   - Can use spinlocks internally

4. **Critical Sections** (via SDK)
   - Disable interrupts
   - Short duration only
   - Blocks both cores if needed

**Example: Using Spinlock**
```cpp
#include "hardware/sync.h"

auto_init_mutex(my_mutex);

void core0_task() {
    mutex_enter_blocking(&my_mutex);
    // Access shared resource
    mutex_exit(&my_mutex);
}

void core1_task() {
    mutex_enter_blocking(&my_mutex);
    // Access shared resource
    mutex_exit(&my_mutex);
}
```

## Performance Considerations

**Advantages of Dual-Core:**
- Nearly 2x performance for parallel tasks
- Dedicated core for time-critical operations
- Separate interrupt handling per core
- Can overlap I/O with computation

**Limitations:**
- Memory bandwidth shared between cores
- Cache coherency overhead (RP2350 has no cache, so N/A)
- FIFO synchronization overhead
- Not all tasks parallelize well

**Best Practices:**
- Minimize shared memory access
- Use FIFO for communication
- Keep critical sections short
- Profile to identify bottlenecks
- Consider cache effects (future RP chips)

## Troubleshooting

**Core 1 doesn't start:**
- Check `multicore_launch_core1()` is called
- Verify entry function signature is correct
- Ensure Core 1 entry doesn't return immediately
- Check for stack overflow on Core 1

**Shared data corruption:**
- Use `volatile` keyword
- Add proper synchronization (mutex/spinlock)
- Check for race conditions
- Verify atomic operations where needed

**FIFO overflow/underflow:**
- Check FIFO status before push/pop
- Use blocking versions for reliability
- Drain FIFO if it fills up
- Consider increasing message frequency

**Unexpected behavior:**
- Verify correct core is executing code
- Use `get_core_num()` for debugging
- Check for unintended interactions
- Review interrupt assignment

## Advanced Topics

**Core Affinity:**
- Pin interrupts to specific core
- Optimize cache usage (future chips)
- Balance load between cores

**DMA Coordination:**
- DMA operates independently of cores
- Can reduce core workload
- Coordinate with interrupts

**Power Management:**
- Can put one core to sleep
- Reduces power consumption
- Wake on interrupt or FIFO

**Debugging:**
- Separate debug channels per core
- Use SWD debugger for step debugging
- Logic analyzer for timing analysis
- Printf with core number prefix

## Real-World Applications

**Example Use Cases:**
1. **Signal Processing:** Core 0 acquires audio, Core 1 performs FFT
2. **Motor Control:** Core 0 handles PID loop, Core 1 manages communication
3. **Video Processing:** Core 0 captures frames, Core 1 compresses data
4. **Robotics:** Core 0 sensor fusion, Core 1 path planning
5. **Network:** Core 0 protocol stack, Core 1 application logic
