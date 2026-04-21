cmake_minimum_required(VERSION 3.10)
project(ProcessMessaging)

set(CMAKE_CXX_STANDARD 17)

add_executable(Receiver Receiver.cpp)
add_executable(Sender Sender.cpp)

# Simple check-build test
enable_testing()
add_test(NAME BuildCheck COMMAND ${CMAKE_COMMAND} --build . --target Receiver)