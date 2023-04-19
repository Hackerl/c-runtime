cmake_minimum_required(VERSION 3.17)
project(c-runtime C)

set(CMAKE_C_STANDARD 99)

option(C_RUNTIME_HEAP_CACHE "enable heap memory cache" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(C_RUNTIME_VERSION 1.0.0)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(
        c_runtime
        STATIC
        src/z_std.c
        src/z_memory.c
        src/z_syscall.c
        src/z_log.c
        src/z_sync.c
        printf/printf.c
)

if (NOT C_RUNTIME_HEAP_CACHE)
    target_compile_definitions(c_runtime PRIVATE DISABLE_CACHE)
endif ()

target_include_directories(
        c_runtime
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/printf>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
        FILES
        printf/printf.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        RENAME z_printf.h
)

install(
        DIRECTORY
        include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS c_runtime
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE runtime::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_package_config_file(
        cmake/${PROJECT_NAME}-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${C_RUNTIME_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)