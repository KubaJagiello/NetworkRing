cmake_minimum_required(VERSION 3.10)
project(Ring C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Ring
        node.c
        node.h network_helper.c network_helper.h queue.c queue.h queue_test.c exercises/netcat-server-tcp.c exercises/netcat-client-tcp.c exercises/netcat-client-udp.c exercises/netcat-server-udp.c socket_helper.c socket_helper.h message_helper.c message_helper.h message_helper_test.c)
find_package(Threads)
target_link_libraries(Ring ${CMAKE_THREAD_LIBS_INIT})