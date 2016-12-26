include_directories( ../nlgenerator ../nlgeometry )

set(NLRENDER_PUBLIC_HEADERS
  Config.h
  Morphologies.h
  Neuron.h
  NeuronMesh.h
  NeuronMorphology.h
  NeuronsCollection.h
  Program.h
  Shader.h
)

set(NLRENDER_HEADERS
)

set(NLRENDER_SOURCES
  Config.cpp
  Morphologies.cpp
  Neuron.cpp
  NeuronMesh.cpp
  NeuronMorphology.cpp
  NeuronsCollection.cpp
  Program.cpp
  Shader.cpp
)

set(NLRENDER_LINK_LIBRARIES
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  nsol
  nlgeometry
  nlgenerator
  ReTo
)

if (ZEROEQ_FOUND)
  set(NLRENDER_LINK_LIBRARIES ${NLRENDER_LINK_LIBRARIES} ZeroEQ)
endif( )

if(LEXIS_FOUND)
  set(NLRENDER_LINK_LIBRARIES ${NLRENDER_LINK_LIBRARIES} Lexis)
endif( )

if (GMRVLEX_FOUND)
  set(NLRENDER_LINK_LIBRARIES ${NLRENDER_LINK_LIBRARIES} gmrvlex)
endif( )

set(NLRENDER_INCLUDE_NAME nlrender)
set(NLRENDER_NAMESPACE nlrender)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlrender )
