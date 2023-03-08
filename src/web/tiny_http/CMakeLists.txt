#tiny_http/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

add_definitions("-Wall -std=c++11")

add_library(tiny_http_model STATIC
  http_factory.cpp
  http_protocol.cpp
  http.cpp
  http_model_file.cpp
  http_model_gzip.cpp
  http_model_fcgi.cpp
  http_model_cache.cpp
  http_parser.cpp
  http_responser.cpp
  http_proxyer.cpp
  http_time.cpp
  str_t.cpp
)
