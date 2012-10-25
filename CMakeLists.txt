cmake_minimum_required(VERSION 2.8)
#cmake_policy(SET CMP0014 OLD) # do not warn about empty directories

project(ROOT C CXX)

#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(BUILD_FRAMEWORKS "Enable this to build *all* frameworks" ON)
option(BUILD_DEMOS      "Enable this to build *all* demos"      ON)
option(BUILD_TESTS      "Enable this to build *all* tests"      ON)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

## include <moon9/*>
include_directories("src")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(BUILD_FRAMEWORKS)
##  add_subdirectory("src/moon9/core"       ./temp/core)
    add_subdirectory("src/moon9/debug"      ./temp/debug)
    add_subdirectory("src/moon9/hid"        ./temp/hid)
    add_subdirectory("src/moon9/io"         ./temp/io)
    add_subdirectory("src/moon9/math"       ./temp/math)
    add_subdirectory("src/moon9/memtracer"  ./temp/memtracer)
    add_subdirectory("src/moon9/os"         ./temp/os)
    add_subdirectory("src/moon9/play"       ./temp/play)
    add_subdirectory("src/moon9/render"     ./temp/render)
    add_subdirectory("src/moon9/spatial"    ./temp/spatial)
    add_subdirectory("src/moon9/string"     ./temp/string)
    add_subdirectory("src/moon9/test"       ./temp/test)
    add_subdirectory("src/moon9/time"       ./temp/time)
endif()

if(BUILD_TESTS)
    include_directories("deps/play/msgpack/src")
    file(GLOB msgpack-static "deps/play/msgpack/src/*.cpp")
	file(GLOB_RECURSE ALL_TESTS "src/moon9/*.test.cpp")
	add_executable(ALL.TESTS "src/moon9/test/test.cpp" ${msgpack-static} ${ALL_TESTS})
endif()
