cmake_minimum_required(VERSION 3.5.2)
project(server)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c command_parser.c command_parser.h  request_handler.c request_handler.h string_helpers.c string_helpers.h configuration.c configuration.h)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
link_libraries(/usr/local/lib/libxml2.so.2.7.2 -lxml2 -lz -lm)
link_libraries(/usr/local/lib/libconfig.so -lconfig -lz -lm)
add_executable(server ${SOURCE_FILES})
file(COPY "config" DESTINATION "${CMAKE_BINARY_DIR}")