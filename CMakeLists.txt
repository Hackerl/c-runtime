cmake_minimum_required(VERSION 3.17)
project(c_runtime C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(c_runtime STATIC src/z_std.c src/z_memory.c src/z_syscall.c src/z_log.c src/z_sync.c printf/printf.c)
target_include_directories(c_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/printf)