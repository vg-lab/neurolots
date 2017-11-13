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
  ${GLUT_LIBRARIES}
  )

add_executable(nlgeometryCubeRender cubeRender.cpp Cube.h Shaders.h)
target_link_libraries(nlgeometryCubeRender
  ReTo
  nlgeometry
  ${GLUT_LIBRARIES}
  )

add_executable(nlrenderCubeRenderer cubeRenderer.cpp Cube.h Shaders.h)
target_link_libraries(nlrenderCubeRenderer
  ReTo
  nlgeometry
  nlrender
  ${GLUT_LIBRARIES}
  )

add_executable(nlrenderMorphologyRenderer morphologyRenderer.cpp Shaders.h)
target_link_libraries(nlrenderMorphologyRenderer
  ReTo
  nsol
  nlgeometry
  nlgenerator
  nlrender
  ${GLUT_LIBRARIES}
  )
