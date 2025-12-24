/**
 * @file main.cpp
 * @brief ADC Reading Test for Raspberry Pi Pico 2 (RP2350)
 *
 * This program demonstrates analog input reading using the RP2350's built-in
 * ADC (Analog-to-Digital Converter). It reads the internal temperature sensor
 * and an external analog input, displaying values in multiple formats.
 *
 * Hardware:
 * - Internal temperature sensor (ADC channel 4)
 * - Optional: External analog input on GPIO 26 (ADC0)
 * - Optional: Potentiometer between 3.3V and GND, wiper to GPIO 26
 *
 * Expected Behavior:
 * - Reads ADC values continuously
 * - Converts raw ADC values to voltage
 * - Calculates temperature from internal sensor
 * - Displays readings every second via USB serial
 */

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

// ADC configuration
const uint ADC_EXTERNAL_PIN = 26;    // GPIO 26 = ADC0
const uint ADC_EXTERNAL_CHANNEL = 0; // ADC channel 0
const uint ADC_TEMP_CHANNEL = 4;     // Internal temperature sensor

// ADC reference voltage (RP2350 uses 3.3V ADC reference)
const float ADC_VREF = 3.3f;
const float ADC_CONVERT_FACTOR = ADC_VREF / (1 << 12);  // 12-bit ADC (0-4095)

// Sampling configuration
const uint32_t SAMPLE_INTERVAL_MS = 1000;  // Read every 1 second
const uint AVERAGE_SAMPLES = 10;           // Number of samples to average

/**
 * @brief Read ADC channel with averaging
 * @param channel ADC channel to read (0-4)
 * @return Averaged raw ADC value (0-4095)
 */
uint16_t read_adc_averaged(uint channel) {
    uint32_t sum = 0;

    // Select ADC channel
    adc_select_input(channel);

    // Take multiple samples and average
    for (uint i = 0; i < AVERAGE_SAMPLES; i++) {
        sum += adc_read();
        sleep_us(100);  // Small delay between samples
    }

    return (uint16_t)(sum / AVERAGE_SAMPLES);
}

/**
 * @brief Convert raw ADC value to voltage
 * @param raw_value Raw ADC reading (0-4095)
 * @return Voltage in volts (0.0 - 3.3V)
 */
float adc_to_voltage(uint16_t raw_value) {
    return (float)raw_value * ADC_CONVERT_FACTOR;
}

/**
 * @brief Read internal temperature sensor
 * @return Temperature in degrees Celsius
 */
float read_internal_temperature() {
    // Read temperature sensor ADC
    uint16_t raw_temp = read_adc_averaged(ADC_TEMP_CHANNEL);

    // Convert to voltage
    float voltage = adc_to_voltage(raw_temp);

    // Convert voltage to temperature
    // Typical formula: T = 27 - (ADC_voltage - 0.706) / 0.001721
    // These constants are from RP2040 datasheet and should be similar for RP2350
    const float temp_offset = 27.0f;
    const float voltage_offset = 0.706f;
    const float slope = 0.001721f;

    float temperature = temp_offset - (voltage - voltage_offset) / slope;

    return temperature;
}

/**
 * @brief Display ADC reading in multiple formats
 */
void display_adc_readings() {
    printf("\n--- ADC Readings ---\n");
    printf("Time: %.3f seconds\n", (float)time_us_64() / 1000000.0f);

    // Read external ADC (GPIO 26)
    uint16_t raw_external = read_adc_averaged(ADC_EXTERNAL_CHANNEL);
    float voltage_external = adc_to_voltage(raw_external);
    float percentage_external = ((float)raw_external / 4095.0f) * 100.0f;

    printf("\nExternal Input (GPIO %d, ADC%d):\n", ADC_EXTERNAL_PIN, ADC_EXTERNAL_CHANNEL);
    printf("  Raw value:  %4u / 4095 (0x%03X)\n", raw_external, raw_external);
    printf("  Voltage:    %.3f V\n", voltage_external);
    printf("  Percentage: %.1f%%\n", percentage_external);

    // Read internal temperature sensor
    float temperature = read_internal_temperature();
    float temperature_f = temperature * 9.0f / 5.0f + 32.0f;  // Convert to Fahrenheit

    printf("\nInternal Temperature Sensor (ADC%d):\n", ADC_TEMP_CHANNEL);
    printf("  Temperature: %.2f C (%.2f F)\n", temperature, temperature_f);

    // Visual bar graph for external input
    printf("\nBar Graph [");
    int bars = (int)(percentage_external / 5.0f);  // 20 bars for 100%
    for (int i = 0; i < 20; i++) {
        if (i < bars) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("] %.0f%%\n", percentage_external);
}

int main() {
    // Initialize stdio for USB serial output
    stdio_init_all();

    // Initialize ADC hardware
    adc_init();

    // Enable temperature sensor
    adc_set_temp_sensor_enabled(true);

    // Configure GPIO 26 as ADC input (ADC0)
    adc_gpio_init(ADC_EXTERNAL_PIN);

    // Wait for USB serial connection
    sleep_ms(2000);

    printf("\n");
    printf("========================================\n");
    printf("  Pico 2 ADC Reading Test\n");
    printf("========================================\n");
    printf("External ADC: GPIO %d (ADC%d)\n", ADC_EXTERNAL_PIN, ADC_EXTERNAL_CHANNEL);
    printf("Temperature:  ADC%d (internal sensor)\n", ADC_TEMP_CHANNEL);
    printf("Resolution:   12-bit (0-4095)\n");
    printf("Reference:    %.1f V\n", ADC_VREF);
    printf("Averaging:    %d samples\n", AVERAGE_SAMPLES);
    printf("Sample Rate:  Every %lu ms\n", SAMPLE_INTERVAL_MS);
    printf("========================================\n");
    printf("\n");
    printf("Optional: Connect potentiometer to GPIO %d\n", ADC_EXTERNAL_PIN);
    printf("  - One end to 3.3V\n");
    printf("  - Other end to GND\n");
    printf("  - Wiper (middle) to GPIO %d\n", ADC_EXTERNAL_PIN);
    printf("\n");

    uint32_t reading_count = 0;

    // Main loop
    while (true) {
        printf("\n========== Reading #%lu ==========\n", reading_count);
        display_adc_readings();

        reading_count++;
        sleep_ms(SAMPLE_INTERVAL_MS);
    }

    return 0;
}
