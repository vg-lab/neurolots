if ( NEUROLOTS_WITH_EXAMPLES )

  ########## RETO ##########
  include( reto_generate_shaders )

  reto_generate_shaders(
    EXAMPLES_SHADERS
    examples
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders.h
    )
  ########## RETO ##########

  if ( GLUT_FOUND )
    if( NOT APPLE )

      add_executable(nlgeometryObjRender objRender.cpp Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      target_link_libraries(nlgeometryObjRender
        ReTo
        nlgeometry
        ${GLUT_LIBRARIES}
        )

      add_executable(nlgeometryCubeRender cubeRender.cpp Cube.h Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      target_link_libraries(nlgeometryCubeRender
        ReTo
        nlgeometry
        ${GLUT_LIBRARIES}
        )

      add_executable(nlrenderMorphologyRenderer morphologyRenderer.cpp
        Shaders.h  DemoCallbacks.cpp DemoCallbacks.h )
      target_link_libraries(nlrenderMorphologyRenderer
        ReTo
        nsol
        nlgeometry
        nlgenerator
        nlrender
        ${GLUT_LIBRARIES}
        )

    endif( )
  endif( )
endif( )
