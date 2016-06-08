if( QT5CORE_FOUND )

set(Name Neurotessmesh)
set(NAME NEUROTESSMESH)
set(namespace neurotessmesh)
set(NAMESPACE NEUROTESSMESH)
set(INCLUDE_NAME neurotessmesh)

set(PROJECT_VERSION_ABI ${${PROJECT_NAME}_VERSION_ABI})
# set(VERSION_ABI 0)

configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/api.h
  ${PROJECT_BINARY_DIR}/include/neurotessmesh/api.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.h
  ${PROJECT_BINARY_DIR}/include/neurotessmesh/version.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.cpp
  ${PROJECT_BINARY_DIR}/src/neurotessmesh/version.cpp @ONLY)

set(NEUROTESSMESH_LINK_LIBRARIES nsol
  Qt5::Core Qt5::Concurrent Qt5::Network Qt5::Quick Qt5::Widgets)

if (ZEROEQ_FOUND)
  list(APPEND NEUROTESSMESH_LINK_LIBRARIES ZeroEQ)
endif()

if (LEXIS_FOUND)
  list(APPEND NEUROTESSMESH_LINK_LIBRARIES Lexis)
endif()

if (GMRVLEX_FOUND)
  list(APPEND NEUROTESSMESH_LINK_LIBRARIES gmrvlex)
endif()

if (DEFLECT_FOUND)
  list(APPEND NEUROTESSMESH_LINK_LIBRARIES Deflect)
endif()

if (FIRES_FOUND)
  list(APPEND NEUROTESSMESH_LINK_LIBRARIES fires)
endif()

set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)

set(NEUROTESSMESH_SOURCES
  ${PROJECT_BINARY_DIR}/src/neurotessmesh/version.cpp
  mainwindow.ui
  resources.qrc
  neurotessmesh.cpp
  MainWindow.cpp
  OpenGLWidget.cpp
  ColorSelectionWidget.cpp
  CMakeSetup.rc
)

set(NEUROTESSMESH_HEADERS
  ${PROJECT_BINARY_DIR}/include/neurotessmesh/version.h
)

set(NEUROTESSMESH_MOC_HEADERS
  MainWindow.h
  OpenGLWidget.h
  ColorSelectionWidget.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(NEUROTESSMESH_LINK_LIBRARIES
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  # ${GLUT_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  Qt5::OpenGL
  nsol
  nlgeometry
  nlfem
  nlgenerator
  nlrender
)

if ( NOT DEFAULT_CONTEXT_OPENGL_MAJOR )
  set( DEFAULT_CONTEXT_OPENGL_MAJOR 4 )
endif( )
if ( NOT DEFAULT_CONTEXT_OPENGL_MINOR )
  set( DEFAULT_CONTEXT_OPENGL_MINOR 4 )
endif( )

add_definitions( "-DDEFAULT_CONTEXT_OPENGL_MAJOR=${DEFAULT_CONTEXT_OPENGL_MAJOR}" )
add_definitions( "-DDEFAULT_CONTEXT_OPENGL_MINOR=${DEFAULT_CONTEXT_OPENGL_MINOR}" )


if (ZEROEQ_FOUND)
  list(APPEND NEUROTESSMESH_LINK_LIBRARIES ZeroEQ ${CMAKE_THREAD_LIBS_INIT})
endif()

if (LEXIS_FOUND)
  list(APPEND NEUROTESSMESH_LINK_LIBRARIES Lexis)
endif()

if (GMRVZEQ_FOUND)
  list( APPEND NEUROTESSMESH_LINK_LIBRARIES gmrvlex )
endif()

if (DEFLECT_FOUND)
  list(APPEND NEUROTESSMESH_LINK_LIBRARIES Deflect)
endif()

if ( MSVC )
  add_definitions( -DNEUROTESSMESH_STATIC )
endif( )

common_application(NeuroTessMesh GUI ${COMMON_APP_ARGS})

endif(QT5CORE_FOUND)
