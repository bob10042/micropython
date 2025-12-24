/**
 * @file main.cpp
 * @brief USB Serial "Hello World" Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program demonstrates USB serial communication by printing system
 * information and formatted messages repeatedly.
 *
 * Hardware:
 * - No external components required
 * - Uses USB connection for serial communication
 *
 * Expected Behavior:
 * - Prints system information on startup
 * - Continuously prints "Hello World" messages with counter and timestamp
 * - Demonstrates various printf() formatting options
 */

#include "pico/stdlib.h"
#include "pico/unique_id.h"
#include <stdio.h>

// Timing constants
const uint32_t PRINT_INTERVAL_MS = 2000;

/**
 * @brief Print system information
 */
void print_system_info() {
    printf("\n");
    printf("========================================\n");
    printf("  Raspberry Pi Pico 2 - System Info\n");
    printf("========================================\n");

    // Chip information
    printf("Chip: RP2350\n");
    printf("ROM Version: %d\n", rp2040_rom_version());

    // Clock frequency
    uint32_t freq = clock_get_hz(clk_sys);
    printf("System Clock: %lu Hz (%lu MHz)\n", freq, freq / 1000000);

    // Unique board ID
    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);
    printf("Board ID: ");
    for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; i++) {
        printf("%02X", board_id.id[i]);
    }
    printf("\n");

    printf("========================================\n");
    printf("\n");
}

/**
 * @brief Demonstrate various printf formatting options
 */
void demonstrate_formatting(uint32_t counter) {
    printf("\n--- Message #%lu ---\n", counter);
    printf("Hello from Raspberry Pi Pico 2!\n");

    // Time since boot
    uint64_t time_us = time_us_64();
    printf("Time since boot: %llu us (%.3f seconds)\n",
           time_us, (double)time_us / 1000000.0);

    // Integer formatting
    printf("Counter (decimal): %lu\n", counter);
    printf("Counter (hex): 0x%08lX\n", counter);
    printf("Counter (binary): 0b");
    for (int i = 31; i >= 0; i--) {
        printf("%d", (counter >> i) & 1);
        if (i % 8 == 0 && i > 0) printf("_");
    }
    printf("\n");

    // Float formatting
    float voltage = 3.3f;
    printf("Reference voltage: %.2f V\n", voltage);

    // Character and string
    char status = 'O';
    const char* message = "Running";
    printf("Status: %c (%s)\n", status, message);

    printf("\n");
}

int main() {
    // Initialize stdio for USB serial output
    stdio_init_all();

    // Wait for USB serial connection
    sleep_ms(2000);

    // Print system information once at startup
    print_system_info();

    printf("Starting continuous output...\n");
    printf("Press Ctrl+C to stop (if connected via terminal)\n");
    printf("\n");

    uint32_t counter = 0;

    // Main loop
    while (true) {
        demonstrate_formatting(counter);

        counter++;
        sleep_ms(PRINT_INTERVAL_MS);
    }

    return 0;
}
