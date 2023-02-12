INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
file(GLOB SOURCES
    */*.h
    */*.cpp
    *.h
    *.cpp
)
add_executable(That ${SOURCES})