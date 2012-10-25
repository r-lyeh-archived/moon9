cmake_minimum_required(VERSION 2.8)
project(MOON9_MEMTRACER C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

## <moon9>
include_directories("../../../src")

file(GLOB MOON9_MEMTRACER_SRCS *.cpp *.hpp)
file(GLOB MOON9_MEMTRACER_SAMPLES *.sample.cpp)
file(GLOB MOON9_MEMTRACER_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_MEMTRACER_SRCS "" ${MOON9_MEMTRACER_SAMPLES} ${MOON9_MEMTRACER_TESTS})
## list(APPEND MOON9_MEMTRACER_SRCS platform/w32/others.cpp)
add_library(moon9-memtracer-static STATIC ${MOON9_MEMTRACER_SRCS})

add_executable(memtracer.sample.cpp             memtracer.sample.cpp ../debug/callstack.cpp)

target_link_libraries(memtracer.sample.cpp      moon9-memtracer-static)
