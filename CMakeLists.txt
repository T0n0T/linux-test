cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_C_COMPILER "/linux-dev/OK113i-linux-sdk/out/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER "/linux-dev/OK113i-linux-sdk/out/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-g++")
project(linux-test VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(serial-test)
add_subdirectory(beep-test)
add_subdirectory(can-test)
add_subdirectory(eth-test)
add_subdirectory(rtc-test)
