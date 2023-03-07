cmake_minimum_required(VERSION 3.16)
project(That VERSION 0.1)

set(CMAKE_CXX_LIBS "-lgmp -lmpfr")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_LIBS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

find_package(Git)

add_subdirectory(src)

# DEBUG QUITAR SI NO SE QUIERE DEBUG
# add_compile_definitions(DEBUG)

# GIT I COSAS
FILE(WRITE ${CMAKE_BINARY_DIR}/version.h.in
"\#define VERSION \"@VERSION@\"\n"
)
FILE(WRITE ${CMAKE_BINARY_DIR}/version.cmake
"EXECUTE_PROCESS(
     COMMAND ${GIT_EXECUTABLE} describe
     OUTPUT_VARIABLE VERSION
     OUTPUT_STRIP_TRAILING_WHITESPACE
 )
 CONFIGURE_FILE(\${SRC} \${DST} @ONLY)
")
ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_BINARY_DIR}/version.h.in
                     -D DST=${CMAKE_BINARY_DIR}/version-g.h
                     -P ${CMAKE_BINARY_DIR}/version.cmake
)
ADD_DEPENDENCIES(That version)