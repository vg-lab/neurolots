# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   neurolots
#   2015 (c) Universidad Rey Juan Carlos
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
project( neurolots VERSION 0.3.5 )
set( neurolots_VERSION_ABI 4 )

# Disable in source building
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
  message( FATAL "no in source building allowed." )
endif( )

list( APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common )

if( NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake )
  message( FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif( )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif( NOT CMAKE_BUILD_TYPE )

option( NEUROLOTS_OPTIONALS_AS_REQUIRED
  "Force optional dependencies as required" OFF )

if ( NEUROLOTS_OPTIONALS_AS_REQUIRED )
  set( NEUROLOTS_OPTS_FIND_ARGS "REQUIRED" )
else()
  set( NEUROLOTS_OPTS_FIND_ARGS "" )
endif()

include( GitExternal )

set( COMMON_PROJECT_DOMAIN vg-lab.es )
set( NEUROLOTS_DESCRIPTION "NeuroLOTs" )

option( NEUROLOTS_WITH_EXAMPLES "NEUROLOTS_WITH_EXAMPLES" ON )

include( Common )

common_find_package(Boost COMPONENTS unit_test_framework system filesystem
  REQUIRED SYSTEM)
common_find_package(OpenGL REQUIRED SYSTEM)
common_find_package(GLEW REQUIRED SYSTEM)
common_find_package(Eigen3 REQUIRED SYSTEM)
common_find_package(nsol REQUIRED)
common_find_package(ReTo REQUIRED)
common_find_package(GLUT SYSTEM)

list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES OpenGL GLEW Eigen3 nsol ReTo )

common_find_package_post( )

set( PROJECT_INCLUDE_NAME neurolots )

add_subdirectory( nlgeometry )
add_subdirectory( nlphysics )
add_subdirectory( nlgenerator )
add_subdirectory( nlrender )
add_subdirectory( examples )

install( FILES ${PROJECT_BINARY_DIR}/include/neurolots/defines.h
  DESTINATION include/neurolots )
install( FILES ${PROJECT_BINARY_DIR}/include/neurolots/defines${SYSTEM}.h
  DESTINATION include/neurolots )

set( DOCS README.md LICENSE.txt )
install( FILES ${DOCS} DESTINATION share/neurolots COMPONENT dev )

set( DOXYGEN_MAINPAGE_MD README.md )
set( DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md
  ${CMAKE_INSTALL_PREFIX}/include/nlgeometry
  ${CMAKE_INSTALL_PREFIX}/include/nlphysics
  ${CMAKE_INSTALL_PREFIX}/include/nlgenerator
  ${CMAKE_INSTALL_PREFIX}/include/nlrender )
include( DoxygenRule )

include( CPackConfig )
include( CTest )
