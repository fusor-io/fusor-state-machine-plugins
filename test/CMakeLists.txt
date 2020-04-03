cmake_minimum_required(VERSION 2.6)

#Set Arduino library root directory
set(ARDUINO_LIB_ROOT "../..")

message("IMPORTANT NOTE: update your ARDUINO_LIB_ROOT in a CMakeLists.txt file to meet your environment")
message(STATUS "ARDUINO_LIB_ROOT=${ARDUINO_LIB_ROOT}")

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

find_package(GTest REQUIRED)
message(STATUS "GTest: ${GTEST_INCLUDE_DIRS} ${GTEST_LIBRARIES}")

include_directories( ${GTEST_INCLUDE_DIRS} )
include_directories(.)
include_directories(mock)
include_directories(${ARDUINO_LIB_ROOT}/ArduinoJson/src)
include_directories(${ARDUINO_LIB_ROOT}/StateMachine/src)
include_directories(${ARDUINO_LIB_ROOT}/StateMachine/src/keycompare)
include_directories(${ARDUINO_LIB_ROOT}/StateMachine/src/timers)
include_directories(${ARDUINO_LIB_ROOT}/StateMachine/src/store)
include_directories(${ARDUINO_LIB_ROOT}/StateMachine/src/compute)
include_directories(${ARDUINO_LIB_ROOT}/StateMachine/src/plugin)
include_directories(${ARDUINO_LIB_ROOT}/BME280/src)
include_directories(../src/Bme280Plugin)

#Link runTests with what we want to test and the GTest and pthread library
add_executable(executeTests test.cpp 
    ../src/Bme280Plugin.cpp
    mock/Wire.cpp
    ${ARDUINO_LIB_ROOT}/BME280/src/BME280.cpp ${ARDUINO_LIB_ROOT}/BME280/src/BME280I2C.cpp
    ${ARDUINO_LIB_ROOT}/StateMachine/src/StateMachine.cpp
    ${ARDUINO_LIB_ROOT}/StateMachine/src/keycompare/keycompare.cpp
    ${ARDUINO_LIB_ROOT}/StateMachine/src/timers/timers.cpp
    ${ARDUINO_LIB_ROOT}/StateMachine/src/store/store.cpp
    ${ARDUINO_LIB_ROOT}/StateMachine/src/compute/compute.cpp
    ${ARDUINO_LIB_ROOT}/StateMachine/src/plugin/plugin.cpp
)
target_link_libraries(executeTests ${GTEST_LIBRARIES} pthread)

