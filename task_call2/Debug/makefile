################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup_Code/subdir.mk
-include Sources/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
MAP_FILE_OUTPUTS += \
task_call2.map \

ARCHIVES += \
libUSER_OBJS.a \

SIZE_FILE_OUTPUTS += \
task_call2.size \

SYMBOL_LIST_FILE_OUTPUTS += \
task_call2.sym \


# All Target
all: task_call2.elf secondary-outputs

# Tool invocations
task_call2.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m0plus -mthumb -g3 -O0 -Wl,--gc-sections -L"C:/Users/Michael Heidinger/workspace.kds/task_call2/Project_Settings/Linker_Files" --specs=nano.specs -T ""Linker-rom.ld"" -Xlinker -Map="task_call2.map"  -o "task_call2.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

task_call2.map: task_call2.elf $(OBJS) $(USER_OBJS)

libUSER_OBJS.a: $(OBJS) $(USER_OBJS)
	@echo 'Invoking: ARM Archiver'
	arm-none-eabi-ar -r   "libUSER_OBJS.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building: $@'
	@echo ' '

task_call2.size: task_call2.elf
	@echo 'Invoking: ARM Sizer'
	arm-none-eabi-size task_call2.elf --format=berkeley --radix=10   >"task_call2.size"
	@echo 'Finished building: $@'
	@echo ' '

task_call2.sym: task_call2.elf
	@echo 'Invoking: ARM Symbol Table'
	arm-none-eabi-nm task_call2.elf --demangle --defined-only --extern-only --numeric-sort   >"task_call2.sym"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(MAP_FILE_OUTPUTS)$(ARCHIVES)$(SYMBOL_LIST_FILE_OUTPUTS)$(CPP_DEPS)$(SIZE_FILE_OUTPUTS)$(EXECUTABLES)$(LIST_FILE_OUTPUTS)$(CXX_DEPS)$(C_UPPER_DEPS) task_call2.elf
	-@echo ' '

secondary-outputs: $(LIST_FILE_OUTPUTS) $(SIZE_FILE_OUTPUTS) $(SYMBOL_LIST_FILE_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
