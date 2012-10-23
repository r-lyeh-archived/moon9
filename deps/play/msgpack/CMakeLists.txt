cmake_minimum_required(VERSION 2.8)

project(msgpack CXX)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    add_definitions("/Zi")
else()
    add_definitions("/Zi")
    add_definitions("/DNDEBUG /D_NDEBUG /Ox /Oy")
endif()

include_directories(src)

add_library(msgpack STATIC
    src/gcc_atomic.cpp
    src/object.cpp
    src/objectc.cpp
    src/unpack.cpp
    src/version.cpp
    src/vrefbuffer.cpp
    src/zone.cpp
)
