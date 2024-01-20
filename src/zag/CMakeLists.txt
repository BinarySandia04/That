INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
file(GLOB SOURCES
    */*.h
    */*.cpp
    */*.c
    *.h
    *.cpp
    *.c
)

set(CMAKE_CXX_STANDARD 17)

add_executable(zag ${SOURCES})

if (NOT WIN32)
    target_link_libraries(zag libdl.so)
endif ()
