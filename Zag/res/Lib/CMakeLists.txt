# ---------------------------------------------------
# Auto generated CMakeLists.txt file from Zag
# Do not touch unless you know what you are doing
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.16)
project(internal VERSION 0.1)

SET(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_CXX_LIBS "-O2")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_LIBS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# We set files
file(GLOB src "src/*.h" "src/*.cpp")
source_group("src" FILES ${src})

set(ALL_FILES ${src}) 
set(ROOT_NAMESPACE Z_Internal)

add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Public headers accessible when using the lib
file(GLOB PUBLIC_HEADERS "*.h") 

target_sources(${PROJECT_NAME} 
    PRIVATE ${ALL_FILES}
    PUBLIC FILE_SET HEADERS 
    BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES ${PUBLIC_HEADERS})

# install(TARGETS ${PROJECT_NAME} FILE_SET HEADERS)

