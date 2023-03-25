set(${CMAKE_LIBRARY_PATH} ${CMAKE_BINARY_DIR})

add_library(imgplugin SHARED image.cpp CImg.h)
