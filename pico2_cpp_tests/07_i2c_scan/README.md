# 07_i2c_scan - I2C Bus Scanner

## Description

This program scans the I2C bus for connected devices and reports their addresses. It's an essential diagnostic tool for identifying I2C devices and troubleshooting connections. The scanner checks all valid 7-bit I2C addresses and displays results in multiple formats.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350)
- I2C devices (sensors, displays, EEPROMs, etc.)
- Pull-up resistors: 2x 4.7kΩ resistors (may be built into device boards)

## Wiring

### Basic I2C Connection:
```
Pico GPIO 4 (SDA) ----[4.7kΩ to 3.3V]---- Device SDA
Pico GPIO 5 (SCL) ----[4.7kΩ to 3.3V]---- Device SCL
Pico 3.3V -------------------------------- Device VCC
Pico GND --------------------------------- Device GND
```

### Multiple I2C Devices (Bus):
```
              [4.7kΩ]      [4.7kΩ]
                |            |
3.3V ---------- +------------ +
                |            |
                |            |
GPIO 4 (SDA) ---+--- Dev1 ---+--- Dev2 ---+--- Dev3
                     SDA          SDA          SDA

GPIO 5 (SCL) ---+--- Dev1 ---+--- Dev2 ---+--- Dev3
                     SCL          SCL          SCL
                |            |            |
GND ----------- +------------ +------------ +
```

**Important:**
- Pull-up resistors are required (one pair for entire bus)
- Many breakout boards have built-in pull-ups
- Only one pair needed even with multiple devices

## Features

- Scans all valid I2C addresses (0x08 to 0x77)
- Identifies common I2C devices by address
- Visual address map display
- Formatted table with hex and decimal addresses
- Automatic rescanning every 5 seconds
- Troubleshooting hints when no devices found
- 100 kHz I2C frequency (standard mode)
- Internal pull-up support

## Expected Output

Serial output with devices found:
```
================================================
  Pico 2 I2C Bus Scanner
================================================
I2C initialized:
  I2C Port: i2c0
  SDA Pin: GPIO 4
  SCL Pin: GPIO 5
  Frequency: 100000 Hz (100 kHz)
================================================

Wiring Instructions:
  - Connect I2C SDA to GPIO 4
  - Connect I2C SCL to GPIO 5
  - Connect device VCC to 3.3V
  - Connect device GND to GND
  - Add 4.7k pull-up resistors on SDA and SCL
    (to 3.3V, if not already on device board)

Scanning will repeat every 5 seconds.
Press Ctrl+C to stop.


--- Scan #0 ---

================================================
          I2C Bus Scan
================================================
Scanning addresses 0x08 to 0x77...

Found 3 devices:

Address  | Hex    | Dec  | Possible Device
---------|--------|------|----------------------------------
0x3C     | 0x3C   | 60   | PCF8574A (I/O Expander)
0x68     | 0x68   | 104  | DS1307 RTC or MPU6050 IMU
0x76     | 0x76   | 118  | BMP280/BME280 (Pressure/Temp/Humidity)

Address Map (X = device found):
     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
00:                         .  .  .  .  .  .  .  .
10:  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
20:  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
30:  .  .  .  .  .  .  .  .  .  .  .  .  X  .  .  .
40:  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
50:  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
60:  .  .  .  .  .  .  .  .  X  .  .  .  .  .  .  .
70:  .  .  .  .  .  .  X  .
================================================
Scan complete. Found 3 devices.
Time: 2.456 seconds
================================================
```

Serial output with no devices:
```
================================================
          I2C Bus Scan
================================================
Scanning addresses 0x08 to 0x77...

No I2C devices found.

Troubleshooting:
  - Check device connections (SDA, SCL, VCC, GND)
  - Verify pull-up resistors (4.7k ohm) on SDA and SCL
  - Ensure device is powered
  - Check for correct voltage (3.3V vs 5V)
================================================
```

## Building

From the `pico2_cpp_tests` directory:
```bash
mkdir build
cd build
cmake ..
make 07_i2c_scan
```

## Uploading

1. Hold the BOOTSEL button while plugging in the Pico
2. Copy `build/07_i2c_scan/07_i2c_scan.uf2` to the RPI-RP2 drive
3. The Pico will reboot and start running the program

## Testing

