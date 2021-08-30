cmake_minimum_required(VERSION 3.17)
project(c_runtime C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(c_runtime STATIC z_std.c z_memory.c z_syscall.c z_log.c printf/printf.c z_sync.c)
target_include_directories(c_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})