INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
file(GLOB SOURCES
    *.h
    *.cpp
)
add_executable(Glass ${SOURCES})