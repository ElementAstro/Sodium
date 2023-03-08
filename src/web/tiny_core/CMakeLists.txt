#tiny_core/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

add_definitions("-Wall -std=c++11")

add_library(tiny_core_model STATIC
  accepter.cpp
  channel.cpp
  client.cpp
  clientpool.cpp
  connection.cpp
  connector.cpp
  currentthread.cpp
  defer.cpp
  epoller.cpp
  eventloop.cpp
  factory.cpp
  master.cpp
  netaddress.cpp
  process.cpp
  processpool.cpp
  protocol.cpp
  server.cpp
  slave.cpp
  socket.cpp
  socketpair.cpp
  status.cpp
  thread.cpp
  threadpool.cpp
  time.cpp
  timer.cpp
  timerqueue.cpp
)
