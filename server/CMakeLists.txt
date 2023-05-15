cmake_minimum_required(VERSION 3.24)
project(Server)

set(CMAKE_CXX_STANDARD 20)

add_executable(Server
        main.cpp
        user_channel.cpp
        user_channel.h
        ../code/logger.cpp
        ../code/logger.h)
