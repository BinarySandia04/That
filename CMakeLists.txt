cmake_minimum_required(VERSION 3.24.1)
project(Glass VERSION 0.1)

find_package(Git)
FILE(WRITE ${CMAKE_BINARY_DIR}/version.h.in
"\#define VERSION \"@VERSION@\"\n"
)
FILE(WRITE ${CMAKE_BINARY_DIR}/version.cmake
"EXECUTE_PROCESS(
     COMMAND ${GIT_EXECUTABLE} --version
     OUTPUT_VARIABLE VERSION
     OUTPUT_STRIP_TRAILING_WHITESPACE
 )
 CONFIGURE_FILE(\${SRC} \${DST} @ONLY)
")
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
add_executable(Glass glass.cpp)
ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_BINARY_DIR}/version.h.in
                     -D DST=${CMAKE_BINARY_DIR}/version.h
                     -P ${CMAKE_BINARY_DIR}/version.cmake
)
ADD_DEPENDENCIES(Glass version)