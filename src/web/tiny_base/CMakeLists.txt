#tiny_base/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

add_definitions("-Wall -std=c++11")

add_library(tiny_base_model STATIC
  api.cpp
  buffer.cpp
  condition.cpp
  configer.cpp
  exception.cpp
  list_timer.cpp
  log.cpp
  file.cpp
  memorypool.cpp
  messagequeue.cpp
  semaphore.cpp
  sharedmemory.cpp
  signalmanager.cpp
  md5.cpp
)
