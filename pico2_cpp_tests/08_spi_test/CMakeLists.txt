cmake_minimum_required(VERSION 3.13)

# Set target name to directory name
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Create executable
add_executable(${TARGET_NAME}
    main.cpp
)

# Link libraries
target_link_libraries(${TARGET_NAME}
    pico_stdlib
    hardware_spi
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(${TARGET_NAME} 1)
pico_enable_stdio_uart(${TARGET_NAME} 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${TARGET_NAME})
