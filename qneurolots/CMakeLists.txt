if( QT5CORE_FOUND )

set(Name Qneurolots)
set(NAME QNEUROLOTS)
set(namespace qneurolots)
set(NAMESPACE QNEUROLOTS)
set(INCLUDE_NAME qneurolots)
configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/api.h
  ${PROJECT_BINARY_DIR}/include/qneurolots/api.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.h
  ${PROJECT_BINARY_DIR}/include/qneurolots/version.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.cpp
  ${PROJECT_BINARY_DIR}/src/qneurolots/version.cpp @ONLY)



set(UIDIR ${CMAKE_CURRENT_SOURCE_DIR}/ui)
set(QRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/qrc)

set(Name Qneurolots)
set(NAME QNEUROLOTS)
set(namespace qneurolots)
set(NAMESPACE QNEUROLOTS)
set(INCLUDE_NAME qneurolots)
configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/api.h
  ${PROJECT_BINARY_DIR}/include/qneurolots/api.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.h
  ${PROJECT_BINARY_DIR}/include/qneurolots/version.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMake/common/cpp/version.cpp
  ${PROJECT_BINARY_DIR}/src/version.cpp @ONLY)

set(QNEUROLOTS_LINK_LIBRARIES nsol
  Qt5::Core Qt5::Concurrent Qt5::Network Qt5::Quick Qt5::Widgets)

if (ZEQ_FOUND)
  list(APPEND QNEUROLOTS_LINK_LIBRARIES zeq zeqHBP)
endif()

if (GMRVZEQ_FOUND)
  list(APPEND QNEUROLOTS_LINK_LIBRARIES gmrvzeq)
endif()

if (DEFLECT_FOUND)
  list(APPEND QNEUROLOTS_LINK_LIBRARIES Deflect)
endif()

if (FIRES_FOUND)
  list(APPEND QNEUROLOTS_LINK_LIBRARIES fires)
endif()


set(QNEUROLOTS_SOURCES
  ${PROJECT_BINARY_DIR}/src/qneurolots/version.cpp
  qneurolots.cpp
  MainWindow.cpp
  OpenGLWidget.cpp
  CMakeSetup.rc
)

set(QNEUROLOTS_HEADERS
  ${PROJECT_BINARY_DIR}/include/qneurolots/version.h
)

set(QNEUROLOTS_MOC_HEADERS
  MainWindow.h
  OpenGLWidget.h)

set(QNEUROLOTS_UI_FORMS
  ${UIDIR}/mainwindow.ui
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


set(QNEUROLOTS_RESOURCES
  ${QRCDIR}/resources.qrc
)


set(QNEUROLOTS_LINK_LIBRARIES
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  # ${GLUT_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  Qt5::OpenGL
  nsol
  nlfem
  nlgenerator
  nlrender
)

if ( NOT DEFAULT_CONTEXT_OPENGL_MAJOR )
  set( DEFAULT_CONTEXT_OPENGL_MAJOR 3 )
endif( )
if ( NOT DEFAULT_CONTEXT_OPENGL_MINOR )
  set( DEFAULT_CONTEXT_OPENGL_MINOR 3 )
endif( )

add_definitions( "-DDEFAULT_CONTEXT_OPENGL_MAJOR=${DEFAULT_CONTEXT_OPENGL_MAJOR}" )
add_definitions( "-DDEFAULT_CONTEXT_OPENGL_MINOR=${DEFAULT_CONTEXT_OPENGL_MINOR}" )


if (ZEQ_FOUND)
  list(APPEND QNEUROLOTS_LINK_LIBRARIES zeq zeqHBP ${CMAKE_THREAD_LIBS_INIT})
endif()

if (GMRVZEQ_FOUND)
  list( APPEND QNEUROLOTS_LINK_LIBRARIES gmrvzeq )
endif()

if (DEFLECT_FOUND)
  list(APPEND QNEUROLOTS_LINK_LIBRARIES Deflect)
endif()

common_application( qneurolots )

endif(QT5CORE_FOUND)
