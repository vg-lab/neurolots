
set(NEUROLOTS_HEADERS
  Camera.h
  Morphologies.h
  NeuronMesh.h
  NeuronMorphology.h
  NeuronsCollection.h
  Program.h 
)

set(NEUROLOTS_SOURCES
  Camera.cpp
  Main.cpp
  Morphologies.cpp
  NeuronMesh.cpp
  NeuronMorphology.cpp
  NeuronsCollection.cpp
  Program.cpp
)

include_directories( ../fem ../generators )
set(NEUROLOTS_LINK_LIBRARIES 
  ${OPENGL_LIBRARIES} 
  ${GLEW_LIBRARIES} 
  ${GLUT_LIBRARIES} 
  ${NSOL_LIBRARIES} 
  ${ZEQ_LIBRARIES} 
  ${BBPSDK_LIBRARIES} 
  nlfem 
  nlgenerator
)

common_application( neurolots )