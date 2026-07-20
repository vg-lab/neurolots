if(NEUROLOTS_WITH_EXAMPLES)

  # Generate Shaders header
  reto_generate_shaders(
    EXAMPLES_SHADERS
    examples
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders.h
  )

  # Examples files
  list(APPEND NEUROLOTS_EXAMPLES_FILES Shaders.h)

  # Function to create and install an example application
  function(add_project_example)
    set(options)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs SOURCES LINK_LIBRARIES)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}")

    if(NOT ARG_TARGET_NAME OR NOT ARG_SOURCES)
      message(FATAL_ERROR "TARGET_NAME and SOURCES are required arguments")
    endif()

    add_executable(${ARG_TARGET_NAME} ${ARG_SOURCES})
    target_link_libraries(${ARG_TARGET_NAME} PRIVATE ${ARG_LINK_LIBRARIES})

    # Get current parent scope value and create a local copy
    set(PROJECT_EXAMPLES_FILES_VAR "${${PROJECT_NAME}_EXAMPLES_FILES}")
    set(current_examples ${PROJECT_EXAMPLES_FILES_VAR})
    
    # Convert to CMake list and append new sources
    list(APPEND current_examples ${ARG_SOURCES})
    
    # Remove duplicates and update parent scope
    list(REMOVE_DUPLICATES current_examples)
    set(${PROJECT_NAME}_EXAMPLES_FILES "${current_examples}" PARENT_SCOPE)

    # Create all examples target
    if(NOT TARGET ${PROJECT_NAME}_all_examples)
      add_custom_target(${PROJECT_NAME}_all_examples)
    endif()

    # Add example to global examples target
    add_dependencies(${PROJECT_NAME}_all_examples ${ARG_TARGET_NAME})
  endfunction()

  if(GLUT_FOUND)
    if(NOT APPLE)

      # neurolotsObjRender
      add_project_example(
        TARGET_NAME neurolotsObjRender
        SOURCES
          objRender.cpp
          Shaders.h
          DemoCallbacks.cpp
          DemoCallbacks.h
        LINK_LIBRARIES
          ReTo::ReTo
          neurolots::nlgeometry
          GLUT::GLUT
      )

      # neurolotsObjRenderTransparency
      add_project_example(
        TARGET_NAME neurolotsObjRenderTransparency
        SOURCES
          objRenderTransparency.cpp
          Shaders.h
          DemoCallbacks.cpp
          DemoCallbacks.h
        LINK_LIBRARIES
          ReTo::ReTo
          neurolots::nlgeometry
          GLUT::GLUT
      )

      # neurolotsCubeRender
      add_project_example(
        TARGET_NAME neurolotsCubeRender
        SOURCES
          cubeRender.cpp
          Cube.h
          Shaders.h
          DemoCallbacks.cpp
          DemoCallbacks.h
        LINK_LIBRARIES
          ReTo::ReTo
          neurolots::nlgeometry
          GLUT::GLUT
      )

      # neurolotsMorphologyRender
      add_project_example(
        TARGET_NAME neurolotsMorphologyRender
        SOURCES
          morphologyRenderer.cpp
          Shaders.h
          DemoCallbacks.cpp
          DemoCallbacks.h
        LINK_LIBRARIES
          ReTo::ReTo
          nsol::nsol
          neurolots::nlgeometry
          neurolots::nlgenerator
          neurolots::nlrender
          Boost::filesystem
          GLUT::GLUT
      )

      # neurolotsMorphologyStructureRender
      add_project_example(
        TARGET_NAME neurolotsMorphologyStructureRender
        SOURCES
          morphologyStructureRenderer.cpp
          Shaders.h
          DemoCallbacks.cpp
          DemoCallbacks.h
        LINK_LIBRARIES
          ReTo::ReTo
          nsol::nsol
          neurolots::nlgeometry
          neurolots::nlgenerator
          neurolots::nlrender
          Boost::filesystem
          GLUT::GLUT
      )

      # neurolotsMorphologyExtractor
      add_project_example(
        TARGET_NAME neurolotsMorphologyExtractor
        SOURCES
          morphologyExtractor.cpp
          Shaders.h
        LINK_LIBRARIES
          ReTo::ReTo
          nsol::nsol
          neurolots::nlgeometry
          neurolots::nlgenerator
          neurolots::nlrender
          Boost::filesystem
          GLUT::GLUT
      )

    endif()
  endif()

  # Define installation directory variables
  include(GNUInstallDirs)

  # Install examples source files
  install(FILES ${${PROJECT_NAME}_EXAMPLES_FILES}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}/examples
    COMPONENT ${PROJECT_NAME}_examples
  )

endif()
