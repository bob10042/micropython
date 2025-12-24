/**
 * @file main.cpp
 * @brief PWM LED Fading Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program demonstrates hardware PWM control by smoothly fading an LED
 * in and out using the RP2350's PWM hardware.
 *
 * Hardware:
 * - LED on GPIO 15 (with current-limiting resistor)
 * - Can also use onboard LED on GPIO 25
 *
 * Expected Behavior:
 * - LED fades in (0% to 100% brightness)
 * - LED fades out (100% to 0% brightness)
 * - Smooth, continuous fading effect
 * - Duty cycle percentage reported via USB serial
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>
#include <math.h>

// GPIO pin definitions
const uint LED_PIN = 15;  // PWM-capable GPIO pin

// PWM configuration
const uint32_t PWM_FREQUENCY = 1000;  // 1 kHz PWM frequency
const uint16_t PWM_STEPS = 256;       // Number of brightness levels (0-255)

// Fade parameters
const uint32_t FADE_STEP_MS = 10;     // Delay between brightness steps
const bool USE_GAMMA_CORRECTION = true; // Apply gamma correction for perceptually linear fading

/**
 * @brief Apply gamma correction for perceptually linear LED brightness
 * @param value Linear brightness value (0-255)
 * @return Gamma-corrected value (0-255)
 */
uint8_t apply_gamma(uint8_t value) {
    // Gamma correction factor (2.2 is typical for LEDs)
    const float gamma = 2.2f;
    float normalized = (float)value / 255.0f;
    float corrected = powf(normalized, gamma);
    return (uint8_t)(corrected * 255.0f);
}

/**
 * @brief Set LED brightness using PWM
 * @param brightness Brightness level (0-255)
 */
void set_led_brightness(uint8_t brightness) {
    uint8_t effective_brightness = USE_GAMMA_CORRECTION ? apply_gamma(brightness) : brightness;

    // Get PWM slice for this GPIO
    uint slice_num = pwm_gpio_to_slice_num(LED_PIN);

    // Calculate duty cycle (16-bit value)
    uint16_t level = (uint16_t)((effective_brightness * 65535) / 255);

    // Set PWM level
    pwm_set_gpio_level(LED_PIN, level);
}

/**
 * @brief Initialize PWM for LED control
 */
void init_pwm_led() {
    // Set GPIO function to PWM
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);

    // Get PWM slice for this GPIO
    uint slice_num = pwm_gpio_to_slice_num(LED_PIN);

    // Configure PWM
    pwm_config config = pwm_get_default_config();

    // Set frequency by adjusting clock divider and wrap value
    // System clock is typically 125 MHz
    float clock_freq = 125000000.0f;
    float divider = clock_freq / (PWM_FREQUENCY * 65536);
    pwm_config_set_clkdiv(&config, divider);

    // Initialize the PWM slice
    pwm_init(slice_num, &config, true);

    printf("PWM initialized on GPIO %d (slice %d)\n", LED_PIN, slice_num);
    printf("PWM Frequency: %lu Hz\n", PWM_FREQUENCY);
    printf("Clock Divider: %.2f\n", divider);
}

/**
 * @brief Fade LED in (0% to 100%)
 */
void fade_in() {
    for (uint16_t i = 0; i < PWM_STEPS; i++) {
        set_led_brightness(i);

        // Print every 10% increment
        if (i % 26 == 0) {
            float percent = (float)i / (PWM_STEPS - 1) * 100.0f;
            printf("Fading IN:  %3.0f%% brightness\n", percent);
        }

        sleep_ms(FADE_STEP_MS);
    }
}

/**
 * @brief Fade LED out (100% to 0%)
 */
void fade_out() {
    for (int16_t i = PWM_STEPS - 1; i >= 0; i--) {
        set_led_brightness(i);

        // Print every 10% decrement
        if (i % 26 == 0) {
            float percent = (float)i / (PWM_STEPS - 1) * 100.0f;
            printf("Fading OUT: %3.0f%% brightness\n", percent);
        }

        sleep_ms(FADE_STEP_MS);
    }
}

int main() {
    // Initialize stdio for USB serial output
    stdio_init_all();

    // Wait for USB serial connection
    sleep_ms(2000);

    printf("\n");
    printf("========================================\n");
    printf("  Pico 2 PWM LED Fading Test\n");
    printf("========================================\n");
    printf("LED Pin: GPIO %d\n", LED_PIN);
    printf("PWM Frequency: %lu Hz\n", PWM_FREQUENCY);
    printf("Brightness Steps: %u (0-%u)\n", PWM_STEPS, PWM_STEPS - 1);
    printf("Fade Step Delay: %lu ms\n", FADE_STEP_MS);
    printf("Gamma Correction: %s\n", USE_GAMMA_CORRECTION ? "Enabled" : "Disabled");
    printf("========================================\n");
    printf("\n");

    // Initialize PWM
    init_pwm_led();

    printf("\nStarting fade cycle...\n\n");

    uint32_t cycle_count = 0;

    // Main loop - continuous fading
    while (true) {
        printf("--- Cycle #%lu ---\n", cycle_count);

        // Fade in
        fade_in();
        sleep_ms(500);  // Hold at full brightness

        // Fade out
        fade_out();
        sleep_ms(500);  // Hold at zero brightness

        printf("\n");
        cycle_count++;
    }

    return 0;
}
