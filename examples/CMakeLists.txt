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

      set( NEUROLOTSPARAMETRIZER_SOURCES parametrizer.cpp Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSPARAMETRIZER_LINK_LIBRARIES ReTo nlgeometry nlgenerator
        nlrender ${GLUT_LIBRARIES} )
      common_application( neurolotsParametrizer NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES ${NEUROLOTSPARAMETRIZER_SOURCES} )

      set( NEUROLOTSCUBERENDER_SOURCES cubeRender.cpp Cube.h Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSCUBERENDER_LINK_LIBRARIES ReTo nlgeometry
        ${GLUT_LIBRARIES} )
      common_application( neurolotsCubeRender NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES ${NEUROLOTSCUBERENDER_SOURCES} )

      set( NEUROLOTSMORPHOLOGYRENDER_SOURCES morphologyRenderer.cpp Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSMORPHOLOGYRENDER_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsMorphologyRender NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSMORPHOLOGYRENDER_SOURCES} )

      set( NEUROLOTSVDMRENDER_SOURCES vdmRenderer.cpp Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSVDMRENDER_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsVDMRender NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSVDMRENDER_SOURCES} )

      set( NEUROLOTSVDMRENDERTEXTURELOADER_SOURCES vdmRendererTextureLoader.cpp
        Shaders.h DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSVDMRENDERTEXTURELOADER_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsVDMRenderTextureLoader NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSVDMRENDERTEXTURELOADER_SOURCES} )

      set( NEUROLOTSVDMRENDERSCENELOADER_SOURCES vdmRendererSceneLoader.cpp
        Shaders.h DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSVDMRENDERSCENELOADER_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsVDMRenderSceneLoader NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSVDMRENDERSCENELOADER_SOURCES} )

      set( NEUROLOTSVDMEXTRACTOR_SOURCES vdmExtractor.cpp Shaders.h
        DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSVDMEXTRACTOR_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsVDMExtractor NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSVDMEXTRACTOR_SOURCES} )

      set( NEUROLOTSVDMTEXTUREEXTRACTOR_SOURCES vdmTextureExtractor.cpp
        Shaders.h )
      set( NEUROLOTSVDMTEXTUREEXTRACTOR_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsVDMTextureExtractor NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSVDMTEXTUREEXTRACTOR_SOURCES} )

      set( NEUROLOTSMORPHOLOGYEXTRACTOR_SOURCES morphologyExtractor.cpp
        Shaders.h)
      set( NEUROLOTSMORPHOLOGYEXTRACTOR_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsMorphologyExtractor NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSMORPHOLOGYEXTRACTOR_SOURCES} )

    endif( )
  endif( )

  include(InstallFiles)
  install_files( share/neurolots/examples FILES ${NEUROLOTS_EXAMPLES_FILES}
    COMPONENT examples )

endif( NEUROLOTS_WITH_EXAMPLES )
