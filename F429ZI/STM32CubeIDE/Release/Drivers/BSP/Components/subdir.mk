################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Drivers/BSP/Components/st7735/st7735.c 

OBJS += \
./Drivers/BSP/Components/st7735.o 

C_DEPS += \
./Drivers/BSP/Components/st7735.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/st7735.o: C:/Users/Drivers/BSP/Components/st7735/st7735.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../../Inc -I../../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components

clean-Drivers-2f-BSP-2f-Components:
	-$(RM) ./Drivers/BSP/Components/st7735.cyclo ./Drivers/BSP/Components/st7735.d ./Drivers/BSP/Components/st7735.o ./Drivers/BSP/Components/st7735.su

.PHONY: clean-Drivers-2f-BSP-2f-Components

