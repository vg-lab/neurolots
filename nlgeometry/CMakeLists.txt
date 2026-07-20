# Options
option(NLGEOMETRY_WITH_TESTS "NLGEOMETRY_WITH_TESTS" ON)

# Set the target name
set(CURRENT_TARGET_NAME nlgeometry)
string(TOLOWER ${CURRENT_TARGET_NAME} CURRENT_TARGET_NAME_LOWER)
string(TOUPPER ${CURRENT_TARGET_NAME} CURRENT_TARGET_NAME_UPPER)

# Define public headers
set(NLGEOMETRY_PUBLIC_HEADERS
  AxisAlignedBoundingBox.h
  Facet.h
  Mesh.h
  OrbitalVertex.h
  Reader/ObjReaderTemplated.h
  SectionQuad.h
  SpatialHashTable.h
  Vertex.h
  Writer/ObjWriter.h
  Writer/OffWriter.h
)

# Create the includes strings for the unified header
set(HEADER_INCLUDES "")
foreach(header ${NLGEOMETRY_PUBLIC_HEADERS})
  string(APPEND HEADER_INCLUDES "#include <${CURRENT_TARGET_NAME_LOWER}/${header}>\n")
endforeach()

# Generate the unified header
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/unified_header-component.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}.h"
)

# Get the current Git commit for the version header
find_package(Git)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} --no-pager log -n 1 HEAD "--pretty=format:%h"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    RESULT_VARIABLE git_result
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(git_result EQUAL 0)
    message(STATUS "Last ${PROJECT_NAME}::${CURRENT_TARGET_NAME} Git commit: ${GIT_COMMIT_HASH}")
  endif()
endif()

# Generate the version header
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/version-component.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/version.h"
)

# Define source files
set(NLGEOMETRY_SOURCES
  AxisAlignedBoundingBox.cpp
  Facet.cpp
  Mesh.cpp
  OrbitalVertex.cpp
  SectionQuad.cpp
  SpatialHashTable.cpp
  Vertex.cpp
  Writer/ObjWriter.cpp
  Writer/OffWriter.cpp
)

# Create the target
add_library(${CURRENT_TARGET_NAME})
add_library(${PROJECT_NAME}::${CURRENT_TARGET_NAME} ALIAS ${CURRENT_TARGET_NAME})

set_target_properties(${CURRENT_TARGET_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Generate export header
include(GenerateExportHeader)
generate_export_header(${CURRENT_TARGET_NAME})

# Add sources to the target
target_sources(${CURRENT_TARGET_NAME}
  PRIVATE
    ${NLGEOMETRY_SOURCES}
  PUBLIC
    FILE_SET public_headers
    TYPE HEADERS
    BASE_DIRS
      ${PROJECT_SOURCE_DIR}
    FILES
      ${NLGEOMETRY_PUBLIC_HEADERS}
)

# Add generated sources to the target
target_sources(${CURRENT_TARGET_NAME}
  PUBLIC
    FILE_SET public_headers_generated
    TYPE HEADERS
    BASE_DIRS
      ${PROJECT_BINARY_DIR}
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}.h
      ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}_export.h
      ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

# Add compile definitions
target_compile_definitions(${CURRENT_TARGET_NAME}
  PUBLIC
  PRIVATE
)

# Link libraries
target_link_libraries(${CURRENT_TARGET_NAME}
  PUBLIC
    OpenGL::GL
    GLEW::GLEW
    Eigen3::Eigen
)

# Don't build tests when others include the library
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND NLGEOMETRY_WITH_TESTS)
  include(CTest)
  add_subdirectory(tests)
endif()

# Make cache variables for install destinations
include(GNUInstallDirs)

# Create an export
install(TARGETS ${CURRENT_TARGET_NAME}
  EXPORT ${CURRENT_TARGET_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_TARGET_NAME_LOWER}
  FILE_SET public_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILE_SET public_headers_generated DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the project targets file
install(EXPORT ${CURRENT_TARGET_NAME}Targets
  FILE ${CURRENT_TARGET_NAME_LOWER}-targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}/${CURRENT_TARGET_NAME_LOWER}
  COMPONENT ${CURRENT_TARGET_NAME}
)

# Include the CMake module that provides helpers for creating package config files
include(CMakePackageConfigHelpers)

# Generate the project-specific cmake config file
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/project-component-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}/${CURRENT_TARGET_NAME_LOWER}
)

# Generate the project-specific cmake config version file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Install the cmake config files
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}/${CURRENT_TARGET_NAME_LOWER}
)

# Allow the configured project-config.cmake in the build directory to be used by
# other projects, without needing it to be installed
export(EXPORT ${CURRENT_TARGET_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  FILE ${CURRENT_TARGET_NAME_LOWER}-targets.cmake
)