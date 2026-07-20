cmake_minimum_required(VERSION 3.25)

project(neurolots
  VERSION 0.3.5
  LANGUAGES CXX
  DESCRIPTION "NeuroLOTs"
)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

# Prevent in-source builds
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# Include custom modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Options
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_DOCS "Build documentation" ON)
option(NEUROLOTS_WITH_EXAMPLES "Build examples" ON)
# option(NEUROLOTS_WITH_TESTS "Build tests" ON)
option(NEUROLOTS_OPTIONALS_AS_REQUIRED "Force optional dependencies as required" OFF)

# Dependencies
find_package(Boost CONFIG REQUIRED COMPONENTS unit_test_framework)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)
# find_package(nsol REQUIRED)
# find_package(ReTo REQUIRED)

include(FetchContent)
FetchContent_Declare(
    nsol
    GIT_REPOSITORY https://github.com/vg-lab/nsol.git
    GIT_SHALLOW ON
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(nsol)

FetchContent_Declare(
    ReTo
    GIT_REPOSITORY https://github.com/vg-lab/ReTo.git
    GIT_SHALLOW ON
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(ReTo)
list(APPEND CMAKE_MODULE_PATH "${ReTo_BINARY_DIR}/modules")

# Optional dependencies
set(NEUROLOTS_OPTS_FIND_ARGS "")
if(NEUROLOTS_OPTIONALS_AS_REQUIRED)
  set(NEUROLOTS_OPTS_FIND_ARGS REQUIRED)
endif()

find_package(GLUT ${NEUROLOTS_OPTS_FIND_ARGS} QUIET)

# Definitions
option(NEUROLOTS_USE_GLUT "" ${GLUT_FOUND})

# Add subdirectories
add_subdirectory(nlgeometry)
add_subdirectory(nlphysics)
add_subdirectory(nlgenerator)
add_subdirectory(nlrender)

# Don't build examples when others include the library
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND NEUROLOTS_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

# Define installation directory variables
include(GNUInstallDirs)

# Include the CMake module that provides helpers for creating package config files
include(CMakePackageConfigHelpers)

# Generate the project-specific cmake config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/project-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}
)

# Generate the project-specific cmake config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Generate the project-specific cmake config version file
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}
)

# Install repo data
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
  "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}
)

# Generate documentation
find_package(Doxygen)
if(DOXYGEN_FOUND AND BUILD_DOCS)
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs_doxygen)
  set(DOXYFILE_IN ${PROJECT_SOURCE_DIR}/cmake/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  doxygen_add_docs(
    ${PROJECT_NAME}_doxygen
    CONFIG_FILE ${DOXYFILE_OUT}
  )

  install(DIRECTORY
    ${DOXYGEN_OUTPUT_DIR}/html
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT ${PROJECT_NAME}_documentation
    OPTIONAL
  )
endif()

# Packaging
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
