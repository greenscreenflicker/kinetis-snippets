################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/Si8900.c \
../Sources/adc.c \
../Sources/adcdma1.c \
../Sources/flexpwm.c \
../Sources/leds.c \
../Sources/lowvoltagewarning.c \
../Sources/main.c \
../Sources/mcu_tracer.c \
../Sources/taskcall.c \
../Sources/taskcall_hal.c \
../Sources/uart1.c \
../Sources/vref.c \
../Sources/wdog.c 

OBJS += \
./Sources/Si8900.o \
./Sources/adc.o \
./Sources/adcdma1.o \
./Sources/flexpwm.o \
./Sources/leds.o \
./Sources/lowvoltagewarning.o \
./Sources/main.o \
./Sources/mcu_tracer.o \
./Sources/taskcall.o \
./Sources/taskcall_hal.o \
./Sources/uart1.o \
./Sources/vref.o \
./Sources/wdog.o 

C_DEPS += \
./Sources/Si8900.d \
./Sources/adc.d \
./Sources/adcdma1.d \
./Sources/flexpwm.d \
./Sources/leds.d \
./Sources/lowvoltagewarning.d \
./Sources/main.d \
./Sources/mcu_tracer.d \
./Sources/taskcall.d \
./Sources/taskcall_hal.d \
./Sources/uart1.d \
./Sources/vref.d \
./Sources/wdog.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fsingle-precision-constant  -g3 -I"../Sources" -I"../Includes" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


