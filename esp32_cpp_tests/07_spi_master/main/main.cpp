#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "SPI_MASTER";

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5

extern "C" void app_main(void)
{
    esp_err_t ret;
    spi_device_handle_t spi;
    spi_bus_config_t buscfg = {};
    buscfg.miso_io_num = PIN_NUM_MISO;
    buscfg.mosi_io_num = PIN_NUM_MOSI;
    buscfg.sclk_io_num = PIN_NUM_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 32;

    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 10*1000*1000;           //Clock out at 10 MHz
    devcfg.mode = 0;                                //SPI mode 0
    devcfg.spics_io_num = PIN_NUM_CS;               //CS pin
    devcfg.queue_size = 7;                          //We want to be able to queue 7 transactions at a time

    //Initialize the SPI bus
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    //Attach the LCD to the SPI bus
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    char sendbuf[20] = "Hello ESP32 SPI!";
    char recvbuf[20] = {0};
    spi_transaction_t t = {};
    t.length = sizeof(sendbuf) * 8;                 //Len is in bits, transaction length
    t.tx_buffer = sendbuf;                          //Data
    t.rx_buffer = recvbuf;

    while(1) {
        ret = spi_device_transmit(spi, &t);  //Transmit!
        ESP_ERROR_CHECK(ret);
        ESP_LOGI(TAG, "Transmitted: %s", sendbuf);
        ESP_LOGI(TAG, "Received: %s", recvbuf);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
