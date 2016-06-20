################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/delay.c \
../Sources/leds.c \
../Sources/main.c 

OBJS += \
./Sources/delay.o \
./Sources/leds.o \
./Sources/main.o 

C_DEPS += \
./Sources/delay.d \
./Sources/leds.d \
./Sources/main.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g3 -O0 -ffunction-sections -fdata-sections -DDEBUG_BUILD -DCPU_MK22FN512VLH12 -I"C:/Users/Michael/workspace.kds/reference2/Sources" -I"C:/Users/Michael/workspace.kds/reference2/Project_Headers" -Wall -std=c11 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -o "$@" $<
	@echo 'Finished building: $<'
	@echo ' '


