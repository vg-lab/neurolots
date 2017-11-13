set(NLGENERATOR_PUBLIC_HEADERS
  Icosphere.h
  JointNode.h
  MeshGenerator.h
)

set(NLGENERATOR_HEADERS
)

set(NLGENERATOR_SOURCES
  Icosphere.cpp
  JointNode.cpp
  MeshGenerator.cpp
)

set(NLGENERATOR_LINK_LIBRARIES
  nsol
  nlgeometry
  nlphysics
  )

set(NLGENERATOR_INCLUDE_NAME nlgenerator)
set(NLGENERATOR_NAMESPACE nlgenerator)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlgenerator )
