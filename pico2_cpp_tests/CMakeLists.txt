cmake_minimum_required(VERSION 3.13)

# Include Pico SDK
include(pico_sdk_import.cmake)

# Project
project(pico2_cpp_tests C CXX ASM)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# Initialize SDK
pico_sdk_init()

# Add all test subdirectories
add_subdirectory(01_blink)
add_subdirectory(02_serial_hello)
add_subdirectory(03_gpio_input)
add_subdirectory(04_pwm_led)
add_subdirectory(05_adc_read)
add_subdirectory(06_uart_echo)
add_subdirectory(07_i2c_scan)
add_subdirectory(08_spi_test)
add_subdirectory(09_multicore)
add_subdirectory(10_timer_interrupt)
