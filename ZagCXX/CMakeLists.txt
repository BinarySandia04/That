include(CMakePrintHelpers)

set(PROJECT_NAME ZagCXX)

file(GLOB src "*.h" "*.cpp")
source_group("src" FILES ${src})
file(GLOB_RECURSE Transpiler "Transpiler/*.h" "Transpiler/*.cpp")
source_group("Transpiler" FILES ${Transpiler})
file(GLOB_RECURSE Formatter "Formatter/*.h" "Formatter/*.cpp")
source_group("Formatter" FILES ${Formatter})

set(ALL_FILES ${src} ${Transpiler} ${Formatter})
set(ROOT_NAMESPACE ZagCXX)

# Odio CMAKE
FUNCTION(ADD_RESOURCES out_var)
  SET(result)
  FOREACH(in_f ${ARGN})
    FILE(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/res/${in_f})
    SET(out_f "${PROJECT_BINARY_DIR}/${in_f}.o")
    cmake_path(GET out_f PARENT_PATH dirPath)
    ADD_CUSTOM_COMMAND(OUTPUT ${out_f}
        COMMAND mkdir -p ${dirPath} && pushd ${PROJECT_NAME} > /dev/null && pushd res > /dev/null && ld -r -b binary -o ${out_f} ${in_f} && popd > /dev/null && popd > /dev/null
      DEPENDS res/${in_f}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Building resource object ${out_f}"
      VERBATIM
      )
    LIST(APPEND result ${out_f})
  ENDFOREACH()
  SET(${out_var} "${result}" PARENT_SCOPE)
ENDFUNCTION()

# Glob resource files
file(GLOB_RECURSE resourceFiles RELATIVE "${CMAKE_SOURCE_DIR}/ZagCXX/res" "res/*")

add_resources(resources ${resourceFiles})

add_executable(${PROJECT_NAME} ${ALL_FILES} ${resources})

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
