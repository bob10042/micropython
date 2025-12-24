#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "BUTTON_EXAMPLE";

#define BUTTON_GPIO GPIO_NUM_0  // Boot button on most ESP32 boards
#define GPIO_INPUT_PIN_SEL  (1ULL<<BUTTON_GPIO)

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Initializing button example...");

    // Configure the button GPIO
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;      // Disable interrupt
    io_conf.mode = GPIO_MODE_INPUT;             // Set as input mode
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;  // Bit mask of the pins that you want to set
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // Disable pull-down mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      // Enable pull-up mode
    gpio_config(&io_conf);

    int last_state = -1;

    while(1) {
        int state = gpio_get_level(BUTTON_GPIO);
        
        if (state != last_state) {
            if (state == 0) {
                ESP_LOGI(TAG, "Button PRESSED");
            } else {
                ESP_LOGI(TAG, "Button RELEASED");
            }
            last_state = state;
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS); // Debounce / poll delay
    }
}
