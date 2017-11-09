########## RETO ##########
include( reto_generate_shaders )

reto_generate_shaders(
  EXAMPLES_SHADERS
  examples
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
  ${CMAKE_CURRENT_SOURCE_DIR}/Shaders.h
  )

message(${CMAKE_CURRENT_SOURCE_DIR}/../exampleShaders/ )
########## RETO ##########

add_executable(nlgeometryObjRender objRender.cpp Shaders.h)
target_link_libraries(nlgeometryObjRender
  ReTo
  nlgeometry
  nlrender
  ${GLUT_LIBRARIES}
  )

add_executable(nlgeometryCubeRender cubeRender.cpp Cube.h Shaders.h)
target_link_libraries(nlgeometryCubeRender
  ReTo
  nlgeometry
  nlrender
  ${GLUT_LIBRARIES}
  )
