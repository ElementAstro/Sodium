include_directories(${CMAKE_SOURCE_DIR}/libs/crow)

set(${CMAKE_LIBRARY_PATH} ${CMAKE_BINARY_DIR})

add_library(server SHARED app.cpp app.h ws.h url.h http.h)