cmake_minimum_required(VERSION 2.8)
project(MOON9_MATH C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

## include_directories( "../../../deps/test" )

file(GLOB MOON9_MATH_SRCS *.cpp *.hpp *.inl)
file(GLOB MOON9_MATH_SAMPLES *.sample.cpp)
file(GLOB MOON9_MATH_MATHS *.test.cpp)
list(REMOVE_ITEM MOON9_MATH_SRCS "" ${MOON9_MATH_SAMPLES} ${MOON9_MATH_MATHS})
## list(APPEND MOON9_MATH_SRCS platform/w32/others.cpp)
add_library(moon9-math-static STATIC ${MOON9_MATH_SRCS})

add_executable(burg.sample                 burg/burg.sample.cpp)
add_executable(rand.sample                 rand/rand.sample.cpp)
add_executable(stats.sample                stats/stats.sample.cpp)

target_link_libraries(burg.sample          moon9-math-static)
target_link_libraries(rand.sample          moon9-math-static)
target_link_libraries(stats.sample         moon9-math-static)
