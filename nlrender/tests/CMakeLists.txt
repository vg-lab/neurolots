if(NLRENDER_WITH_TESTS)

  # Function to add a test
  function(add_component_test NAME SOURCE)
    add_executable(${NAME} ${SOURCE})
    target_include_directories(${NAME}
      PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(${NAME} PRIVATE
      ${CURRENT_TARGET_NAME}
      Boost::unit_test_framework
    )
    add_test(NAME ${NAME} COMMAND ${NAME})

    # Create all tests target
    if(NOT TARGET ${CURRENT_TARGET_NAME}_all_tests)
      add_custom_target(${CURRENT_TARGET_NAME}_all_tests)
    endif()

    # Add test to global tests target
    add_dependencies(${CURRENT_TARGET_NAME}_all_tests ${NAME})
  endfunction()

  # Add tests
  # add_component_test(nlrender Foo.cpp)

endif()
