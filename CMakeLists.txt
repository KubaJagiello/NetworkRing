cmake_minimum_required(VERSION 3.10)
project(Ring C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Ring
        node.c
        node.h network_helper.c network_helper.h queue.c queue.h)
