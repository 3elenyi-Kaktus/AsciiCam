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
        network.cpp
        network.h
        scroll_object.cpp
        scroll_object.h
        chat.cpp
        chat.h)
#        video_driver.cpp
#        video_driver.h)
set(ASAN_OPTIONS "log_path=</home/user/Desktop/log.txt>")
add_definitions(-DASAN_OPTIONS="${ASAN_OPTIONS}")
target_link_libraries(AsciiCam ${OpenCV_LIBS} -lncurses -fsanitize=address -static-libasan -g)