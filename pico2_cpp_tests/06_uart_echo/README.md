# 06_uart_echo - UART Echo Test

## Description

This program demonstrates UART (Universal Asynchronous Receiver/Transmitter) serial communication on the Raspberry Pi Pico 2. It echoes back any characters received on UART0, providing a simple way to test serial connectivity.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- USB-to-Serial adapter (FTDI, CP2102, CH340, etc.) **OR**
- Another microcontroller/device with UART **OR**
- Jumper wire for loopback test

## Wiring

### Option 1: With USB-to-Serial Adapter
```
Pico GPIO 0 (TX) ---- RX on USB-Serial adapter
Pico GPIO 1 (RX) ---- TX on USB-Serial adapter
Pico GND ------------ GND on USB-Serial adapter
```

### Option 2: Loopback Test (No External Hardware)
```
Pico GPIO 0 (TX) ---- Pico GPIO 1 (RX)
                      (connect with jumper wire)
```

### Option 3: Connect to Another Device
```
Pico GPIO 0 (TX) ---- RX on other device
Pico GPIO 1 (RX) ---- TX on other device
Pico GND ------------ GND on other device
```

**Important:** Always connect TX to RX and RX to TX (cross-over connection).

## Features

- UART communication at 115200 baud (configurable)
- Echoes received characters back to sender
- Character-by-character echo with minimal latency
- Special character handling (newline, carriage return, tab)
- Statistics tracking (bytes received/sent, lines)
- Dual output: UART echo + USB serial debug
- 8 data bits, no parity, 1 stop bit (8N1)
- Hardware FIFO enabled for reliability

## Expected Output

### USB Serial (Debug Output):
```
========================================
  Pico 2 UART Echo Test
========================================
UART initialized:
  UART ID: uart0
  TX Pin: GPIO 0
  RX Pin: GPIO 1
  Baud Rate: 115200
  Data Bits: 8
  Stop Bits: 1
  Parity: None
========================================

Wiring Instructions:
  - Connect UART TX (GPIO 0) to RX of other device
  - Connect UART RX (GPIO 1) to TX of other device
  - Connect GND to GND of other device
  - Set other device to 115200 baud, 8N1

For loopback test:
  - Connect GPIO 0 (TX) to GPIO 1 (RX) with a jumper

Test message sent via UART

Starting echo loop...
Received characters will be displayed below:
-----------------------------------------
Hello [CR] [LF]World [CR] [LF]
--- Statistics ---
Bytes received: 14
Bytes sent:     14
Lines received: 2
Time: 5.234 seconds
```

### UART Output (What the other device receives):
```
=== Pico 2 UART Echo Test ===
Send characters and they will be echoed back.
Ready to receive...

Hello
World
```

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 06_uart_echo
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/06_uart_echo/06_uart_echo.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

### Test 1: Loopback Test (Simplest)
1. Upload the program
2. Connect GPIO 0 to GPIO 1 with a jumper wire
3. Connect to USB serial (for debug output)
4. You should see the test message echoed back in the statistics

### Test 2: With Serial Terminal
1. Connect USB-to-Serial adapter as shown
2. Upload the program
3. Open two serial terminals:
   - Terminal 1: Pico's USB serial (debug output)
   - Terminal 2: USB-to-Serial adapter at 115200 baud, 8N1
4. Type in Terminal 2
5. Verify characters are echoed back
6. Check Terminal 1 for debug output and statistics

### Test 3: With Another Microcontroller
1. Configure other device for 115200 baud, 8N1
2. Connect TX/RX pins as shown
3. Send test data from other device
4. Verify data is echoed back

## How It Works

**UART Basics:**
- Asynchronous serial communication (no clock signal)
- Data transmitted as start bit + data bits + stop bit(s)
- Both devices must use same baud rate
- Standard format: 8N1 (8 data bits, no parity, 1 stop bit)

**RP2350 UART:**
- Two UART peripherals: UART0 and UART1
- UART0 can use GPIO 0/1, 12/13, 16/17, 28/29
- UART1 can use GPIO 4/5, 8/9, 20/21, 24/25
- This program uses UART0 on GPIO 0/1
- Hardware FIFO buffers (32 bytes) for reliability

**Echo Implementation:**
- Main loop continuously checks `uart_is_readable()`
- When data available, reads character with `uart_getc()`
- Immediately echoes back with `uart_putc_raw()`
- Also displays on USB serial for debugging

**Dual Serial Output:**
- UART0 (GPIO 0/1): Echo functionality
- USB CDC: Debug output and statistics
- These are independent serial ports

## Customization

You can modify these parameters in the code:
- `UART_BAUD_RATE`: Change baud rate (9600, 38400, 115200, 921600, etc.)
- `UART_TX_PIN` / `UART_RX_PIN`: Use different GPIO pins
- `UART_ID`: Switch to uart1 for second UART
- `UART_DATA_BITS`: Change to 7 or 9 bits
- `UART_STOP_BITS`: Change to 2 stop bits
- `UART_PARITY`: Add even/odd parity

**Example: Use UART1 on GPIO 4/5:**
```cpp
#define UART_ID uart1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
```

## Common Baud Rates

- 9600: Slow but reliable (legacy devices)
- 19200: 2x faster than 9600
- 38400: 4x faster than 9600
- 57600: Common for GPS modules
- 115200: Most common modern rate (this program's default)
- 230400: 2x faster than 115200
- 460800: 4x faster than 115200
- 921600: Very fast (max for most USB-Serial adapters)

**Note:** Both devices must use the same baud rate!

## Troubleshooting

**No characters received:**
- Check wiring (TX→RX, RX→TX)
- Verify GND connection
- Confirm baud rate matches on both devices
- Check that other device is transmitting

**Garbled characters:**
- Baud rate mismatch (most common cause)
- Wrong data format (e.g., 7N1 vs 8N1)
- Electrical noise (use shorter wires, add ground)
- Voltage level mismatch (ensure 3.3V logic levels)

**Characters dropped:**
- Baud rate too high for system
- FIFO overflow (increase processing speed)
- Reduce baud rate or add flow control

**Loopback doesn't work:**
- Verify jumper wire connection
- Try different wire/connection
- Check GPIO pins aren't damaged

## Advanced Features to Explore

- Flow control (RTS/CTS hardware handshaking)
- Interrupt-driven UART (instead of polling)
- DMA for high-speed transfers
- Binary protocol implementation
- Packet framing and checksums
- Multiple UART instances (UART0 + UART1 simultaneously)
