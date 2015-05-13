#add_library( fem Fem.cpp Node.cpp Tetrahedron.cpp )


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

set(PROJECT_INCLUDE_NAME nlfem )
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlfem )



