################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CodingDecodingData.cpp \
../LibShine.cpp \
../client.cpp \
../utilityForTesting.cpp 

OBJS += \
./CodingDecodingData.o \
./LibShine.o \
./client.o \
./utilityForTesting.o 

CPP_DEPS += \
./CodingDecodingData.d \
./LibShine.d \
./client.d \
./utilityForTesting.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


