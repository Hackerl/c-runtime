cmake_minimum_required(VERSION 3.17)
project(c_runtime C)

set(CMAKE_C_STANDARD 99)

option(RUNTIME_MULTIPLE_THREADS "enable multi-thread support " ON)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

if (RUNTIME_MULTIPLE_THREADS)
    add_definitions(-DMULTIPLE_THREADS)
endif ()

add_library(c_runtime STATIC z_std.c z_memory.c z_syscall.c z_log.c printf/printf.c $<$<BOOL:${RUNTIME_MULTIPLE_THREADS}>:z_sync.c>)
target_include_directories(c_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})