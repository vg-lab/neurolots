# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   neurolots
#   2014 (c) Universidad Rey Juan Carlos
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(neurolots VERSION 0.0.2)
set(neurolots_VERSION_ABI 1)

# Disable in source building
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
  message( FATAL "no in source building allowed." )
endif()

list( APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common )

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif()

option( NEUROLOTS_OPTIONALS_AS_REQUIRED
  "Force optional dependencies as required" OFF )

if ( NEUROLOTS_OPTIONALS_AS_REQUIRED )
  set( NEUROLOTS_OPTS_FIND_ARGS "REQUIRED" )
else()
  set( NEUROLOTS_OPTS_FIND_ARGS "" )
endif()


include(GitExternal)

set(COMMON_PROJECT_DOMAIN gmrv.es)
set(NEUROLOTS_DESCRIPTION "NeuroLOTs")

include(Common)

common_find_package(Boost COMPONENTS unit_test_framework)
common_find_package(Qt5Core 5.4 SYSTEM )
common_find_package(Qt5Widgets 5.4 SYSTEM )
common_find_package(Qt5OpenGL 5.4 SYSTEM )
common_find_package(OpenGL REQUIRED SYSTEM)
common_find_package(GLEW REQUIRED SYSTEM)
common_find_package(Eigen3 REQUIRED SYSTEM)
common_find_package(nsol REQUIRED)
common_find_package(ReTo REQUIRED)
common_find_package(GLUT)
# common_find_package(Deflect ${NEUROLOTS_OPTS_FIND_ARGS})
common_find_package(Lexis ${NEUROLOTS_OPTS_FIND_ARGS})
common_find_package(ZeroEQ ${NEUROLOTS_OPTS_FIND_ARGS})
common_find_package(gmrvlex ${NEUROLOTS_OPTS_FIND_ARGS})

list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Qt5Core Qt5Widget Qt5OpenGL
  OpenGL GLEW GLUT Eigen3 nsol ReTo)


if (DEFLECT_FOUND)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Deflect)
endif( )

if (LEXIS_FOUND)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Lexis)
endif( )

if ( ZEROEQ_FOUND )
  common_find_package(Threads REQUIRED)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES ZeroEQ)
endif( )

if ( GMRVLEX_FOUND )
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES gmrvlex)
endif( )

common_find_package_post()

set(PROJECT_INCLUDE_NAME neurolots)

add_subdirectory( nlgeometry )
add_subdirectory( nlfem )
add_subdirectory( nlgenerator )
add_subdirectory( nlrender )
add_subdirectory( examples )

if ( Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5OpenGL_FOUND )
  add_subdirectory( neurotessmesh )
else( )
  message(WARNING "neurotessmesh build is skipped because it needs Qt 5.4 or above." )
endif( )


install(FILES ${PROJECT_BINARY_DIR}/include/neurolots/defines.h
  DESTINATION include/neurolots )
install(FILES ${PROJECT_BINARY_DIR}/include/neurolots/defines${SYSTEM}.h
  DESTINATION include/neurolots )

include(CPackConfig)
include(DoxygenRule)
