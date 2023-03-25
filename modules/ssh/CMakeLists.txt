set(${CMAKE_LIBRARY_PATH} ${CMAKE_BINARY_DIR})

find_package(OpenSSL REQUIRED)

add_library(ssh SHARED ssh.cpp ssh.hpp)
add_library(webssh SHARED webssh.cpp ssh.cpp ssh.hpp)