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
        devices/webcam_capture.h
        devices/webcam_capture.cpp
        executor.h
        executor.cpp
        logger.h
        logger.cpp
        network.cpp
        network.h
        scroll_object.cpp
        scroll_object.h
        windows/chat.cpp
        windows/chat.h
        devices/video_driver.cpp
        devices/video_driver.h
        definitions.h
        devices/input_device.cpp
        devices/input_device.h
        window.h
        devices/terminal.cpp
        devices/terminal.h
        windows/video_gallery.cpp
        windows/video_gallery.h)
set(ASAN_OPTIONS "log_path=</home/user/Desktop/log.txt>")
add_definitions(-DASAN_OPTIONS="${ASAN_OPTIONS}")
target_link_libraries(AsciiCam ${OpenCV_LIBS} -lncursesw -fsanitize=address -static-libasan -g)