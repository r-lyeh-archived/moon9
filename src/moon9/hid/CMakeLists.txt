cmake_minimum_required(VERSION 2.8)
project(MOON9_HID C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

include_directories("../../../deps/hid/manymouse")

file(GLOB MANYMOUSE_SRCS "../../../deps/hid/manymouse/*.c")
add_library(manymouse-static STATIC ${MANYMOUSE_SRCS})

file(GLOB MOON9_HID_SRCS *.cpp *.hpp *.inl)
file(GLOB MOON9_HID_SAMPLES *.sample.cpp)
file(GLOB MOON9_HID_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_HID_SRCS "" ${MOON9_HID_SAMPLES} ${MOON9_HID_TESTS})
## list(APPEND MOON9_HID_SRCS platform/w32/others.cpp)
add_library(moon9-hid-static STATIC ${MOON9_HID_SRCS})

add_executable(windows.keyboard.sample              windows.keyboard.sample.cpp)
add_executable(windows.mouse.sample                 windows.mouse.sample.cpp)
add_executable(windows.gamepad.sample               windows.gamepad.sample.cpp)
add_executable(windows.policy.sample                windows.policy.sample.cpp)

target_link_libraries(windows.keyboard.sample       moon9-hid-static manymouse-static)
target_link_libraries(windows.mouse.sample          moon9-hid-static manymouse-static)
target_link_libraries(windows.gamepad.sample        moon9-hid-static manymouse-static)
target_link_libraries(windows.policy.sample         moon9-hid-static manymouse-static)
