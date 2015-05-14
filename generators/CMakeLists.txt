# add_library( generators SHARED GeometricPrimitive.cpp NeuronMeshGenerator.cpp 
#              VectorizedNode.cpp Edges.cpp Icosphere.cpp Quad.cpp )

# target_link_libraries(generators  ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} 
#                                  ${GLEW_LIBRARIES} ${NSOL_LIBRARIES})


set(NLGENERATOR_LINK_LIBRARIES nlfem)
include_directories( ../fem )

set(NLGENERATOR_PUBLIC_HEADERS
  Edges.h
  GeometricPrimitive.h
  Icosphere.h
  NeuronMeshGenerator.h
  Quad.h
  VectorizedNode.h
)

set(NLGENERATOR_HEADERS
)

set(NLGENERATOR_SOURCES
  Edges.cpp
  GeometricPrimitive.cpp
  Icosphere.cpp
  NeuronMeshGenerator.cpp
  Quad.cpp
  VectorizedNode.cpp
)

set(PROJECT_INCLUDE_NAME nlgenerator)

set(NLGENERATOR_LINK_LIBRARIES
  ${OPENGL_LIBRARIES} 
  ${GLEW_LIBRARIES} 
  ${GLUT_LIBRARIES} 
  ${NSOL_LIBRARIES} 
  ${ZEQ_LIBRARIES} 
  ${BBPSDK_LIBRARIES} 
  nlfem )
  
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlgenerator )
