cmake_minimum_required(VERSION 2.8)
project(MOON9_TEST C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

## include_directories( "../../../deps/test" )

file(GLOB MOON9_TEST_SRCS *.cpp *.hpp *.inl)
file(GLOB MOON9_TEST_SAMPLES *.sample.cpp)
file(GLOB MOON9_TEST_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_TEST_SRCS "" ${MOON9_TEST_SAMPLES} ${MOON9_TEST_TESTS})
## list(APPEND MOON9_TEST_SRCS platform/w32/others.cpp)
add_library(moon9-test-static STATIC ${MOON9_TEST_SRCS})

add_executable(test.sample                 test.sample.cpp)

target_link_libraries(test.sample          moon9-test-static)
