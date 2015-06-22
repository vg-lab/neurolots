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

set(PROJECT_INCLUDE_NAME neurolots/nlgenerator)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlgenerator )
