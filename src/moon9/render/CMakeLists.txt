cmake_minimum_required(VERSION 2.8)
project(MOON9_RENDER C CXX)

if(MSVC)
  add_definitions(/EHsc)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/wd4244) # dear MSVC, im fully aware and confident about my type conversions. stop hammering me. kthx :s
endif(MSVC)

include_directories("../../../deps/render/glew/include")
include_directories("../../../deps/render/glfw/include")
include_directories("../../../deps/render/glfw/lib")
include_directories("../../../deps/render/glfw/lib/win32")
include_directories("../../../deps/render/freeglut/include")
include_directories("../../../deps/render/spark/include")
include_directories("../../../deps/render/")

include_directories("../../../deps/spatial/glm")

include_directories("../../../src")

file(GLOB STB_SRCS "../../../deps/render/stb/*.c")
add_library(stb-static STATIC ${STB_SRCS})

file(GLOB FREEGLUT_SRCS "../../../deps/render/freeglut/*.c")
add_library(freeglut-static STATIC ${FREEGLUT_SRCS})

file(GLOB GLEW_SRCS "../../../deps/render/glew/src/*.c")
add_library(glew-static STATIC ${GLEW_SRCS})

file(GLOB GLFW_SRCS "../../../deps/render/glfw/lib/*.c" "../../../deps/render/glfw/lib/win32/*.c")
add_library(glfw-static STATIC ${GLFW_SRCS})

file(GLOB SPARK_SRCS "../../../deps/render/spark/src/SPK_All.cpp" "../../../deps/render/spark/src/SPK_GL_All.cpp")
add_library(spark-static STATIC ${SPARK_SRCS})

file(GLOB MOON9_RENDER_SRCS *.cpp *.hpp)
file(GLOB MOON9_RENDER_SAMPLES *.sample.cpp)
file(GLOB MOON9_RENDER_TESTS *.test.cpp)
list(REMOVE_ITEM MOON9_RENDER_SRCS "" ${MOON9_RENDER_SAMPLES} ${MOON9_RENDER_TESTS})
## list(APPEND MOON9_RENDER_SRCS platform/w32/others.cpp)
add_library(moon9-render-static STATIC ${MOON9_RENDER_SRCS})

##add_executable(frustum.sample                 camera/frustum/frustum.sample.cpp)
add_executable(float16.sample                   float16/float16.sample.cpp)
add_executable(image.sample                     texture/image/image.sample.cpp)
add_executable(window.sample                    window.sample.cpp ../time/wink.cpp ../spatial/utils.cpp)

##target_link_libraries(frustum.sample          moon9-render-static)
target_link_libraries(float16.sample            moon9-render-static)
target_link_libraries(image.sample              moon9-render-static stb-static)
target_link_libraries(window.sample             moon9-render-static freeglut-static glew-static glfw-static)
