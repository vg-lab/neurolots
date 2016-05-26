# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   neurolots
#   2014 (c) Universidad Rey Juan Carlos
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(neurolots VERSION 0.0.1)
set(neurolots_VERSION_ABI 0)


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

include(GitExternal)

set(COMMON_PROJECT_DOMAIN gmrv.es)
set(NEUROLOTS_DESCRIPTION "NeuroLOTs")

option(NEUROLOTS_WITH_ZEQ "NEUROLOTS_WITH_ZEQ" ON)
option(NEUROLOTS_WITH_GMRVZEQ "NEUROLOTS_WITH_GMRVZEQ" ON)
option(NEUROLOTS_WITH_DEFLECT "NEUROLOTS_WITH_DEFLECT" ON)

include(Common)

common_find_package(OpenGL REQUIRED SYSTEM)
common_find_package(GLEW REQUIRED SYSTEM)
common_find_package(Eigen3 REQUIRED SYSTEM)
common_find_package(nsol REQUIRED)
list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES OpenGL GLEW GLUT Eigen3 nsol)

if(NEUROLOTS_WITH_ZEQ)
  common_find_package(ZeroEQ)
  if ( ZEROEQ_FOUND )
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES ZeroEQ)
    common_find_package(Threads REQUIRED)
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Threads)
  endif( )
endif( )

if(NEUROLOTS_WITH_GMRVZEQ)
  common_find_package(gmrvzeq)
  if ( GMRVZEQ_FOUND )
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES gmrvzeq)
  endif( )
endif( )

if(NEUROLOTS_WITH_DEFLECT)
  common_find_package(Deflect SYSTEM)
  if (DEFLECT_FOUND)
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Deflect)
  endif( )
endif( )

common_find_package(Qt5Core 5.4 SYSTEM )
common_find_package(Qt5Widgets 5.4 SYSTEM )
common_find_package(Qt5OpenGL 5.4 SYSTEM )

if (QT5CORE_FOUND)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Qt5Core)
endif( )

if (QT5WIDGETS_FOUND)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Qt5Widgets)
endif( )

if (QT5OPENGL_FOUND)
  list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Qt5OpenGL)
endif( )

common_find_package_post()

set(PROJECT_INCLUDE_NAME neurolots)

add_subdirectory( nlgeometry )
add_subdirectory( nlfem )
add_subdirectory( nlgenerator )
add_subdirectory( nlrender )
add_subdirectory( neurotessmesh )

install(FILES ${PROJECT_BINARY_DIR}/include/neurolots/defines.h
  DESTINATION include/neurolots )
install(FILES ${PROJECT_BINARY_DIR}/include/neurolots/defines${SYSTEM}.h
  DESTINATION include/neurolots )

include(CPackConfig)
include(DoxygenRule)
