/**
 * @file main.cpp
 * @brief SPI Loopback Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program demonstrates SPI (Serial Peripheral Interface) communication
 * by performing loopback tests where MOSI is connected to MISO.
 *
 * Hardware:
 * - SPI0 MISO on GPIO 16 (RX)
 * - SPI0 MOSI on GPIO 19 (TX)
 * - SPI0 SCK on GPIO 18 (Clock)
 * - SPI0 CS on GPIO 17 (Chip Select)
 * - Jumper wire connecting MOSI to MISO
 *
 * Expected Behavior:
 * - Sends test patterns via SPI
 * - Receives data through loopback connection
 * - Verifies sent data matches received data
 * - Reports success/failure of each test
 * - Tests various data patterns and sizes
 */

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>
#include <string.h>

// SPI configuration
#define SPI_PORT spi0
#define SPI_MISO_PIN 16  // RX
#define SPI_CS_PIN   17  // Chip Select
#define SPI_SCK_PIN  18  // Clock
#define SPI_MOSI_PIN 19  // TX

#define SPI_FREQUENCY 1000000  // 1 MHz

// Test parameters
const uint32_t TEST_INTERVAL_MS = 3000;

// Test statistics
uint32_t tests_passed = 0;
uint32_t tests_failed = 0;

/**
 * @brief Initialize SPI bus
 */
void init_spi() {
    // Initialize SPI at specified frequency
    spi_init(SPI_PORT, SPI_FREQUENCY);

    // Set SPI format: 8 data bits, SPI mode 0 (CPOL=0, CPHA=0)
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // Set GPIO functions for SPI
    gpio_set_function(SPI_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CS_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);

    printf("SPI initialized:\n");
    printf("  SPI Port: spi%d\n", spi_get_index(SPI_PORT));
    printf("  MISO Pin (RX): GPIO %d\n", SPI_MISO_PIN);
    printf("  MOSI Pin (TX): GPIO %d\n", SPI_MOSI_PIN);
    printf("  SCK Pin:       GPIO %d\n", SPI_SCK_PIN);
    printf("  CS Pin:        GPIO %d\n", SPI_CS_PIN);
    printf("  Frequency:     %d Hz (%d MHz)\n", SPI_FREQUENCY, SPI_FREQUENCY / 1000000);
    printf("  Mode:          Mode 0 (CPOL=0, CPHA=0)\n");
    printf("  Bit Order:     MSB First\n");
}

/**
 * @brief Display byte array in hex format
 */
void print_hex_bytes(const char* label, uint8_t* data, size_t len) {
    printf("%s", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0 && i < len - 1) {
            printf("\n%*s", (int)strlen(label), "");
        }
    }
    printf("\n");
}

/**
 * @brief Test SPI loopback with given data
 */
bool test_spi_loopback(uint8_t* tx_data, size_t len, const char* test_name) {
    uint8_t rx_data[256];
    memset(rx_data, 0, sizeof(rx_data));

    printf("\n--- %s ---\n", test_name);
    printf("Length: %d bytes\n", len);

    // Print TX data
    print_hex_bytes("TX: ", tx_data, len);

    // Perform SPI transfer (simultaneous read/write)
    int bytes_transferred = spi_write_read_blocking(SPI_PORT, tx_data, rx_data, len);

    // Print RX data
    print_hex_bytes("RX: ", rx_data, len);

    // Verify transfer
    if (bytes_transferred != (int)len) {
        printf("ERROR: Expected %d bytes, transferred %d bytes\n", len, bytes_transferred);
        tests_failed++;
        return false;
    }

    // Compare TX and RX data
    bool match = true;
    for (size_t i = 0; i < len; i++) {
        if (tx_data[i] != rx_data[i]) {
            printf("MISMATCH at byte %d: sent 0x%02X, received 0x%02X\n", i, tx_data[i], rx_data[i]);
            match = false;
        }
    }

    if (match) {
        printf("PASS: All bytes matched!\n");
        tests_passed++;
        return true;
    } else {
        printf("FAIL: Data mismatch detected\n");
        tests_failed++;
        return false;
    }
}

/**
 * @brief Run comprehensive SPI tests
 */
