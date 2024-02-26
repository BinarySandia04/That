cmake_minimum_required(VERSION 3.16)
project(Zag VERSION 0.1)

SET(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_CXX_LIBS "-O3")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_LIBS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

find_package(Git)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# add_subdirectory(ZagIR)
# add_subdirectory(ZagCXX)
add_subdirectory(Zag)
# add_subdirectory(ZagStd)
