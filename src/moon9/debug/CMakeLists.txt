cmake_minimum_required(VERSION 2.8)
project(MOON9_DEBUG C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

file(GLOB MOON9_DEBUG_SRCS *.cpp *.hpp *.inl)
file(GLOB MOON9_DEBUG_SAMPLES *.sample.cpp)
file(GLOB MOON9_DEBUG_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_DEBUG_SRCS "" ${MOON9_DEBUG_SAMPLES} ${MOON9_DEBUG_TESTS})
## list(APPEND MOON9_DEBUG_SRCS platform/w32/others.cpp)
add_library(moon9-debug-static STATIC ${MOON9_DEBUG_SRCS})

add_executable(assert.sample                     assert.sample.cpp)
add_executable(debugger.sample                   debugger.sample.cpp)
add_executable(callstack.sample                  callstack/callstack.sample.cpp)

target_link_libraries(assert.sample              moon9-debug-static)
target_link_libraries(debugger.sample            moon9-debug-static)
target_link_libraries(callstack.sample           moon9-debug-static)
