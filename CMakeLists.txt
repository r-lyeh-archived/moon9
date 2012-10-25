cmake_minimum_required(VERSION 2.8)
#cmake_policy(SET CMP0014 OLD) # do not warn about empty directories

project(ROOT C CXX)

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

if(BUILD_FRAMEWORKS)
##  add_subdirectory("src/moon9/core")
    add_subdirectory("src/moon9/debug")
    add_subdirectory("src/moon9/hid")
    add_subdirectory("src/moon9/io")
##  add_subdirectory("src/moon9/math")
    add_subdirectory("src/moon9/memtracer")
    add_subdirectory("src/moon9/os")
    add_subdirectory("src/moon9/play")
    add_subdirectory("src/moon9/render")
    add_subdirectory("src/moon9/spatial")
    add_subdirectory("src/moon9/string")
    add_subdirectory("src/moon9/test")
    add_subdirectory("src/moon9/time")

    file(GLOB_RECURSE ALL_BINARIES "*.sample.exe")
    foreach(f ${ALL_BINARIES})
        file(COPY ${f} DESTINATION ./)
    endforeach(f)
endif()

if(BUILD_TESTS)
    include_directories("deps/play/msgpack/src")
    file(GLOB msgpack-static "deps/play/msgpack/src/*.cpp")

	file(GLOB_RECURSE ALL_TESTS 				"src/moon9/*.test.cpp")
	add_executable(all.tests 					src/moon9/test/test.cpp ${msgpack-static} ${ALL_TESTS})
endif()
