cmake_minimum_required (VERSION 2.6)
project (Examples)
include_directories("${PROJECT_SOURCE_DIR}/../../source")
include_directories("/prj/libs/boost") # optional for non c99 compilers (like msvc2008)

IF( MSVC )
set(CMAKE_CXX_FLAGS "/Zi /EHsc") 
ENDIF()

add_executable(main main.cpp arbprogram.cpp image.cpp md5model.cpp shader.cpp texture.cpp ../../source/render9/[library][freeglut].c) 
