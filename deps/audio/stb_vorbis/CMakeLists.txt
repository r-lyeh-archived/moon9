cmake_minimum_required(VERSION 2.8)
project(stb_vorbis C)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

include_directories(./)
add_library(stb_vorbis STATIC stb_vorbis.c)
