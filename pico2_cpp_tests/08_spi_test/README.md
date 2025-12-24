# 08_spi_test - SPI Loopback Test

## Description

This program demonstrates SPI (Serial Peripheral Interface) communication on the Raspberry Pi Pico 2 by performing loopback tests. It sends various data patterns via SPI and verifies that the received data matches, providing a comprehensive test of the SPI hardware and configuration.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- 1x Jumper wire (for loopback connection)
- Optional: Logic analyzer for signal debugging

## Wiring

### Loopback Configuration:
```
GPIO 19 (MOSI) ----[JUMPER WIRE]---- GPIO 16 (MISO)

Other SPI pins (generate signals but no external connection needed):
GPIO 18 (SCK)  - Clock output
GPIO 17 (CS)   - Chip Select output
```

**Critical:** The jumper wire connecting MOSI to MISO is essential for this test. Without it, all tests will fail.

## Features

- SPI hardware initialization at 1 MHz
- Multiple test patterns:
  - Single byte transfers
  - Alternating bit patterns
  - Incrementing sequences
  - All zeros / all ones
  - Random patterns
  - Large transfers (64 bytes)
  - ASCII strings
- Data verification (TX vs RX comparison)
- Comprehensive error reporting
- Test statistics and pass/fail tracking
- Hex dump display of transmitted and received data
- Automatic test cycling every 3 seconds

## Expected Output

Serial output (with loopback connected):
```
================================================
  Pico 2 SPI Loopback Test
================================================
SPI initialized:
  SPI Port: spi0
  MISO Pin (RX): GPIO 16
  MOSI Pin (TX): GPIO 19
  SCK Pin:       GPIO 18
  CS Pin:        GPIO 17
  Frequency:     1000000 Hz (1 MHz)
  Mode:          Mode 0 (CPOL=0, CPHA=0)
  Bit Order:     MSB First
================================================

Wiring Instructions:
  IMPORTANT: Connect MOSI to MISO for loopback test:
  - GPIO 19 (MOSI) --[JUMPER]-- GPIO 16 (MISO)

  Other pins (no connection needed for loopback):
  - GPIO 18 (SCK)  - Clock signal
  - GPIO 17 (CS)   - Chip Select

Tests will run every 3 seconds.
Press Ctrl+C to stop.


##################################################
#  Test Cycle #0
##################################################

================================================
          Running SPI Loopback Tests
================================================

--- Test 1: Single Byte (0xA5) ---
Length: 1 bytes
TX: A5
RX: A5
PASS: All bytes matched!

--- Test 2: Alternating Pattern ---
Length: 8 bytes
TX: 00 FF 00 FF 00 FF 00 FF
RX: 00 FF 00 FF 00 FF 00 FF
PASS: All bytes matched!

--- Test 3: Incrementing Sequence (0x00-0x0F) ---
Length: 16 bytes
TX: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
RX: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
PASS: All bytes matched!

--- Test 4: All Zeros ---
Length: 8 bytes
TX: 00 00 00 00 00 00 00 00
RX: 00 00 00 00 00 00 00 00
PASS: All bytes matched!

--- Test 5: All Ones ---
Length: 8 bytes
TX: FF FF FF FF FF FF FF FF
RX: FF FF FF FF FF FF FF FF
PASS: All bytes matched!

--- Test 6: Random Pattern ---
Length: 8 bytes
TX: 12 34 56 78 9A BC DE F0
RX: 12 34 56 78 9A BC DE F0
PASS: All bytes matched!

--- Test 7: Large Transfer (64 bytes) ---
Length: 64 bytes
TX: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
    10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
    20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F
    30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F
RX: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
    10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
    20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F
    30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F
PASS: All bytes matched!

--- Test 8: ASCII String ---
Length: 13 bytes
TX: 48 65 6C 6C 6F 20 50 69 63 6F 20 32 21
RX: 48 65 6C 6C 6F 20 50 69 63 6F 20 32 21
PASS: All bytes matched!

================================================
          Test Summary
================================================
Tests Passed: 8
Tests Failed: 0
Total Tests:  8
Status: ALL TESTS PASSED!
================================================
Time: 2.456 seconds
================================================
```

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 08_spi_test
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/08_spi_test/08_spi_test.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

### Test Procedure:
1. Upload the program (without loopback wire initially)
2. Connect to USB serial at 115200 baud
3. Observe that tests fail (expected without loopback)
4. Connect jumper wire from GPIO 19 to GPIO 16
5. Wait for next test cycle (or reset Pico)
6. Verify all tests now pass
7. Remove jumper wire and verify tests fail again

This confirms the SPI hardware and loopback test are working correctly.

## How It Works

