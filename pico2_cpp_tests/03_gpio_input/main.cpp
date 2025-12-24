/**
 * @file main.cpp
 * @brief GPIO Button Input Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program reads a button connected to a GPIO pin and reports state
 * changes via USB serial. Includes debouncing to avoid false triggers.
 *
 * Hardware:
 * - Button connected between GPIO 14 and GND
 * - Internal pull-up resistor enabled
 * - LED on GPIO 25 (optional, mirrors button state)
 *
 * Expected Behavior:
 * - Button press detected and reported
 * - Button release detected and reported
 * - Press duration measured and displayed
 * - LED mirrors button state (on when pressed)
 */

#include "pico/stdlib.h"
#include <stdio.h>

// GPIO pin definitions
const uint BUTTON_PIN = 14;  // Button input with pull-up
const uint LED_PIN = 25;     // Onboard LED for visual feedback

// Debounce timing
const uint32_t DEBOUNCE_MS = 50;  // 50ms debounce time

// Button state tracking
bool last_button_state = true;  // true = not pressed (pull-up)
uint64_t press_start_time = 0;
uint32_t press_count = 0;

/**
 * @brief Read button state with debouncing
 * @return true if button is currently pressed (LOW due to pull-up)
 */
bool read_button_debounced() {
    bool current_state = gpio_get(BUTTON_PIN);

    // Wait for signal to stabilize
    sleep_ms(DEBOUNCE_MS);

    // Verify state hasn't changed during debounce period
    bool stable_state = gpio_get(BUTTON_PIN);

    if (current_state == stable_state) {
        return !stable_state;  // Invert because pull-up (LOW = pressed)
    }

    // State changed during debounce, return last known state
    return !last_button_state;
}

/**
 * @brief Process button state change
 */
void process_button_state() {
    bool current_button_state = gpio_get(BUTTON_PIN);

    // Check for state change
    if (current_button_state != last_button_state) {
        bool button_pressed = !current_button_state;  // Invert for pull-up

        if (button_pressed) {
            // Button just pressed
            press_start_time = time_us_64();
            press_count++;
            printf("\n[PRESS #%lu] Button PRESSED at %.3f seconds\n",
                   press_count,
                   (double)press_start_time / 1000000.0);

            // Turn on LED
            gpio_put(LED_PIN, 1);

        } else {
            // Button just released
            uint64_t release_time = time_us_64();
            uint64_t duration_us = release_time - press_start_time;

            printf("[RELEASE #%lu] Button RELEASED at %.3f seconds\n",
                   press_count,
                   (double)release_time / 1000000.0);
            printf("              Press duration: %.3f ms\n",
                   (double)duration_us / 1000.0);

            // Turn off LED
            gpio_put(LED_PIN, 0);
        }

        last_button_state = current_button_state;
    }
}

int main() {
    // Initialize stdio for USB serial output
    stdio_init_all();

    // Initialize button pin with pull-up
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Initialize LED pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    // Wait for USB serial connection
    sleep_ms(2000);

    printf("\n");
    printf("========================================\n");
    printf("  Pico 2 GPIO Button Input Test\n");
    printf("========================================\n");
    printf("Button Pin: GPIO %d (with pull-up)\n", BUTTON_PIN);
    printf("LED Pin: GPIO %d\n", LED_PIN);
    printf("Debounce Time: %lu ms\n", DEBOUNCE_MS);
    printf("========================================\n");
    printf("\n");
    printf("Waiting for button presses...\n");
    printf("(Connect button between GPIO %d and GND)\n", BUTTON_PIN);
    printf("\n");

    // Read initial button state
    last_button_state = gpio_get(BUTTON_PIN);

    // Main loop
    while (true) {
        process_button_state();

        // Small delay to reduce CPU usage
        sleep_ms(10);
    }

    return 0;
}
