option( NLGEOMETRY_WITH_TESTS "NLGEOMETRY_WITH_TESTS" ON )

set( NLGEOMETRY_PUBLIC_HEADERS
  AxisAlignedBoundingBox.h
  Facet.h
  Mesh.h
  OrbitalVertex.h
  Parametrizer.h
  Reader/ObjReaderTemplated.h
  Reader/VDMapReader.h
  SectionQuad.h
  SpatialHashTable.h
  MeshMetrics.h
  VDMap.h
  Vertex.h
  Writer/ObjWriter.h
  Writer/OffWriter.h
  Writer/VDMapWriter.h
)

set( NLGEOMETRY_HEADERS
)

set( NLGEOMETRY_SOURCES
  AxisAlignedBoundingBox.cpp
  Facet.cpp
  Mesh.cpp
  OrbitalVertex.cpp
  Parametrizer.cpp
  Reader/VDMapReader.cpp
  SectionQuad.cpp
  SpatialHashTable.cpp
  MeshMetrics.cpp
  VDMap.cpp
  Vertex.cpp
  Writer/ObjWriter.cpp
  Writer/OffWriter.cpp
  Writer/VDMapWriter.cpp
)

set( NLGEOMETRY_LINK_LIBRARIES
  ReTo
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  )

if( TIFF_FOUND )
  list( APPEND NLGEOMETRY_LINK_LIBRARIES ${TIFF_LIBRARIES} )
endif( )

if( QT5CORE_FOUND )
  list( APPEND NLGEOMETRY_LINK_LIBRARIES Qt5::Core )
endif( )

set( NLGEOMETRY_INCLUDE_NAME nlgeometry )
set( NLGEOMETRY_NAMESPACE nlgeometry )
add_definitions( -DNEUROLOTS_SHARED )
common_library( nlgeometry )

add_subdirectory(tests)
