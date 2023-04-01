cmake_minimum_required(VERSION 3.24)
project(AsciiCam)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(AsciiCam
        code/main.cpp
        code/printer.h
        code/printer.cpp
        code/ascii_converter.h
        code/ascii_converter.cpp
        code/webcam_capture.h
        code/webcam_capture.cpp
        code/executor.h
        code/executor.cpp
        code/logger.h
        code/logger.cpp
        code/server.h
        code/server.cpp code/client.cpp code/client.h)
target_link_libraries(AsciiCam ${OpenCV_LIBS} -lncurses)