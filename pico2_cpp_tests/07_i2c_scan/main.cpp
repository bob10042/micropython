/**
 * @file main.cpp
 * @brief I2C Bus Scanner for Raspberry Pi Pico 2 (RP2350)
 *
 * This program scans the I2C bus for connected devices and reports their
 * addresses. Useful for identifying I2C devices and troubleshooting connections.
 *
 * Hardware:
 * - I2C0 SDA on GPIO 4
 * - I2C0 SCL on GPIO 5
 * - Optional: Pull-up resistors (4.7kΩ) on SDA and SCL lines
 * - I2C devices to scan (sensors, displays, etc.)
 *
 * Expected Behavior:
 * - Scans all valid I2C addresses (0x08 to 0x77)
 * - Reports found devices with addresses in hex and decimal
 * - Displays results in a formatted table
 * - Repeats scan every 5 seconds
 */

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

// I2C configuration
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define I2C_FREQUENCY 100000  // 100 kHz (standard mode)

// I2C address range
// Valid 7-bit addresses: 0x08 to 0x77
// 0x00-0x07 and 0x78-0x7F are reserved
const uint8_t I2C_MIN_ADDR = 0x08;
const uint8_t I2C_MAX_ADDR = 0x77;

// Scan interval
const uint32_t SCAN_INTERVAL_MS = 5000;

/**
 * @brief Initialize I2C bus
 */
void init_i2c() {
    // Initialize I2C at specified frequency
    i2c_init(I2C_PORT, I2C_FREQUENCY);

    // Set GPIO pins for I2C
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // Enable internal pull-ups (optional if external pull-ups exist)
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    printf("I2C initialized:\n");
    printf("  I2C Port: i2c%d\n", i2c_hw_index(I2C_PORT));
    printf("  SDA Pin: GPIO %d\n", I2C_SDA_PIN);
    printf("  SCL Pin: GPIO %d\n", I2C_SCL_PIN);
    printf("  Frequency: %d Hz (%d kHz)\n", I2C_FREQUENCY, I2C_FREQUENCY / 1000);
}

/**
 * @brief Check if device responds at given I2C address
 * @param addr 7-bit I2C address to check
 * @return true if device responds, false otherwise
 */
bool i2c_check_address(uint8_t addr) {
    // Try to write 0 bytes to the address
    // If device exists, it will ACK
    int ret = i2c_write_blocking(I2C_PORT, addr, NULL, 0, false);
    return (ret >= 0);
}

/**
 * @brief Get common device name for known I2C addresses
 * @param addr 7-bit I2C address
 * @return Device name or empty string if unknown
 */
const char* get_device_name(uint8_t addr) {
    switch (addr) {
        case 0x0E: return "MAG3110 (Magnetometer)";
        case 0x1C: case 0x1D: return "ADXL345 (Accelerometer)";
        case 0x1E: return "HMC5883L (Magnetometer)";
        case 0x20: case 0x21: case 0x22: case 0x23:
        case 0x24: case 0x25: case 0x26: case 0x27:
            return "PCF8574 (I/O Expander) or MCP23008";
        case 0x38: case 0x39: case 0x3A: case 0x3B:
        case 0x3C: case 0x3D: case 0x3E: case 0x3F:
            return "PCF8574A (I/O Expander)";
        case 0x40: case 0x41: case 0x42: case 0x43:
        case 0x44: case 0x45: case 0x46: case 0x47:
            return "PCA9685 (PWM Driver) or INA219";
        case 0x48: case 0x49: case 0x4A: case 0x4B:
            return "ADS1115 (ADC) or TMP102";
        case 0x50: case 0x51: case 0x52: case 0x53:
        case 0x54: case 0x55: case 0x56: case 0x57:
            return "AT24C EEPROM";
        case 0x68: return "DS1307 RTC or MPU6050 IMU";
        case 0x69: return "MPU6050 IMU (alt address)";
        case 0x76: case 0x77: return "BMP280/BME280 (Pressure/Temp/Humidity)";
        default: return "";
    }
}

