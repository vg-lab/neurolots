# add_library( generators SHARED GeometricPrimitive.cpp NeuronMeshGenerator.cpp 
#              VectorizedNode.cpp Edges.cpp Icosphere.cpp Quad.cpp )

# target_link_libraries(generators  ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} 
#                                  ${GLEW_LIBRARIES} ${NSOL_LIBRARIES})


set(NLGENERATOR_LINK_LIBRARIES nlfem)
include_directories( ../fem )

set(NLGENERATOR_PUBLIC_HEADERS
  ${COMMON_INCLUDES}
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
  ${COMMON_SOURCES}
  Edges.cpp
  GeometricPrimitive.cpp
  Icosphere.cpp
  NeuronMeshGenerator.cpp
  Quad.cpp
  VectorizedNode.cpp
)

set(PROJECT_INCLUDE_NAME nlgenerator)
common_library( nlgenerator )
