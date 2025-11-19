# Install script for directory: /home/arans/Pro/That/ThatLib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/arans/Pro/That/builds/ThatLib/libThatLib.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThatLib/Ast" TYPE FILE FILES
    "/home/arans/Pro/That/ThatLib/Ast/ast.h"
    "/home/arans/Pro/That/ThatLib/Ast/error.h"
    "/home/arans/Pro/That/ThatLib/Ast/node.h"
    "/home/arans/Pro/That/ThatLib/Ast/parser.h"
    "/home/arans/Pro/That/ThatLib/Ast/scanner.h"
    "/home/arans/Pro/That/ThatLib/Ast/token.h"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThatLib/Formatter" TYPE FILE FILES "/home/arans/Pro/That/ThatLib/Formatter/formatter.h")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThatLib/Libs" TYPE FILE FILES
    "/home/arans/Pro/That/ThatLib/Libs/ctypes.h"
    "/home/arans/Pro/That/ThatLib/Libs/internal_package.h"
    "/home/arans/Pro/That/ThatLib/Libs/packages.h"
    "/home/arans/Pro/That/ThatLib/Libs/types.h"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThatLib/Logs" TYPE FILE FILES "/home/arans/Pro/That/ThatLib/Logs/logs.h")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThatLib/Objects" TYPE FILE FILES
    "/home/arans/Pro/That/ThatLib/Objects/container.h"
    "/home/arans/Pro/That/ThatLib/Objects/function.h"
    "/home/arans/Pro/That/ThatLib/Objects/object.h"
    "/home/arans/Pro/That/ThatLib/Objects/operation.h"
    "/home/arans/Pro/That/ThatLib/Objects/prototype.h"
    "/home/arans/Pro/That/ThatLib/Objects/scope.h"
    "/home/arans/Pro/That/ThatLib/Objects/type.h"
    "/home/arans/Pro/That/ThatLib/Objects/variable.h"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThatLib/Transpiler" TYPE FILE FILES "/home/arans/Pro/That/ThatLib/Transpiler/transpiler.h")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThatLib/Utils" TYPE FILE FILES
    "/home/arans/Pro/That/ThatLib/Utils/system.h"
    "/home/arans/Pro/That/ThatLib/Utils/thatpath.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/arans/Pro/That/builds/ThatLib/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
