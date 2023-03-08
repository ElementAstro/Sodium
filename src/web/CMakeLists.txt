# main CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

set (TINYWEB_VERSION "\"TnyWeb/0.0.8\"")
set (SHARED_MEMORY_ANON 1)
set (SHARED_MEMORY_DEVZERO 0)
set (SHARED_MEMORY_SYSVSHM 0)

message("-- Web Server version ${TINYWEB_VERSION}")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/TinyWebConfig.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/TinyWebConfig.h"
  )

add_definitions("-Wall -std=c++11")

SET (SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${SOURCE_DIR})

add_subdirectory("${SOURCE_DIR}/tiny_base")
add_subdirectory("${SOURCE_DIR}/tiny_core")
add_subdirectory("${SOURCE_DIR}/tiny_http")
add_subdirectory("${SOURCE_DIR}/tiny_struct")


