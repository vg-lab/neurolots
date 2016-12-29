include_directories( ../nlgenerator ../nlgeometry )


########## RETO ##########
include( reto_generate_shaders )

reto_generate_shaders(
  NLRENDER_SHADERS
  nlrender
  ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/
  ${CMAKE_CURRENT_SOURCE_DIR}/Shaders.h
)
########## RETO ##########


set(NLRENDER_PUBLIC_HEADERS
  Shaders.h
  Config.h
  Morphologies.h
  Neuron.h
  NeuronMesh.h
  NeuronMorphology.h
  NeuronsCollection.h
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
