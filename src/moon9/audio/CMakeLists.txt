cmake_minimum_required(VERSION 2.8)
project(MOON9_AUDIO C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

# <moon9>
include_directories( "../../../src" )

#include_directories( "../../../deps/audio/freealut/include" )
#include_directories( "../../../deps/audio/libogg" )
#include_directories( "../../../deps/audio/libvorbis" )
include_directories( "../../../deps/audio/openal/include" )
#include_directories( "../../../deps/audio/opus/include" )
include_directories( "../../../deps/audio/stb_vorbis" )

file(GLOB MOON9_AUDIO_SRCS *.cpp *.hpp *.inl)
file(GLOB MOON9_AUDIO_SAMPLES *.sample.cpp)
file(GLOB MOON9_AUDIO_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_AUDIO_SRCS "" ${MOON9_AUDIO_SAMPLES} ${MOON9_AUDIO_TESTS})
## list(APPEND MOON9_AUDIO_SRCS platform/w32/others.cpp)
add_library(moon9-audio-static STATIC ${MOON9_AUDIO_SRCS})

add_executable(device.sample              device.sample.cpp)
target_link_libraries(device.sample       moon9-audio-static openal stb_vorbis)

#add_executable(effects.sample             effects.sample.cpp)
#target_link_libraries(effects.sample      moon9-audio-static openal stb_vorbis)
