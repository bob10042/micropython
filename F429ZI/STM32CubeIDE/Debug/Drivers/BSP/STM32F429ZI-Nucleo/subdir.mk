################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Drivers/BSP/STM32F4xx_Nucleo_144/stm32f4xx_nucleo_144.c 

OBJS += \
./Drivers/BSP/STM32F429ZI-Nucleo/stm32f4xx_nucleo_144.o 

C_DEPS += \
./Drivers/BSP/STM32F429ZI-Nucleo/stm32f4xx_nucleo_144.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32F429ZI-Nucleo/stm32f4xx_nucleo_144.o: C:/Users/Drivers/BSP/STM32F4xx_Nucleo_144/stm32f4xx_nucleo_144.c Drivers/BSP/STM32F429ZI-Nucleo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_STM32F4XX_NUCLEO_144 -c -I../../Inc -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/bob43/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-STM32F429ZI-2d-Nucleo

clean-Drivers-2f-BSP-2f-STM32F429ZI-2d-Nucleo:
	-$(RM) ./Drivers/BSP/STM32F429ZI-Nucleo/stm32f4xx_nucleo_144.cyclo ./Drivers/BSP/STM32F429ZI-Nucleo/stm32f4xx_nucleo_144.d ./Drivers/BSP/STM32F429ZI-Nucleo/stm32f4xx_nucleo_144.o ./Drivers/BSP/STM32F429ZI-Nucleo/stm32f4xx_nucleo_144.su

.PHONY: clean-Drivers-2f-BSP-2f-STM32F429ZI-2d-Nucleo

