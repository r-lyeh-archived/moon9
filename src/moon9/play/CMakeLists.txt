cmake_minimum_required(VERSION 2.8)
project(MOON9_PLAY C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

# <moon9>
include_directories( "../../../src" )

include_directories( "../../../deps/play" )
include_directories( "../../../deps/play/kptree" )
include_directories( "../../../deps/play/msgpack/src" )

file(GLOB MSGPACK_SRCS "../../../deps/play/msgpack/src/*.cpp")
add_library(msgpack-static STATIC ${MSGPACK_SRCS})

file(GLOB MOON9_PLAY_SRCS *.cpp *.hpp *.inl)
file(GLOB MOON9_PLAY_SAMPLES *.sample.cpp)
file(GLOB MOON9_PLAY_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_PLAY_SRCS "" ${MOON9_PLAY_SAMPLES} ${MOON9_PLAY_TESTS})
## list(APPEND MOON9_PLAY_SRCS platform/w32/others.cpp)
add_library(moon9-play-static STATIC ${MOON9_PLAY_SRCS})

add_executable(callback.sample              callback/callback.sample.cpp)
add_executable(event.sample                 event/event.sample.cpp)
add_executable(factory.sample               factory/factory.sample.cpp)
add_executable(fsm.sample                   fsm/fsm.sample.cpp)
add_executable(obj.first.sample             obj/obj.first.sample.cpp)
add_executable(obj.second.sample            obj/obj.second.sample.cpp)
add_executable(obj.third.sample             obj/obj.third.sample.cpp)
add_executable(obj.fourth.sample            obj/obj.fourth.sample.cpp ../io/netstring.cpp)
add_executable(tree.sample                  tree/tree.sample.cpp)

target_link_libraries(callback.sample       moon9-play-static)
target_link_libraries(event.sample          moon9-play-static)
target_link_libraries(factory.sample        moon9-play-static)
target_link_libraries(fsm.sample            moon9-play-static)
target_link_libraries(obj.first.sample      moon9-play-static)
target_link_libraries(obj.second.sample     moon9-play-static)
target_link_libraries(obj.third.sample      moon9-play-static)
target_link_libraries(obj.fourth.sample     moon9-play-static msgpack-static)
target_link_libraries(tree.sample           moon9-play-static)
