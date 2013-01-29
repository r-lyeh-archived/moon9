cmake_minimum_required(VERSION 2.8)

project(ROOT C CXX)

#cmake_policy(SET CMP0014 OLD) # do not warn about empty directories

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

set( OPENAL_INCLUDE_DIR "${ROOT_SOURCE_DIR}/openal/include")
#set( OPUS_INCLUDE_DIR "${ROOT_SOURCE_DIR}/opus/include")
set( VORBIS_INCLUDE_DIR "${ROOT_SOURCE_DIR}/stb_vorbis")

if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
else()
endif()

#add_subdirectory(freealut)
add_subdirectory(openal)
#add_subdirectory(opus)
#add_subdirectory(proteaaudio)
#add_subdirectory(rtaudio)
add_subdirectory(stb_vorbis)
