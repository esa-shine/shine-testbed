################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CheckFunction.cpp \
../ConflictGraph.cpp \
../DecodingInput.cpp \
../EnvironmentSetup.cpp \
../LibShine.cpp \
../grasp.cpp \
../heapSort.cpp \
../hgcc.cpp \
../psdes.cpp \
../randomHandler.cpp \
../randomUniform.cpp \
../server.cpp \
../utilityForTesting.cpp 

OBJS += \
./CheckFunction.o \
./ConflictGraph.o \
./DecodingInput.o \
./EnvironmentSetup.o \
./LibShine.o \
./grasp.o \
./heapSort.o \
./hgcc.o \
./psdes.o \
./randomHandler.o \
./randomUniform.o \
./server.o \
./utilityForTesting.o 

CPP_DEPS += \
./CheckFunction.d \
./ConflictGraph.d \
./DecodingInput.d \
./EnvironmentSetup.d \
./LibShine.d \
./grasp.d \
./heapSort.d \
./hgcc.d \
./psdes.d \
./randomHandler.d \
./randomUniform.d \
./server.d \
./utilityForTesting.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/matteo/gsl/lib -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


