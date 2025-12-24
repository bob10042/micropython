/**
 * @file main.cpp
 * @brief Hardware Timer Interrupt Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program demonstrates hardware timer interrupts on the RP2350.
 * A repeating timer triggers an interrupt at precise intervals, toggling
 * an LED and updating counters.
 *
 * Hardware:
 * - LED on GPIO 25 (onboard LED)
 * - Optional: LED on GPIO 16 for visual feedback
 *
 * Expected Behavior:
 * - Timer interrupt fires at 1 Hz (every 1 second)
 * - LED toggles in interrupt service routine (ISR)
 * - Main loop runs independently, printing timestamps
 * - Demonstrates interrupt-driven programming
 * - Shows accurate timing with hardware timer
 */

#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include <stdio.h>

// GPIO pin definitions
const uint LED_PIN = 25;  // Onboard LED, toggled by timer interrupt

// Timer configuration
const uint32_t TIMER_INTERVAL_US = 1000000;  // 1 second (1,000,000 microseconds)

// Shared variables (accessed by both ISR and main)
volatile uint32_t interrupt_count = 0;
volatile bool led_state = false;
volatile uint64_t last_interrupt_time = 0;

// Timing statistics
volatile uint64_t min_interval_us = UINT64_MAX;
volatile uint64_t max_interval_us = 0;
volatile uint64_t total_interval_us = 0;

/**
 * @brief Timer interrupt callback
 *
 * This function is called by the hardware timer interrupt.
 * It runs in interrupt context, so it should be fast and avoid
 * blocking operations.
 *
 * @param t Repeating timer structure
 * @return true to continue repeating, false to stop
 */
bool timer_callback(struct repeating_timer *t) {
    // Get current time
    uint64_t current_time = time_us_64();

    // Calculate interval since last interrupt
    if (interrupt_count > 0) {
        uint64_t interval = current_time - last_interrupt_time;

        // Update statistics
        if (interval < min_interval_us) min_interval_us = interval;
        if (interval > max_interval_us) max_interval_us = interval;
        total_interval_us += interval;
    }

    last_interrupt_time = current_time;

    // Toggle LED
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);

    // Increment counter
    interrupt_count++;

    // Return true to keep timer repeating
    return true;
}

/**
 * @brief Display interrupt statistics
 */
void display_statistics() {
    uint32_t local_count = interrupt_count;

    if (local_count == 0) {
        printf("No interrupts received yet.\n");
        return;
    }

    printf("\n========== Interrupt Statistics ==========\n");
    printf("Total interrupts: %lu\n", local_count);
    printf("LED state: %s\n", led_state ? "ON" : "OFF");
    printf("Time since boot: %.3f seconds\n", (double)time_us_64() / 1000000.0);

    if (local_count > 1) {
        double avg_interval = (double)total_interval_us / (local_count - 1);
        double expected = (double)TIMER_INTERVAL_US;
        double error = avg_interval - expected;
        double error_percent = (error / expected) * 100.0;

        printf("\nTiming Analysis:\n");
        printf("  Expected interval: %lu us (%.3f ms)\n",
               TIMER_INTERVAL_US, (double)TIMER_INTERVAL_US / 1000.0);
        printf("  Average interval:  %.3f us (%.3f ms)\n",
               avg_interval, avg_interval / 1000.0);
        printf("  Minimum interval:  %llu us (%.3f ms)\n",
               min_interval_us, (double)min_interval_us / 1000.0);
        printf("  Maximum interval:  %llu us (%.3f ms)\n",
               max_interval_us, (double)max_interval_us / 1000.0);
        printf("  Timing error:      %.3f us (%.6f%%)\n", error, error_percent);

        // Jitter calculation
        uint64_t jitter = max_interval_us - min_interval_us;
        printf("  Jitter:            %llu us\n", jitter);

        // Accuracy assessment
        printf("\nAccuracy Assessment:\n");
        if (fabs(error_percent) < 0.001) {
            printf("  EXCELLENT: Timing error < 0.001%%\n");
        } else if (fabs(error_percent) < 0.01) {
            printf("  GOOD: Timing error < 0.01%%\n");
        } else if (fabs(error_percent) < 0.1) {
            printf("  ACCEPTABLE: Timing error < 0.1%%\n");
        } else {
            printf("  WARNING: Timing error >= 0.1%%\n");
        }
    }

    printf("==========================================\n\n");
}

/**
 * @brief Display real-time interrupt info
 */
void display_interrupt_info() {
    // This is safe to read volatile variables in main loop
    uint32_t count = interrupt_count;
    bool state = led_state;
    uint64_t time = last_interrupt_time;

    printf("[Main Loop] Interrupts: %5lu | LED: %s | Last: %.3f s\n",
           count,
           state ? "ON " : "OFF",
           (double)time / 1000000.0);
}

int main() {
    // Initialize stdio for USB serial
    stdio_init_all();

    // Wait for USB serial connection
    sleep_ms(2000);

    printf("\n");
    printf("================================================\n");
    printf("  Pico 2 Hardware Timer Interrupt Test\n");
    printf("================================================\n");
    printf("Timer Interval: %lu us (%.3f seconds)\n",
           TIMER_INTERVAL_US, (double)TIMER_INTERVAL_US / 1000000.0);
    printf("Timer Frequency: %.3f Hz\n", 1000000.0 / TIMER_INTERVAL_US);
    printf("LED Pin: GPIO %d\n", LED_PIN);
    printf("================================================\n");
    printf("\n");

    // Initialize LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    printf("Initializing hardware timer...\n");

    // Create and start repeating timer
    struct repeating_timer timer;
    bool success = add_repeating_timer_us(
        TIMER_INTERVAL_US,    // Interval in microseconds
        timer_callback,        // Callback function
        NULL,                  // User data (not used)
        &timer                 // Timer structure
    );

    if (!success) {
        printf("ERROR: Failed to create repeating timer!\n");
        while (1) {
            tight_loop_contents();
        }
    }

    printf("Timer started successfully!\n");
    printf("Interrupt service routine (ISR) will toggle LED every %.3f seconds.\n",
           (double)TIMER_INTERVAL_US / 1000000.0);
    printf("Main loop continues running independently.\n");
    printf("\n");
    printf("Press Ctrl+C to stop.\n");
    printf("================================================\n");
    printf("\n");

    uint32_t loop_iterations = 0;
    uint32_t last_stats_display = 0;

    // Main loop - runs independently of timer interrupt
    while (true) {
        // Display current interrupt info
        display_interrupt_info();

        // Display detailed statistics every 10 interrupts
        if (interrupt_count > 0 && interrupt_count % 10 == 0 && interrupt_count != last_stats_display) {
            display_statistics();
            last_stats_display = interrupt_count;
        }

        // Show that main loop is running
        loop_iterations++;

        if (loop_iterations % 50 == 0) {
            printf("[Main Loop] Still running... (iteration %lu)\n", loop_iterations);
        }

        // Sleep to reduce serial output rate
        sleep_ms(100);
    }

    // This code is never reached, but shown for completeness
    cancel_repeating_timer(&timer);

    return 0;
}