void run_spi_tests() {
    printf("\n");
    printf("================================================\n");
    printf("          Running SPI Loopback Tests\n");
    printf("================================================\n");

    uint8_t test_data[256];

    // Test 1: Single byte
    test_data[0] = 0xA5;
    test_spi_loopback(test_data, 1, "Test 1: Single Byte (0xA5)");

    // Test 2: Alternating pattern
    for (int i = 0; i < 8; i++) {
        test_data[i] = (i % 2) ? 0xFF : 0x00;
    }
    test_spi_loopback(test_data, 8, "Test 2: Alternating Pattern");

    // Test 3: Incrementing sequence
    for (int i = 0; i < 16; i++) {
        test_data[i] = i;
    }
    test_spi_loopback(test_data, 16, "Test 3: Incrementing Sequence (0x00-0x0F)");

    // Test 4: All zeros
    memset(test_data, 0x00, 8);
    test_spi_loopback(test_data, 8, "Test 4: All Zeros");

    // Test 5: All ones
    memset(test_data, 0xFF, 8);
    test_spi_loopback(test_data, 8, "Test 5: All Ones");

    // Test 6: Random-like pattern
    const uint8_t random_pattern[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    memcpy(test_data, random_pattern, 8);
    test_spi_loopback(test_data, 8, "Test 6: Random Pattern");

    // Test 7: Large transfer (64 bytes)
    for (int i = 0; i < 64; i++) {
        test_data[i] = i & 0xFF;
    }
    test_spi_loopback(test_data, 64, "Test 7: Large Transfer (64 bytes)");

    // Test 8: ASCII string
    const char* ascii_msg = "Hello Pico 2!";
    memcpy(test_data, ascii_msg, strlen(ascii_msg));
    test_spi_loopback(test_data, strlen(ascii_msg), "Test 8: ASCII String");

    // Display summary
    printf("\n");
    printf("================================================\n");
    printf("          Test Summary\n");
    printf("================================================\n");
    printf("Tests Passed: %lu\n", tests_passed);
    printf("Tests Failed: %lu\n", tests_failed);
    printf("Total Tests:  %lu\n", tests_passed + tests_failed);

    if (tests_failed == 0) {
        printf("Status: ALL TESTS PASSED!\n");
    } else {
        printf("Status: SOME TESTS FAILED\n");
        printf("\nTroubleshooting:\n");
        printf("  - Verify MOSI (GPIO %d) is connected to MISO (GPIO %d)\n", SPI_MOSI_PIN, SPI_MISO_PIN);
        printf("  - Check jumper wire connection\n");
        printf("  - Ensure good electrical contact\n");
    }

    printf("================================================\n");
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
    printf("  Pico 2 SPI Loopback Test\n");
    printf("================================================\n");

    // Initialize SPI
    init_spi();

    printf("================================================\n");
    printf("\n");
    printf("Wiring Instructions:\n");
    printf("  IMPORTANT: Connect MOSI to MISO for loopback test:\n");
    printf("  - GPIO %d (MOSI) --[JUMPER]-- GPIO %d (MISO)\n", SPI_MOSI_PIN, SPI_MISO_PIN);
    printf("\n");
    printf("  Other pins (no connection needed for loopback):\n");
    printf("  - GPIO %d (SCK)  - Clock signal\n", SPI_SCK_PIN);
    printf("  - GPIO %d (CS)   - Chip Select\n", SPI_CS_PIN);
    printf("\n");
    printf("Tests will run every %lu seconds.\n", TEST_INTERVAL_MS / 1000);
    printf("Press Ctrl+C to stop.\n");

    uint32_t test_cycle = 0;

    // Main loop
    while (true) {
        printf("\n\n");
        printf("##################################################\n");
        printf("#  Test Cycle #%lu\n", test_cycle);
        printf("##################################################\n");

        // Reset statistics for this cycle
        uint32_t passed_before = tests_passed;
        uint32_t failed_before = tests_failed;

        // Run all tests
        run_spi_tests();

        // Calculate results for this cycle
        uint32_t cycle_passed = tests_passed - passed_before;
        uint32_t cycle_failed = tests_failed - failed_before;

        printf("\nCycle Results: %lu passed, %lu failed\n", cycle_passed, cycle_failed);
        printf("Overall Total: %lu passed, %lu failed\n", tests_passed, tests_failed);

        test_cycle++;
        sleep_ms(TEST_INTERVAL_MS);
    }

    return 0;
}
