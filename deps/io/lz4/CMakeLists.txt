cmake_minimum_required(VERSION 2.8)

project(lz4 C)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    add_definitions("/Zi")
else()
    add_definitions("/Zi")
    add_definitions("/DNDEBUG /D_NDEBUG /Ox /Oy")
endif()

#include_directories(src)

add_library(lz4 STATIC
    lz4.c
    lz4.h
    lz4hc.c
    lz4hc.h
)
