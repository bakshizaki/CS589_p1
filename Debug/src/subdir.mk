################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/command_handling.c \
../src/common_methods.c \
../src/help_functions.c \
../src/ip_list.c \
../src/mbakshi_proj1.c \
../src/selectclient.c \
../src/selectserver.c 

OBJS += \
./src/command_handling.o \
./src/common_methods.o \
./src/help_functions.o \
./src/ip_list.o \
./src/mbakshi_proj1.o \
./src/selectclient.o \
./src/selectserver.o 

C_DEPS += \
./src/command_handling.d \
./src/common_methods.d \
./src/help_functions.d \
./src/ip_list.d \
./src/mbakshi_proj1.d \
./src/selectclient.d \
./src/selectserver.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


