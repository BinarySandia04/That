include(CMakePrintHelpers)

set(PROJECT_NAME ZagIR)

# We set files
file(GLOB src "*.h" "*.cpp")
source_group("src" FILES ${src})
file(GLOB_RECURSE Ast "Ast/*.h" "Ast/*.cpp")
source_group("Ast" FILES ${Ast})
file(GLOB_RECURSE Libs "Libs/*.h" "Libs/*.cpp")
source_group("Libs" FILES ${Libs})

set(ALL_FILES ${src} ${Ast} ${Libs})
set(ROOT_NAMESPACE ZagIR)

add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../vendors") 


if (NOT WIN32)
    target_link_libraries(ZagIR libdl.so)
endif ()

cmake_print_variables(PROJECT_SOURCE_DIR)

# Public headers accessible when using the lib
file(GLOB PUBLIC_HEADERS "*.h" "Ast/*.h" "Libs/*.h")

target_sources(${PROJECT_NAME} 
    PRIVATE ${ALL_FILES}
    PUBLIC FILE_SET HEADERS 
    BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES ${PUBLIC_HEADERS})

install(TARGETS ${PROJECT_NAME} FILE_SET HEADERS)