**SPI Basics:**
- Four-wire serial protocol (MOSI, MISO, SCK, CS)
- Synchronous (clock signal coordinates data transfer)
- Full-duplex (simultaneous transmit and receive)
- Master-slave architecture (Pico is master in this test)
- Higher speed than I2C (MHz range typical)

**SPI Signals:**
- **MOSI** (Master Out, Slave In): Data from master to slave
- **MISO** (Master In, Slave Out): Data from slave to master
- **SCK** (Serial Clock): Clock generated by master
- **CS** (Chip Select): Select which slave device (active low)

**SPI Modes (CPOL/CPHA):**
- Mode 0 (CPOL=0, CPHA=0): Clock idle low, sample on leading edge
- Mode 1 (CPOL=0, CPHA=1): Clock idle low, sample on trailing edge
- Mode 2 (CPOL=1, CPHA=0): Clock idle high, sample on leading edge
- Mode 3 (CPOL=1, CPHA=1): Clock idle high, sample on trailing edge
- This program uses Mode 0 (most common)

**Loopback Test:**
- MOSI connected directly to MISO
- Data sent on MOSI is immediately received on MISO
- Verifies SPI hardware is functioning correctly
- No actual slave device needed
- Simple and effective hardware test

**RP2350 SPI:**
- Two SPI peripherals: SPI0 and SPI1
- SPI0 can use GPIO 0-3, 4-7, 16-19
- This program uses SPI0 on GPIO 16-19
- Maximum frequency: 62.5 MHz (system clock / 2)
- Configurable data size (4-16 bits, using 8 bits here)

## Test Patterns Explained

1. **Single Byte (0xA5):** Basic functionality test
2. **Alternating:** Tests all bits toggle correctly
3. **Incrementing:** Tests sequential data integrity
4. **All Zeros/Ones:** Tests constant signal levels
5. **Random Pattern:** Tests arbitrary data
6. **Large Transfer:** Tests FIFO and DMA capability
7. **ASCII String:** Tests real-world data

## Customization

You can modify these parameters in the code:
- `SPI_FREQUENCY`: Change speed (max ~62.5 MHz, typical 1-10 MHz)
- `SPI_*_PIN`: Use different GPIO pins
- `SPI_PORT`: Switch to spi1 for second SPI bus
- SPI mode: Change `SPI_CPOL_*` and `SPI_CPHA_*`
- Test patterns: Add custom test data
- `TEST_INTERVAL_MS`: Adjust test cycle frequency

**Example: Use SPI1 at 10 MHz in Mode 3:**
```cpp
#define SPI_PORT spi1
#define SPI_FREQUENCY 10000000
spi_set_format(SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
```

## Connecting Real SPI Devices

After verifying loopback test passes, you can connect real SPI devices:

```
Pico GPIO 19 (MOSI) ---- Device MOSI (or DI/SDI)
Pico GPIO 16 (MISO) ---- Device MISO (or DO/SDO)
Pico GPIO 18 (SCK)  ---- Device SCK (or SCLK)
Pico GPIO 17 (CS)   ---- Device CS (or SS/~CS)
Pico 3.3V -------------- Device VCC
Pico GND --------------- Device GND
```

Common SPI devices:
- SD Cards (SPI mode)
- OLED/TFT Displays (SSD1306, ILI9341, ST7735)
- NRF24L01 Radio Module
- RFID Readers (RC522)
- Flash Memory (W25Q series)
- ADCs/DACs (MCP3008, MCP4921)

## Troubleshooting

**All tests fail:**
- Check MOSI→MISO jumper wire is connected
- Verify using correct GPIO pins (19→16)
- Check for damaged pins or poor breadboard contact
- Try different jumper wire

**Intermittent failures:**
- Loose connection
- EMI/electrical noise
- Try lower SPI frequency
- Use shorter jumper wire

**Some bytes mismatch:**
- Electrical noise
- Signal integrity issues
- Try lower frequency
- Add ground wire parallel to jumper

**No output at all:**
- Verify program uploaded correctly
- Check USB serial connection
- Try resetting Pico

## Advanced Topics

**DMA Transfers:**
- For high-speed bulk transfers
- Offloads CPU during SPI transfer
- Essential for high data rates

**Multiple Slaves:**
- Use separate CS pin for each device
- Only one CS active at a time
- Master controls which slave responds

**SPI Signal Characteristics:**
- Rise/fall times important at high speeds
- Capacitance limits maximum speed
- Use short wires for >10 MHz
- Consider transmission line effects >25 MHz

**Debugging with Logic Analyzer:**
- Connect to MOSI, MISO, SCK, CS
- Decode SPI protocol
- Verify timing and data
- Essential for troubleshooting real devices