/**
 * @brief Scan I2C bus and display results
 */
void scan_i2c_bus() {
    printf("\n");
    printf("================================================\n");
    printf("          I2C Bus Scan\n");
    printf("================================================\n");
    printf("Scanning addresses 0x%02X to 0x%02X...\n\n", I2C_MIN_ADDR, I2C_MAX_ADDR);

    uint8_t found_count = 0;
    uint8_t found_addresses[128];

    // Scan all valid addresses
    for (uint8_t addr = I2C_MIN_ADDR; addr <= I2C_MAX_ADDR; addr++) {
        if (i2c_check_address(addr)) {
            found_addresses[found_count] = addr;
            found_count++;
        }
    }

    // Display results
    if (found_count == 0) {
        printf("No I2C devices found.\n");
        printf("\nTroubleshooting:\n");
        printf("  - Check device connections (SDA, SCL, VCC, GND)\n");
        printf("  - Verify pull-up resistors (4.7k ohm) on SDA and SCL\n");
        printf("  - Ensure device is powered\n");
        printf("  - Check for correct voltage (3.3V vs 5V)\n");
    } else {
        printf("Found %d device%s:\n\n", found_count, found_count == 1 ? "" : "s");
        printf("Address  | Hex    | Dec  | Possible Device\n");
        printf("---------|--------|------|----------------------------------\n");

        for (uint8_t i = 0; i < found_count; i++) {
            uint8_t addr = found_addresses[i];
            const char* name = get_device_name(addr);

            printf("0x%02X     | 0x%02X   | %-4d | %s\n",
                   addr, addr, addr, name);
        }
    }

    // Display address map (visual representation)
    printf("\n");
    printf("Address Map (X = device found):\n");
    printf("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (uint8_t row = 0; row < 8; row++) {
        printf("%02X: ", row * 16);

        for (uint8_t col = 0; col < 16; col++) {
            uint8_t addr = row * 16 + col;

            if (addr < I2C_MIN_ADDR || addr > I2C_MAX_ADDR) {
                printf("   ");  // Reserved addresses
            } else {
                bool found = false;
                for (uint8_t i = 0; i < found_count; i++) {
                    if (found_addresses[i] == addr) {
                        found = true;
                        break;
                    }
                }
                printf(" %c ", found ? 'X' : '.');
            }
        }
        printf("\n");
    }

    printf("================================================\n");
    printf("Scan complete. Found %d device%s.\n", found_count, found_count == 1 ? "" : "s");
    printf("Time: %.3f seconds\n", (float)time_us_64() / 1000000.0f);
    printf("================================================\n");
}

int main() {
    // Initialize USB stdio
    stdio_init_all();

    // Wait for USB serial connection
    sleep_ms(2000);

    printf("\n");
    printf("================================================\n");
    printf("  Pico 2 I2C Bus Scanner\n");
    printf("================================================\n");

    // Initialize I2C
    init_i2c();

    printf("================================================\n");
    printf("\n");
    printf("Wiring Instructions:\n");
    printf("  - Connect I2C SDA to GPIO %d\n", I2C_SDA_PIN);
    printf("  - Connect I2C SCL to GPIO %d\n", I2C_SCL_PIN);
    printf("  - Connect device VCC to 3.3V\n");
    printf("  - Connect device GND to GND\n");
    printf("  - Add 4.7k pull-up resistors on SDA and SCL\n");
    printf("    (to 3.3V, if not already on device board)\n");
    printf("\n");
    printf("Scanning will repeat every %lu seconds.\n", SCAN_INTERVAL_MS / 1000);
    printf("Press Ctrl+C to stop.\n");

    uint32_t scan_count = 0;

    // Main loop - continuous scanning
    while (true) {
        printf("\n\n--- Scan #%lu ---\n", scan_count);
        scan_i2c_bus();

        scan_count++;
        sleep_ms(SCAN_INTERVAL_MS);
    }

    return 0;
}
