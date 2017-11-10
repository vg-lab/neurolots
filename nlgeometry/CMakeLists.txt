option( NLGEOMETRY_WITH_EXAMPLES "NLGEOMETRY_WITH_EXAMPLES" ON )
option( NLGEOMETRY_WITH_TESTS "NLGEOMETRY_WITH_TESTS" ON )

set( NLGEOMETRY_PUBLIC_HEADERS
  AxisAlignedBoundingBox.h
  Facet.h
  Mesh.h
  OrbitalVertex.h
  Reader/ObjReader.h
  SectionQuad.h
  SpatialHashTable.h
  Vertex.h
  Writer/ObjWriter.h
)

set( NLGEOMETRY_HEADERS
)

set( NLGEOMETRY_SOURCES
  AxisAlignedBoundingBox.cpp
  Facet.cpp
  Mesh.cpp
  OrbitalVertex.cpp
  Reader/ObjReader.cpp
  SectionQuad.cpp
  SpatialHashTable.cpp
  Vertex.cpp
  Writer/ObjWriter.cpp
)

set( NLGEOMETRY_LINK_LIBRARIES
  ${GLEW_LIBRARIES}
)

set( NLGEOMETRY_INCLUDE_NAME nlgeometry )
set( NLGEOMETRY_NAMESPACE nlgeometry )
add_definitions( -DNEUROLOTS_SHARED )
common_library( nlgeometry )

add_subdirectory(tests)
