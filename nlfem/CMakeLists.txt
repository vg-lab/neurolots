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

set(NLFEM_INCLUDE_NAME nlfem)
set(NLFEM_NAMESPACE nlfem)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlfem )
