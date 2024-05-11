include(CMakePrintHelpers)

set(PROJECT_NAME ThatLib)

# We set files
file(GLOB src "*.h" "*.cpp")
source_group("src" FILES ${src})
file(GLOB_RECURSE Ast "Ast/*.h" "Ast/*.cpp")
source_group("Ast" FILES ${Ast})
file(GLOB_RECURSE Libs "Libs/*.h" "Libs/*.cpp")
source_group("Libs" FILES ${Libs})
file(GLOB_RECURSE Logs "Logs/*.h" "Logs/*.cpp")
source_group("Logs" FILES ${Logs})
file(GLOB_RECURSE Utils "Utils/*.h" "Utils/*.cpp")
source_group("Utils" FILES ${Utils})
file(GLOB_RECURSE Objects "Objects/*.h" "Objects/*.cpp")
source_group("Objects" FILES ${Objects})
file(GLOB_RECURSE Transpiler "Transpiler/*.h" "Transpiler/*.cpp")
source_group("Transpiler" FILES ${Transpiler})
file(GLOB_RECURSE Formatter "Formatter/*.h" "Formatter/*.cpp")
source_group("Formatter" FILES ${Formatter})

set(ALL_FILES ${src} ${Ast} ${Libs} ${Logs} ${Utils} ${Objects} ${Transpiler} ${Formatter})
set(ROOT_NAMESPACE ThatLib)

add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external/argh/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external/termcolor/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external/tomlplusplus/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external") 
# target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external/cppparser/pub")


if (NOT WIN32)
    target_link_libraries(ThatLib libdl.so)
endif ()

cmake_print_variables(PROJECT_SOURCE_DIR)

# Public headers accessible when using the lib
file(GLOB PUBLIC_HEADERS "*.h" "Ast/*.h" "Libs/*.h" "Logs/*.h" "Utils/*.h" "Objects/*.h" "Transpiler/*.h" "Formatter/*.h")

target_sources(${PROJECT_NAME} 
    PRIVATE ${ALL_FILES}
    PUBLIC FILE_SET HEADERS 
    BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES ${PUBLIC_HEADERS})

install(TARGETS ${PROJECT_NAME} FILE_SET HEADERS)

