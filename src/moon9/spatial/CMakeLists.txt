cmake_minimum_required(VERSION 2.8)
project(MOON9_SPATIAL C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

include_directories( "../../../deps/spatial/glm" )
include_directories( "../../../deps/spatial/tokamak/include" )

## file(GLOB GLM_SRCS "../../../deps/spatial/glm/*.cpp")
## add_library(glm-static STATIC ${GLM_SRCS})

file(GLOB TOKAMAK_SRCS "../../../deps/spatial/tokamak/src/*.cpp")
add_library(tokamak-static STATIC ${TOKAMAK_SRCS})

file(GLOB MOON9_SPATIAL_SRCS *.cpp *.hpp *.inl)
file(GLOB MOON9_SPATIAL_SAMPLES *.sample.cpp)
file(GLOB MOON9_SPATIAL_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_SPATIAL_SRCS "" ${MOON9_SPATIAL_SAMPLES} ${MOON9_SPATIAL_TESTS})
## list(APPEND MOON9_SPATIAL_SRCS platform/w32/others.cpp)
add_library(moon9-spatial-static STATIC ${MOON9_SPATIAL_SRCS})

add_executable(bvh.sample                   bvh.sample.cpp)
add_executable(sphere.sample                sphere.sample.cpp)
add_executable(stats.sample                 stats/stats.sample.cpp)

target_link_libraries(bvh.sample            moon9-spatial-static)
target_link_libraries(sphere.sample         moon9-spatial-static)
target_link_libraries(stats.sample          moon9-spatial-static)
