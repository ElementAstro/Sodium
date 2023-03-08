#tiny_struct/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

add_definitions("-Wall -std=c++11")

add_library(tiny_struct_model STATIC
    dict_t.cpp
    list_t.cpp
    rbtree_t.cpp
    sdstr_t.cpp
)
