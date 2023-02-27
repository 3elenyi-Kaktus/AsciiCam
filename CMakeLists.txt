cmake_minimum_required(VERSION 3.24)
project(AsciiCam)

set(CMAKE_CXX_STANDARD 20)



add_executable(AsciiCam code/main.cpp code/printer.h code/printer.cpp code/ascii_converter.cpp)
target_link_libraries(AsciiCam -lncurses)