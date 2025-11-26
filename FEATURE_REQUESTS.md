# MicroPython Feature Requests & Enhancements

This is a sampling of enhancement requests from the MicroPython issue tracker.
These are NOT bugs, but features that users have requested.

## Recent Enhancement Requests

### Tools & Development

**#18420 - Add checksum verification to mpremote file transfers**
- Status: Active discussion, PR submitted
- Request: Verify file integrity after upload to MCU
- Use case: Prevent corruption during serial transfer over USB/UART
- Comments: 5 (maintainers engaging)

### Hardware Support

**#18411 - EXT_GPIO1 on RM2 module configuration**
- Platform: RP2 (Pico W with RM2 WiFi module)
- Request: Allow configuring EXT_GPIO1 as input (currently forced to output)
- Request: Expose internal pull-up/pull-down functionality

**#18281 - PSRAM support for Teensy 4.1**
- Platform: mimxrt (NXP i.MX RT)
- Request: Add support for PSRAM chips on Teensy 4.1
- Status: PR submitted (#18288), working implementation exists!
- Potential: 8-16MB additional RAM

**#18213 - ESP32-C6 USB pin control**
- Platform: ESP32-C6
- Issue: USB pins disabled even when USB not in use
- Request: Allow using USB pins as GPIO when USB is disabled via efuse

**#18192 - PDM microphone support for Zephyr**
- Platform: Zephyr (NRF52840)
- Request: Add PDM (Pulse Density Modulation) microphone support
- Use case: Built-in microphones on boards like XIAO BLE Sense
- Status: Draft PR submitted (#18287)

**#17877 - Enable PPP by default on RP2/Pico W**
- Platform: RP2 (Raspberry Pi Pico W)
- Request: Include PPP (Point-to-Point Protocol) in default builds
- Use case: Cellular modems, serial internet connectivity
- Status: Previously submitted PR (#16445), being updated
- Note: Already available as custom build option

**#18088 - ESP32-C6 Zigbee support**
- Platform: ESP32-C6
- Request: Add Zigbee protocol support for ESP32-C6
- Use case: Smart home, IoT mesh networking

### API Enhancements

**#18385 - Update aiohttp to newer version**
- Request: Update micropython-lib's aiohttp implementation
- Note: MicroPython's aiohttp is separate from CPython's version

**#17996 - I2CTarget: Add bytes read/written counter**
- Request: Track number of bytes transferred in I2C target mode
- Use case: Protocol debugging, performance monitoring

**#17878 - Zephyr: Support for USER_C_MODULES**
- Platform: Zephyr
- Request: Enable C module extensions on Zephyr port
- Status: PR submitted (#18030)
- Use case: Custom hardware drivers, optimized code

### Developer Experience

**#18340 - ESP32: Support 64-bit printf in embedded targets**
- Platform: ESP32
- Request: Enable `%llu` format specifier for printing 64-bit integers
- Trade-off: Code size vs convenience
- Current workaround: Divide large numbers down to 32-bit range

**#18319 - Unix port: Dynamic linking against libmbedtls**
- Platform: Unix/Linux
- Request: Allow linking dynamically instead of static compilation
- Use case: System-wide TLS library updates, smaller binaries

## Common Themes in Feature Requests

### 1. **Platform-Specific Hardware**
Many requests are for exposing hardware features:
- PSRAM, SDRAM expansion
- USB configuration control
- Wireless protocols (Zigbee, PPP)
- Audio interfaces (I2S, PDM microphones)

### 2. **Developer Tools**
Requests for better development experience:
- File transfer verification
- Better debugging (64-bit printf, byte counters)
- Build system improvements

### 3. **Ecosystem Compatibility**
Requests to match other platforms:
- Library updates (aiohttp)
- C module support across ports
- Dynamic linking options

### 4. **Memory Expansion**
High demand for external RAM support:
- PSRAM for Teensy, RP2
- SDRAM for mimxrt
- Allows larger applications

## Why Some Requests Take Time

### Resource Constraints
- **Code size**: Many features add binary size (issue on small MCUs)
- **RAM usage**: Features may need runtime memory
- **Example**: 64-bit printf adds code but most users don't need it

### Maintainer Bandwidth
- Small team reviewing hundreds of requests
- Must balance features vs maintenance burden
- Platform-specific features need hardware to test

### Community-Driven
- Many features implemented by community members
- PRs often submitted alongside requests
- Examples: PSRAM (#18288), PDM mic (#18287), Zephyr modules (#18030)

## How Features Get Implemented

**Successful pattern:**
1. User opens issue with clear use case
2. Discussion with maintainers about approach
3. Community member submits PR
4. Review and iteration
5. Merge when ready

**Examples in progress:**
- #18281 (PSRAM) → PR #18288 submitted same day!
- #17878 (USER_C_MODULES) → PR #18030 working through review
- #18420 (checksums) → Active discussion, PR submitted

## Contributing Features

If you want to implement a feature:
1. **Open issue first** - Discuss approach with maintainers
2. **Check feasibility** - Code size, compatibility, test requirements
3. **Submit PR** - With tests and documentation
4. **Iterate** - Based on maintainer feedback

Many enhancement requests welcome contributions! 🎉
