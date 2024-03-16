include(CMakePrintHelpers)

set(PROJECT_NAME ZagCXX)

file(GLOB src "*.h" "*.cpp")
source_group("src" FILES ${src})
file(GLOB_RECURSE Transpiler "Transpiler/*.h" "Transpiler/*.cpp")
source_group("Transpiler" FILES ${Transpiler})

set(ALL_FILES ${src} ${Transpiler})

set(ROOT_NAMESPACE ZagCXX)

add_executable(${PROJECT_NAME} ${ALL_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
  ZagIR
)
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ZagIR/src"
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external") 
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external/argh/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external/termcolor/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../external/tomlplusplus/include")
#
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "zagc")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
