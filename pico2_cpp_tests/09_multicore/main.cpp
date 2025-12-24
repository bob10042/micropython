/**
 * @file main.cpp
 * @brief Multicore (Dual-Core) Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program demonstrates dual-core programming on the RP2350 by running
 * separate tasks on each of the two ARM Cortex-M33 cores.
 *
 * Hardware:
 * - LED on GPIO 25 (Core 0) - onboard LED
 * - LED on GPIO 16 (Core 1) - external LED (optional)
 *
 * Expected Behavior:
 * - Core 0: Blinks LED at 1 Hz, prints status messages
 * - Core 1: Blinks different LED at 2 Hz, counts iterations
 * - Both cores run simultaneously and independently
 * - Demonstrates inter-core communication via FIFO
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <stdio.h>

// GPIO pin definitions
const uint LED_CORE0_PIN = 25;  // Onboard LED for Core 0
const uint LED_CORE1_PIN = 16;  // External LED for Core 1

// Timing parameters
const uint32_t CORE0_BLINK_MS = 1000;  // 1 Hz (1000ms period)
const uint32_t CORE1_BLINK_MS = 500;   // 2 Hz (500ms period)

// Core 1 statistics (updated by Core 1)
volatile uint32_t core1_iterations = 0;
volatile uint32_t core1_blinks = 0;

/**
 * @brief Core 1 entry point
 *
 * This function runs on Core 1. It blinks an LED at a different rate
 * than Core 0 and sends periodic status updates via the inter-core FIFO.
 */
void core1_entry() {
    // Initialize LED pin for Core 1
    gpio_init(LED_CORE1_PIN);
    gpio_set_dir(LED_CORE1_PIN, GPIO_OUT);

    printf("[CORE 1] Started successfully!\n");
    printf("[CORE 1] Blinking LED on GPIO %d at %lu ms intervals\n",
           LED_CORE1_PIN, CORE1_BLINK_MS);

    uint32_t local_blink_count = 0;

    while (true) {
        // Turn LED on
        gpio_put(LED_CORE1_PIN, 1);
        sleep_ms(CORE1_BLINK_MS / 2);

        // Turn LED off
        gpio_put(LED_CORE1_PIN, 0);
        sleep_ms(CORE1_BLINK_MS / 2);

        local_blink_count++;
        core1_blinks = local_blink_count;

        // Send status to Core 0 every 10 blinks via FIFO
        if (local_blink_count % 10 == 0) {
            // Check if FIFO has space
            if (!multicore_fifo_wready()) {
                // FIFO full, drain it
                multicore_fifo_drain();
            }

            // Send blink count to Core 0
            multicore_fifo_push_blocking(local_blink_count);
        }

        core1_iterations++;
    }
}

/**
 * @brief Check for messages from Core 1
 */
void check_core1_messages() {
    // Check if data available in FIFO
    while (multicore_fifo_rvalid()) {
        uint32_t message = multicore_fifo_pop_blocking();
        printf("[CORE 0] Received from Core 1: %lu blinks completed\n", message);
    }
}

/**
 * @brief Display system status
 */
void display_status(uint32_t core0_blinks) {
    printf("\n========== Status ==========\n");
    printf("Time: %.3f seconds\n", (float)time_us_64() / 1000000.0f);
    printf("\n");
    printf("Core 0:\n");
    printf("  Running on: Core %d\n", get_core_num());
    printf("  LED Pin: GPIO %d\n", LED_CORE0_PIN);
    printf("  Blink Rate: %lu ms\n", CORE0_BLINK_MS);
    printf("  Blinks: %lu\n", core0_blinks);
    printf("\n");
    printf("Core 1:\n");
    printf("  LED Pin: GPIO %d\n", LED_CORE1_PIN);
    printf("  Blink Rate: %lu ms\n", CORE1_BLINK_MS);
    printf("  Blinks: %lu\n", core1_blinks);
    printf("  Iterations: %lu\n", core1_iterations);
    printf("============================\n\n");
}

int main() {
    // Initialize stdio for USB serial
    stdio_init_all();

    // Wait for USB serial connection
    sleep_ms(2000);

    printf("\n");
    printf("================================================\n");
    printf("  Pico 2 Multicore (Dual-Core) Test\n");
    printf("================================================\n");
    printf("Chip: RP2350 (Dual ARM Cortex-M33 cores)\n");
    printf("Current Core: Core %d\n", get_core_num());
    printf("================================================\n");
    printf("\n");

    // Initialize LED for Core 0
    gpio_init(LED_CORE0_PIN);
    gpio_set_dir(LED_CORE0_PIN, GPIO_OUT);

    printf("Hardware Configuration:\n");
    printf("  Core 0 LED: GPIO %d (onboard LED)\n", LED_CORE0_PIN);
    printf("  Core 1 LED: GPIO %d (external LED, optional)\n", LED_CORE1_PIN);
    printf("\n");
    printf("If you connect an LED to GPIO %d:\n", LED_CORE1_PIN);
    printf("  GPIO %d ----[220Ω]----[LED]---- GND\n", LED_CORE1_PIN);
    printf("\n");

    printf("Starting Core 1...\n");

    // Launch Core 1
    multicore_launch_core1(core1_entry);

    // Wait for Core 1 to initialize
    sleep_ms(500);

    printf("\n[CORE 0] Core 1 launched successfully!\n");
    printf("[CORE 0] Blinking LED on GPIO %d at %lu ms intervals\n",
           LED_CORE0_PIN, CORE0_BLINK_MS);
    printf("\n");
    printf("Both cores now running independently...\n");
    printf("================================================\n");

    uint32_t core0_blink_count = 0;
    uint32_t status_display_count = 0;

    // Main loop for Core 0
    while (true) {
        // Verify we're on Core 0
        if (get_core_num() != 0) {
            printf("ERROR: Main loop not running on Core 0!\n");
        }

        // Turn LED on
        gpio_put(LED_CORE0_PIN, 1);
        printf("[CORE 0] LED ON  (blink #%lu)\n", core0_blink_count);
        sleep_ms(CORE0_BLINK_MS / 2);

        // Check for messages from Core 1
        check_core1_messages();

        // Turn LED off
        gpio_put(LED_CORE0_PIN, 0);
        printf("[CORE 0] LED OFF (blink #%lu)\n", core0_blink_count);
        sleep_ms(CORE0_BLINK_MS / 2);

        core0_blink_count++;

        // Check for messages from Core 1
        check_core1_messages();

        // Display detailed status every 5 blinks
        if (core0_blink_count % 5 == 0) {
            display_status(core0_blink_count);
            status_display_count++;
        }

        // Demonstrate inter-core communication
        if (core0_blink_count % 10 == 0) {
            printf("\n[CORE 0] Communication Test:\n");
            printf("  Core 0 has blinked %lu times\n", core0_blink_count);
            printf("  Core 1 has blinked %lu times (ratio: %.2f:1)\n",
                   core1_blinks,
                   (float)core1_blinks / (float)core0_blink_count);
            printf("  Expected ratio: 2:1 (Core 1 blinks twice as fast)\n\n");
        }
    }

    return 0;
}
