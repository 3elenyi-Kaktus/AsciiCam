cmake_minimum_required(VERSION 3.24)
project(AsciiCam)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(AsciiCam
        main.cpp
        printer.h
        printer.cpp
        ascii_converter.h
        ascii_converter.cpp
        webcam_capture.h
        webcam_capture.cpp
        executor.h
        executor.cpp
        logger.h
        logger.cpp
        client.cpp
        client.h
        scroll_object.cpp
        scroll_object.h
        chat.cpp
        chat.h)
target_link_libraries(AsciiCam ${OpenCV_LIBS} -lncurses)