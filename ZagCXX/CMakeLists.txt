set(PROJECT_NAME ZagCXX)

file(GLOB src "src/*.h" "src/*.cpp")
source_group("src" FILES ${src})

set(ALL_FILES ${src})
set(ROOT_NAMESPACE ZagCXX)

add_executable(${PROJECT_NAME} ${ALL_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
  ZagIR
)
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ZagIR/src"
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../vendors") 

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "zagc")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
