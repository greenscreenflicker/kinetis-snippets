################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Startup_Code/clock.c \
../Startup_Code/console.c \
../Startup_Code/newlib_stubs.c \
../Startup_Code/system.c \
../Startup_Code/uart1.c \
../Startup_Code/vectors.c 

S_UPPER_SRCS += \
../Startup_Code/startup_ARMLtdGCC.S 

OBJS += \
./Startup_Code/clock.o \
./Startup_Code/console.o \
./Startup_Code/newlib_stubs.o \
./Startup_Code/startup_ARMLtdGCC.o \
./Startup_Code/system.o \
./Startup_Code/uart1.o \
./Startup_Code/vectors.o 

C_DEPS += \
./Startup_Code/clock.d \
./Startup_Code/console.d \
./Startup_Code/newlib_stubs.d \
./Startup_Code/system.d \
./Startup_Code/uart1.d \
./Startup_Code/vectors.d 


# Each subdirectory must supply rules for building sources it contributes
Startup_Code/%.o: ../Startup_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g3 -O0 -ffunction-sections -fdata-sections -DDEBUG_BUILD -DCPU_MK22FN512VLH12 -I"C:/Users/Michael/workspace.kds/uart-dma/Sources" -I"C:/Users/Michael/workspace.kds/uart-dma/Project_Headers" -Wall -std=c11 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -o "$@" $<
	@echo 'Finished building: $<'
	@echo ' '

Startup_Code/%.o: ../Startup_Code/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g3 -O0 -ffunction-sections -fdata-sections -x assembler-with-cpp -DDEBUG_BUILD -I"C:/Users/Michael/workspace.kds/uart-dma/Project_Headers" -Wall -Wextra -c -fmessage-length=0  -o "$@" $<
	@echo 'Finished building: $<'
	@echo ' '


