set(NEUROLOTS_PUBLIC_HEADERS
  ${COMMON_INCLUDES}
)

install( FILES ${COMMON_INCLUDES} DESTINATION include/neurolots COMPONENT dev )

set(NEUROLOTS_HEADERS
)

set(NEUROLOTS_SOURCES
  Main.cpp
)

include_directories( ../fem ../generators )

set(NEUROLOTS_LINK_LIBRARIES
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  nsol
  nlfem
  nlgenerator
  nlrender
)

if (ZEQ_FOUND)
  list(APPEND NEUROLOTS_LINK_LIBRARIES zeq zeqHBP ${CMAKE_THREAD_LIBS_INIT})
endif()

if (GMRVZEQ_FOUND)
  list( APPEND NEUROLOTS_LINK_LIBRARIES gmrvzeq )
endif()

if (DEFLECT_FOUND)
  list(APPEND NEUROLOTS_LINK_LIBRARIES Deflect)
endif()


if (APPLE)
  set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations" )
endif( )

common_application( neurolots )
