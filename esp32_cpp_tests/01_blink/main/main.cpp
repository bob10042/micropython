#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Define the LED pin - change this to your board's LED pin
// GPIO 2 is common for onboard LEDs on many ESP32 boards
#define BLINK_GPIO GPIO_NUM_2

static const char *TAG = "cpp_blink";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting C++ Blink Example");

    // Configure the GPIO
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    int counter = 0;

    while (1) {
        ESP_LOGI(TAG, "Blinking LED (count: %d)", counter++);
        
        // Turn LED ON
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Turn LED OFF
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
