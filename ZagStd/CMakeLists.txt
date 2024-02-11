set(PROJECT_NAME ZagStd)

# We set files
file(GLOB src "*.h" "*.cpp")
source_group("src" FILES ${src})

set(ALL_FILES ${src} ${Ast} ${libs})
set(ROOT_NAMESPACE ${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME} libdl.so)
endif ()

target_sources(${PROJECT_NAME} 
    PRIVATE ${ALL_FILES}
    PUBLIC FILE_SET HEADERS 
    BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES ${PUBLIC_HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "zagstd")
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
