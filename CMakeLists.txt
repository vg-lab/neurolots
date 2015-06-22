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

include(GitExternal)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "1")
set(VERSION_ABI 1)

set(COMMON_PROJECT_DOMAIN gmrv.es)
set(NEUROLOTS_DESCRIPTION "NeuroLOTs")

option(NEUROLOTS_WITH_ZEQ "NEUROLOTS_WITH_ZEQ" ON)
option(NEUROLOTS_WITH_BBPSDK "NEUROLOTS_WITH_BBPSDK" ON)

include(Common)
include(FindPackages)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory( nlfem )
add_subdirectory( nlgenerator )
add_subdirectory( neurolots )

include(CPackConfig)
include(DoxygenRule)
