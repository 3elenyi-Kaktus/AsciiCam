cmake_minimum_required(VERSION 3.24)
project(AsciiCam)

set(CMAKE_CXX_STANDARD 20)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

#add_executable(AsciiCam code/main.cpp code/printer.h code/printer.cpp code/ascii_converter.cpp code/webcam_capture.cpp)
add_executable(AsciiCam code/webcam_capture.cpp)
target_link_libraries(AsciiCam ${OpenCV_LIBS} -lncurses)