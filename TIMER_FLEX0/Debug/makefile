################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := usbdm-rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup_Code/subdir.mk
-include Sources/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
MAP_FILE_OUTPUTS += \
TIMER_FLEX0.map \

ARCHIVES += \
libUSER_OBJS.a \

SIZE_FILE_OUTPUTS += \
TIMER_FLEX0.size \

SYMBOL_LIST_FILE_OUTPUTS += \
TIMER_FLEX0.sym \


# All Target
all: TIMER_FLEX0.elf secondary-outputs

# Tool invocations
TIMER_FLEX0.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g3 -O0 -Wl,--gc-sections -L"C:/Users/Michael/workspace.kds/TIMER_FLEX0/Project_Settings/Linker_Files" --specs=nano.specs -T "Linker-rom.ld" -Xlinker -Map="TIMER_FLEX0.map"  -o "TIMER_FLEX0.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

TIMER_FLEX0.map: TIMER_FLEX0.elf $(OBJS) $(USER_OBJS)

libUSER_OBJS.a: $(OBJS) $(USER_OBJS)
	@echo 'Invoking: ARM Archiver'
	arm-none-eabi-ar -r   "libUSER_OBJS.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building: $@'
	@echo ' '

TIMER_FLEX0.size: TIMER_FLEX0.elf
	@echo 'Invoking: ARM Sizer'
	arm-none-eabi-size "TIMER_FLEX0.elf" --format=sysv --radix=10   >"TIMER_FLEX0.size"
	@echo 'Finished building: $@'
	@echo ' '

TIMER_FLEX0.sym: TIMER_FLEX0.elf
	@echo 'Invoking: ARM Symbol Table'
	arm-none-eabi-nm "TIMER_FLEX0.elf" --demangle --defined-only --extern-only --numeric-sort   >"TIMER_FLEX0.sym"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(MAP_FILE_OUTPUTS)$(ARCHIVES)$(SYMBOL_LIST_FILE_OUTPUTS)$(SIZE_FILE_OUTPUTS)$(EXECUTABLES)$(LIST_FILE_OUTPUTS) TIMER_FLEX0.elf
	-@echo ' '

secondary-outputs: $(LIST_FILE_OUTPUTS) $(SIZE_FILE_OUTPUTS) $(SYMBOL_LIST_FILE_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
