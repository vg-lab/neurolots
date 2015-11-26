include_directories( ../nlgenerator ../nlgeometry )

set(NLRENDER_PUBLIC_HEADERS
  Camera.h
  Config.h
  Morphologies.h
  Neuron.h
  NeuronMesh.h
  NeuronMorphology.h
  NeuronsCollection.h
  Program.h
)

set(NLRENDER_HEADERS
)

set(NLRENDER_SOURCES
  Camera.cpp
  Config.cpp
  Morphologies.cpp
  Neuron.cpp
  NeuronMesh.cpp
  NeuronMorphology.cpp
  NeuronsCollection.cpp
  Program.cpp
)

set(NLRENDER_LINK_LIBRARIES
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  nsol
  nlgeometry
  nlgenerator
)

if (ZEQ_FOUND)
  set(NLRENDER_LINK_LIBRARIES ${NLRENDER_LINK_LIBRARIES} zeq zeqHBP)
endif( )

if (GMRVZEQ_FOUND)
  set(NLRENDER_LINK_LIBRARIES ${NLRENDER_LINK_LIBRARIES} gmrvzeq)
endif( )

set(NLRENDER_INCLUDE_NAME neurolots/nlrender)
set(NLRENDER_NAMESPACE nlrender)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlrender )
