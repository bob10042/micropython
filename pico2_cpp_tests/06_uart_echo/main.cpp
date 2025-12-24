/**
 * @file main.cpp
 * @brief UART Echo Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program demonstrates UART (serial) communication by echoing back
 * any characters received on UART0. It also provides statistics and
 * demonstrates UART configuration.
 *
 * Hardware:
 * - UART0 TX on GPIO 0
 * - UART0 RX on GPIO 1
 * - Connect to USB-to-Serial adapter or another device
 *
 * Expected Behavior:
 * - Echoes received characters back to sender
 * - Displays received data on USB serial (for debugging)
 * - Shows byte count and character statistics
 * - Handles special characters (newline, carriage return, etc.)
 */

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>
#include <ctype.h>

// UART configuration
#define UART_ID uart0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define UART_BAUD_RATE 115200

// Data parity and stop bits
#define UART_DATA_BITS 8
#define UART_STOP_BITS 1
#define UART_PARITY UART_PARITY_NONE

// Statistics
uint32_t bytes_received = 0;
uint32_t bytes_sent = 0;
uint32_t lines_received = 0;

/**
 * @brief Initialize UART with specified parameters
 */
void init_uart() {
    // Initialize UART
    uart_init(UART_ID, UART_BAUD_RATE);

    // Set GPIO pins for UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Set UART parameters
    uart_set_format(UART_ID, UART_DATA_BITS, UART_STOP_BITS, UART_PARITY);

    // Enable UART FIFO
    uart_set_fifo_enabled(UART_ID, true);

    printf("UART initialized:\n");
    printf("  UART ID: uart%d\n", uart_get_index(UART_ID));
    printf("  TX Pin: GPIO %d\n", UART_TX_PIN);
    printf("  RX Pin: GPIO %d\n", UART_RX_PIN);
    printf("  Baud Rate: %d\n", UART_BAUD_RATE);
    printf("  Data Bits: %d\n", UART_DATA_BITS);
    printf("  Stop Bits: %d\n", UART_STOP_BITS);
    printf("  Parity: None\n");
}

/**
 * @brief Send string via UART
 */
void uart_send_string(const char* str) {
    while (*str) {
        uart_putc_raw(UART_ID, *str);
        bytes_sent++;
        str++;
    }
}

/**
 * @brief Display statistics
 */
void display_stats() {
    printf("\n--- Statistics ---\n");
    printf("Bytes received: %lu\n", bytes_received);
    printf("Bytes sent:     %lu\n", bytes_sent);
    printf("Lines received: %lu\n", lines_received);
    printf("Time: %.3f seconds\n", (float)time_us_64() / 1000000.0f);
}

/**
 * @brief Process and echo received character
 */
void process_char(char c) {
    bytes_received++;

    // Echo character back via UART
    uart_putc_raw(UART_ID, c);
    bytes_sent++;

    // Display on USB serial for debugging
    if (c == '\n') {
        printf(" [LF]");
        lines_received++;
    } else if (c == '\r') {
        printf(" [CR]");
    } else if (c == '\t') {
        printf(" [TAB]");
    } else if (isprint(c)) {
        printf("%c", c);
    } else {
        printf("[0x%02X]", (unsigned char)c);
    }

    // Show statistics every 100 bytes
    if (bytes_received % 100 == 0) {
        display_stats();
        printf("\nContinuing echo...\n");
    }

    // Flush USB output periodically
    if (bytes_received % 10 == 0) {
        stdio_flush();
    }
}

/**
 * @brief Send test message via UART
 */
void send_test_message() {
    const char* test_msg = "\r\n=== Pico 2 UART Echo Test ===\r\n"
                           "Send characters and they will be echoed back.\r\n"
                           "Ready to receive...\r\n\r\n";

    uart_send_string(test_msg);

    printf("\nTest message sent via UART\n");
}

int main() {
    // Initialize USB stdio for debugging output
    stdio_init_all();

    // Wait for USB serial connection
    sleep_ms(2000);

    printf("\n");
    printf("========================================\n");
    printf("  Pico 2 UART Echo Test\n");
    printf("========================================\n");

    // Initialize UART
    init_uart();

    printf("========================================\n");
    printf("\n");
    printf("Wiring Instructions:\n");
    printf("  - Connect UART TX (GPIO %d) to RX of other device\n", UART_TX_PIN);
    printf("  - Connect UART RX (GPIO %d) to TX of other device\n", UART_RX_PIN);
    printf("  - Connect GND to GND of other device\n");
    printf("  - Set other device to %d baud, 8N1\n", UART_BAUD_RATE);
    printf("\n");
    printf("For loopback test:\n");
    printf("  - Connect GPIO %d (TX) to GPIO %d (RX) with a jumper\n", UART_TX_PIN, UART_RX_PIN);
    printf("\n");

    // Send initial test message
    send_test_message();

    printf("Starting echo loop...\n");
    printf("Received characters will be displayed below:\n");
    printf("-----------------------------------------\n");

    uint32_t last_stats_time = 0;

    // Main loop
    while (true) {
        // Check if data is available
        if (uart_is_readable(UART_ID)) {
            // Read character
            char c = uart_getc(UART_ID);

            // Process and echo
            process_char(c);
        }

        // Display periodic statistics (every 10 seconds)
        uint32_t current_time = time_us_64() / 1000000;
        if (current_time - last_stats_time >= 10 && bytes_received > 0) {
            display_stats();
            last_stats_time = current_time;
        }

        // Small delay to reduce CPU usage
        sleep_us(100);
    }

    return 0;
}
