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

  set( NEUROLOTSHAUSDORFFDISTANCE_SOURCES hausdorffDistance.cpp )
  set( NEUROLOTSHAUSDORFFDISTANCE_LINK_LIBRARIES nlgeometry )
  common_application( neurolotsHausdorffDistance NOHELP )
  list( APPEND NEUROLOTS_EXAMPLES_FILES ${NEUROLOTSHAUSDORFFDISTANCE_SOURCES} )

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

      set( NEUROLOTSVDMSCENESAVER_SOURCES vdmSceneSaver.cpp )
      set( NEUROLOTSVDMSCENESAVER_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsVDMSceneSaver NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSVDMSCENESAVER_SOURCES} )

      set( NEUROLOTSTEST2GENERATIONTIME_SOURCES test2-generationTime.cpp )
      set( NEUROLOTSTEST2GENERATIONTIME_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsTest2GenerationTime NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSTEST2GENERATIONTIME_SOURCES} )

      set( NEUROLOTSTEST2LOADTIMEMESHES_SOURCES test2-loadTimeMeshes.cpp
        Shaders.h DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSTEST2LOADTIMEMESHES_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsTest2LoadTimeMeshes NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSTEST2LOADTIMEMESHES_SOURCES} )

      set( NEUROLOTSTEST2LOADTIME_SOURCES test2-loadTime.cpp )
      set( NEUROLOTSTEST2LOADTIME_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsTest2LoadTime NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSTEST2LOADTIME_SOURCES} )

      set( NEUROLOTSTEST3ORIGINALMESH_SOURCES test3-originalMesh.cpp
        Shaders.h DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSTEST3ORIGINALMESH_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsTest3OriginalMesh NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSTEST3ORIGINALMESHE_SOURCES})

      set( NEUROLOTSTEST3SCENERENDER_SOURCES test3-sceneRender.cpp
        Shaders.h DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSTEST3SCENERENDER_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsTest3SceneRender NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSTEST3SCENERENDER_SOURCES} )

      set( NEUROLOTSTEST4ORIGINALMESH_SOURCES test4-originalMesh.cpp
        Shaders.h DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSTEST4ORIGINALMESH_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsTest4OriginalMesh NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSTEST4ORIGINALMESHE_SOURCES})

      set( NEUROLOTSTEST4SCENERENDER_SOURCES test4-sceneRender.cpp
        Shaders.h DemoCallbacks.cpp DemoCallbacks.h )
      set( NEUROLOTSTEST4SCENERENDER_LINK_LIBRARIES ReTo nsol nlgeometry
        nlgenerator nlrender ${Boost_SYSTEM_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARIES} ${GLUT_LIBRARIES} )
      common_application( neurolotsTest4SceneRender NOHELP )
      list( APPEND NEUROLOTS_EXAMPLES_FILES
        ${NEUROLOTSTEST4SCENERENDER_SOURCES} )


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