### Test 1: No Devices (Baseline)
1. Upload program without any I2C devices connected
2. Connect to USB serial
3. Verify "No I2C devices found" message
4. This confirms scanner is working

### Test 2: Single Device
1. Connect one I2C device (e.g., OLED display, sensor)
2. Add pull-up resistors if not on device board
3. Verify device appears in scan results
4. Note the address for use in other programs

### Test 3: Multiple Devices
1. Connect multiple I2C devices to the bus
2. Verify all devices are detected
3. Check that addresses don't conflict

## Common I2C Devices and Addresses

| Address | Device |
|---------|--------|
| 0x1D | ADXL345 Accelerometer |
| 0x1E | HMC5883L Magnetometer |
| 0x20-0x27 | PCF8574 I/O Expander |
| 0x3C, 0x3D | SSD1306 OLED Display |
| 0x40-0x47 | PCA9685 PWM Driver |
| 0x48-0x4B | ADS1115 ADC |
| 0x50-0x57 | AT24C EEPROM |
| 0x68 | DS1307 RTC, MPU6050 IMU |
| 0x76, 0x77 | BMP280/BME280 Sensor |

## How It Works

**I2C Protocol:**
- Two-wire serial bus (SDA for data, SCL for clock)
- Master-slave architecture (Pico is master)
- Devices have unique 7-bit addresses (0x00-0x7F)
- Addresses 0x00-0x07 and 0x78-0x7F are reserved
- Multiple devices can share the bus

**Scanning Process:**
- For each address (0x08 to 0x77):
  - Send START condition
  - Send address with WRITE bit
  - If device exists, it responds with ACK
  - If no device, no ACK (NACK)
  - Send STOP condition
- Record addresses that responded with ACK

**Pull-up Resistors:**
- I2C lines are open-drain (require pull-ups)
- Typical value: 4.7kΩ to 3.3V
- Without pull-ups, bus won't work
- Too strong (low resistance): High current, signal distortion
- Too weak (high resistance): Slow rise time, unreliable

**RP2350 I2C:**
- Two I2C peripherals: I2C0 and I2C1
- I2C0 can use GPIO 0/1, 4/5, 8/9, 12/13, 16/17, 20/21
- This program uses I2C0 on GPIO 4/5
- Supports standard (100 kHz) and fast mode (400 kHz)

## Customization

You can modify these parameters in the code:
- `I2C_SDA_PIN` / `I2C_SCL_PIN`: Use different GPIO pins
- `I2C_PORT`: Switch to i2c1 for second I2C bus
- `I2C_FREQUENCY`: Change to 400000 for fast mode (400 kHz)
- `SCAN_INTERVAL_MS`: Adjust scanning frequency
- Add more device names in `get_device_name()` function

**Example: Use I2C1 on GPIO 6/7 at 400 kHz:**
```cpp
#define I2C_PORT i2c1
#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7
#define I2C_FREQUENCY 400000
```

## Troubleshooting

**No devices found:**
- Check all connections (especially GND)
- Verify device is powered (3.3V, not 5V)
- Ensure pull-up resistors are present
- Try different I2C device (test if scanner works)
- Check for short circuits

**Some devices missing:**
- Address conflict (two devices with same address)
- Device in sleep/low-power mode
- Wrong voltage level (some devices need 5V)
- Device requires initialization sequence

**Intermittent detection:**
- Weak or missing pull-ups
- Long wires (keep under 30cm for 100 kHz)
- Electrical noise
- Poor connections (check breadboard contacts)

**Wrong addresses shown:**
- Some devices have multiple possible addresses
- Check device datasheet for address selection
- Some devices have address select pins (A0, A1, A2)

## Pull-up Resistor Selection

**Standard formula:**
```
R_pullup = (V_supply - 0.4V) / I_max
```

For 3.3V I2C:
- Minimum: ~1kΩ (very high capacitance)
- Typical: 4.7kΩ (most applications)
- Maximum: ~10kΩ (low capacitance, short wires)

**Factors:**
- Bus capacitance (devices + wires)
- Bus speed (100 kHz vs 400 kHz)
- Number of devices
- Wire length

## Advanced Usage

- Use with logic analyzer to debug I2C communication
- Identify unknown I2C devices
- Verify device addressing before writing driver code
- Check for bus conflicts in complex projects
- Monitor bus for device hot-plugging
