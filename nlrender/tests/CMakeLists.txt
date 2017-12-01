if ( NLRENDER_WITH_TESTS )

  include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

  if( ${Boost_FOUND})

    set(TEST_LIBRARIES
      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
      nlrender
      )

      include(CommonCTest)

  endif()

endif()
