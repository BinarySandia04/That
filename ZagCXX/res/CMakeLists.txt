cmake_minimum_required(VERSION 3.16)
project({1} VERSION 0.1)

SET(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_CXX_LIBS "-O2")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${{CMAKE_C_FLAGS}}")
set(CMAKE_CXX_FLAGS "${{CMAKE_CXX_FLAGS}} ${{CMAKE_CXX_LIBS}}")
set(CMAKE_EXE_LINKER_FLAGS "${{CMAKE_EXE_LINKER_FLAGS}}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMakePrintHelpers)

file(GLOB_RECURSE src "src/*.h" "src/*.cpp" "main.cpp")
source_group("src" FILES ${{src}})

set(ALL_FILES ${{src}})

add_executable(${{PROJECT_NAME}} ${{ALL_FILES}})
{2}
