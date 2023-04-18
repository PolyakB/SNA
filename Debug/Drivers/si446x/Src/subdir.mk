################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/si446x/Src/si446x.c 

OBJS += \
./Drivers/si446x/Src/si446x.o 

C_DEPS += \
./Drivers/si446x/Src/si446x.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/si446x/Src/si446x.o: ../Drivers/si446x/Src/si446x.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F301x8 -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"D:/Projects/ScalarNetworkAnalyzer/SW/Drivers/si446x/Inc" -I"D:/Projects/ScalarNetworkAnalyzer/SW/Drivers/control/Inc" -I"D:/Projects/ScalarNetworkAnalyzer/SW/Drivers/statemachine/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/si446x/Src/si446x.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

