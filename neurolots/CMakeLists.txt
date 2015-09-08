set(NEUROLOTS_PUBLIC_HEADERS
  ${COMMON_INCLUDES}
)

install( FILES ${COMMON_INCLUDES} DESTINATION include/neurolots COMPONENT dev )

set(NEUROLOTS_HEADERS
  Camera.h
  Morphologies.h
  NeuronMesh.h
  NeuronMorphology.h
  NeuronsCollection.h
  Program.h
  Neuron.h
)

set(NEUROLOTS_SOURCES
  Camera.cpp
  Main.cpp
  Morphologies.cpp
  NeuronMesh.cpp
  NeuronMorphology.cpp
  NeuronsCollection.cpp
  Program.cpp
  Neuron.cpp
)

include_directories( ../fem ../generators )

set(NEUROLOTS_LINK_LIBRARIES
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  nsol
  nlfem
  nlgenerator
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
