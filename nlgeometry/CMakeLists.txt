set( NLGEOMETRY_PUBLIC_HEADERS
  Vertex.h
  Facet.h
  SpatialHashTable.h
)

set( NLGEOMETRY_HEADERS
)

set( NLGEOMETRY_SOURCES
  Vertex.cpp
  Facet.cpp
  SpatialHashTable.cpp
)

set( NLGEOMETRY_LINK_LIBRARIES
)

set( NLGEOMETRY_INCLUDE_NAME neurolots/nlgeometry )
set( NLGEOMETRY_NAMESPACE nlgeometry )
add_definitions( -DNEUROLOTS_SHARED )
common_library( nlgeometry )