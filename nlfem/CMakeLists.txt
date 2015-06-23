set(NLFEM_PUBLIC_HEADERS
  Fem.h
  Node.h
  Tetrahedron.h
)

set(NLFEM_HEADERS
)

set(NLFEM_SOURCES
  Fem.cpp
  Node.cpp
  Tetrahedron.cpp
)

set(PROJECT_INCLUDE_NAME neurolots/nlfem)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlfem )
