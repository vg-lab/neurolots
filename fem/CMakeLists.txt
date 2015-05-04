#add_library( fem Fem.cpp Node.cpp Tetrahedron.cpp )


set(NLFEM_PUBLIC_HEADERS
  ${COMMON_INCLUDES}
  Fem.h
  Node.h
  Tetrahedron.h
)

set(NLFEM_HEADERS
)

set(NLFEM_SOURCES
  ${COMMON_SOURCES}
  Fem.cpp
  Node.cpp
  Tetrahedron.cpp 
)

set(PROJECT_INCLUDE_NAME nlfem)
common_library( nlfem )



