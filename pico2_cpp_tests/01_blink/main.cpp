/**
 * @file main.cpp
 * @brief Basic LED Blink Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program blinks the onboard LED at 1 Hz and prints status messages
 * via USB serial output.
 *
 * Hardware:
 * - LED on GPIO 25 (onboard LED on Pico 2)
 *
 * Expected Behavior:
 * - LED blinks on for 500ms, off for 500ms
 * - "LED ON" and "LED OFF" messages printed to USB serial
 */

#include "pico/stdlib.h"
#include <stdio.h>

// GPIO pin definitions
const uint LED_PIN = 25;  // Onboard LED

// Timing constants
const uint32_t BLINK_INTERVAL_MS = 500;

int main() {
    // Initialize stdio for USB serial output
    stdio_init_all();

    // Initialize GPIO pin for LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Wait for USB serial connection (optional, for debugging)
    sleep_ms(2000);

    printf("\n=================================\n");
    printf("Pico 2 LED Blink Test\n");
    printf("=================================\n");
    printf("LED Pin: GPIO %d\n", LED_PIN);
    printf("Blink Rate: %d ms ON, %d ms OFF\n", BLINK_INTERVAL_MS, BLINK_INTERVAL_MS);
    printf("=================================\n\n");

    uint32_t blink_count = 0;

    // Main loop
    while (true) {
        // Turn LED on
        gpio_put(LED_PIN, 1);
        printf("LED ON  (count: %lu)\n", blink_count);
        sleep_ms(BLINK_INTERVAL_MS);

        // Turn LED off
        gpio_put(LED_PIN, 0);
        printf("LED OFF (count: %lu)\n", blink_count);
        sleep_ms(BLINK_INTERVAL_MS);

        blink_count++;
    }

    return 0;
}
