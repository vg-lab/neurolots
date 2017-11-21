option( NLPHYSICS_WITH_EXAMPLES "NLPHYSICS_WITH_EXAMPLES" ON )
option( NLPHYSICS_WITH_TESTS "NLPHYSICS_WITH_TESTS" ON )

set(NLPHYSICS_PUBLIC_HEADERS
  Fem.h
  Node.h
  Tetrahedron.h
)

set(NLPHYSICS_HEADERS
)

set(NLPHYSICS_SOURCES
  Fem.cpp
)

set(NLPHYSICS_INCLUDE_NAME nlphysics)
set(NLPHYSICS_NAMESPACE nlphysics)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlphysics )

add_subdirectory(tests)
