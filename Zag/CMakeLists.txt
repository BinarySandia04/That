set(PROJECT_NAME Zag)

file(GLOB src "*.h" "*.cpp")
source_group("src" FILES ${src})

set(ALL_FILES ${src})
set(ROOT_NAMESPACE Zag)

add_executable(${PROJECT_NAME} ${ALL_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../vendors") 

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "zag")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
