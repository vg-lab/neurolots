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

      set( NEUROLOTSOBJRENDER_SOURCES objRender.cpp Shaders.h DemoCallbacks.cpp
        DemoCallbacks.h )
      set( NEUROLOTSOBJRENDER_LINK_LIBRARIES ReTo nlgeometry ${GLUT_LIBRARIES} )
      common_application( neurolotsObjRender NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES ${NEUROLOTSOBJRENDER_SOURCES} )

      set( NEUROLOTSCUBERENDER_SOURCES cubeRender.cpp Cube.h Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSCUBERENDER_LINK_LIBRARIES ReTo nlgeometry
        ${GLUT_LIBRARIES} )
      common_application( neurolotsCubeRender NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES ${NEUROLOTSCUBERENDER_SOURCES} )

      set( NEUROLOTSMORPHOLOGYRENDER_SOURCES morphologyRenderer.cpp Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSMORPHOLOGYRENDER_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${GLUT_LIBRARIES} )
      common_application( neurolotsMorphologyRender NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSMORPHOLOGYRENDER_SOURCES} )

    endif( )
  endif( )

  include(InstallFiles)
  install_files( share/neurolots/examples FILES ${NEUROLOTS_EXAMPLES_FILES}
    COMPONENT examples )

endif( NEUROLOTS_WITH_EXAMPLES )
