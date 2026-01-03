################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Drivers/BSP/Adafruit_Shield/stm32_adafruit_lcd.c \
C:/Users/Drivers/BSP/Adafruit_Shield/stm32_adafruit_sd.c 

OBJS += \
./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_lcd.o \
./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_sd.o 

C_DEPS += \
./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_lcd.d \
./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_sd.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Adafruit\ Shield/stm32_adafruit_lcd.o: C:/Users/Drivers/BSP/Adafruit_Shield/stm32_adafruit_lcd.c Drivers/BSP/Adafruit\ Shield/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STM32F4XX_NUCLEO_144 -c -I../../Inc -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Drivers/BSP/Adafruit Shield/stm32_adafruit_lcd.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Adafruit\ Shield/stm32_adafruit_sd.o: C:/Users/Drivers/BSP/Adafruit_Shield/stm32_adafruit_sd.c Drivers/BSP/Adafruit\ Shield/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STM32F4XX_NUCLEO_144 -c -I../../Inc -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Drivers/BSP/Adafruit Shield/stm32_adafruit_sd.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Adafruit-20-Shield

clean-Drivers-2f-BSP-2f-Adafruit-20-Shield:
	-$(RM) ./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_lcd.cyclo ./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_lcd.d ./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_lcd.o ./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_lcd.su ./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_sd.cyclo ./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_sd.d ./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_sd.o ./Drivers/BSP/Adafruit\ Shield/stm32_adafruit_sd.su

.PHONY: clean-Drivers-2f-BSP-2f-Adafruit-20-Shield

