################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ID_table.cpp \
../Knowledge.cpp \
../Token_stream.cpp \
../Tree_node.cpp \
../theorem_prover.cpp \
../util.cpp 

OBJS += \
./ID_table.o \
./Knowledge.o \
./Token_stream.o \
./Tree_node.o \
./theorem_prover.o \
./util.o 

CPP_DEPS += \
./ID_table.d \
./Knowledge.d \
./Token_stream.d \
./Tree_node.d \
./theorem_prover.d \
./util.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


