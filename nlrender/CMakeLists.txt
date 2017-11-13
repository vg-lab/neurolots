########## RETO ##########
include( reto_generate_shaders )

reto_generate_shaders(
  NLRENDER_SHADERS
  nlrender
  ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/
  ${CMAKE_CURRENT_SOURCE_DIR}/Shaders.h
)
########## RETO ##########


set(NLRENDER_PUBLIC_HEADERS
  Shaders.h
  Config.h
  Renderer.h
)

set(NLRENDER_HEADERS
)

set(NLRENDER_SOURCES
  Config.cpp
  Renderer.cpp
)

set(NLRENDER_LINK_LIBRARIES
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  nlgeometry
  ReTo
)

set(NLRENDER_INCLUDE_NAME nlrender)
set(NLRENDER_NAMESPACE nlrender)
add_definitions(-DNEUROLOTS_SHARED)
common_library( nlrender )
