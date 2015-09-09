# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   neurolots
#   2014 (c) Universidad Rey Juan Carlos
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

project(neurolots)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)


include(GitExternal)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "1")
set(VERSION_ABI 1)

include(Common)

set(COMMON_PROJECT_DOMAIN gmrv.es)
set(NEUROLOTS_DESCRIPTION "NeuroLOTs")

option(NEUROLOTS_WITH_ZEQ "NEUROLOTS_WITH_ZEQ" ON)
option(NEUROLOTS_WITH_GMRVZEQ "NEUROLOTS_WITH_GMRVZEQ" ON)
option(NEUROLOTS_WITH_DEFLECT "NEUROLOTS_WITH_DEFLECT" ON)

common_package(OpenGL REQUIRED SYSTEM)
common_package(GLEW REQUIRED SYSTEM)
common_package(GLUT REQUIRED SYSTEM)
common_package(Eigen3 REQUIRED SYSTEM)
common_package(nsol REQUIRED)
list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES OpenGL GLEW GLUT Eigen3 nsol)

if(NEUROLOTS_WITH_ZEQ)
  common_package(zeq)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES zeq)
  if ( ZEQ_FOUND )
    common_package(Threads REQUIRED)
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Threads)
  endif( )
endif( )

if(NEUROLOTS_WITH_GMRVZEQ)
  common_package(gmrvzeq)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES gmrvzeq)
endif( )

if(NEUROLOTS_WITH_DEFLECT)
  common_package(Deflect)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Deflect)
endif( )

common_package_post()

set(PROJECT_INCLUDE_NAME neurolots)

add_subdirectory( nlfem )
add_subdirectory( nlgenerator )
add_subdirectory( neurolots )

include(CPackConfig)
include(DoxygenRule)
