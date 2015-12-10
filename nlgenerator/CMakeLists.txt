include_directories( ../nlfem )

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

set(NLGENERATOR_LINK_LIBRARIES
  nsol
  nlfem )

set(NLGENERATOR_INCLUDE_NAME nlgenerator)
set(NLGENERATOR_NAMESPACE nlgenerator)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlgenerator )
