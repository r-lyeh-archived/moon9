cmake_minimum_required(VERSION 2.8)
project(MOON9_TIME C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

## include_directories( "../../../deps/time" )

file(GLOB MOON9_TIME_SRCS *.cpp *.hpp *.inl)
file(GLOB MOON9_TIME_SAMPLES *.sample.cpp)
file(GLOB MOON9_TIME_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_TIME_SRCS "" ${MOON9_TIME_SAMPLES} ${MOON9_TIME_TESTS})
## list(APPEND MOON9_TIME_SRCS platform/w32/others.cpp)
add_library(moon9-time-static STATIC ${MOON9_TIME_SRCS})

add_executable(dt.sample                   dt/dt.sample.cpp)
add_executable(rtc.sample                  rtc.sample.cpp)
add_executable(timer.sample                timer.sample.cpp)

target_link_libraries(dt.sample            moon9-time-static)
target_link_libraries(rtc.sample           moon9-time-static)
target_link_libraries(timer.sample         moon9-time-static)
