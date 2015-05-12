include(System)
list(APPEND FIND_PACKAGES_DEFINES ${SYSTEM})
find_package(PkgConfig)


#########################################################
# FIND NVIDIA OPENGL
#########################################################

# This tries to fix problems when linking in Linux with nVidia OpenGL
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(nvidiaOpenGL)
  if ( NVIDIA_OPENGL_gl_LIBRARY )
    set( CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,${NVIDIA_OPENGL_gl_LIBRARY_PATH}" )
    message(STATUS "nVidia library used ["
      ${NVIDIA_OPENGL_gl_LIBRARY_PATH}
      "]")
  endif()
endif()

#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  set(OpenGL_name OPENGL)
  set(OpenGL_FOUND TRUE)
elseif(OpenGL_FOUND)
  set(OpenGL_name OpenGL)
  set(OPENGL_FOUND TRUE)
endif()
if(OpenGL_name)
  list(APPEND FIND_PACKAGES_DEFINES NEUROLOTS_USE_OPENGL)
  if(NOT COMMON_LIBRARY_TYPE MATCHES "SHARED")
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES OpenGL)
  endif()
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} OpenGL")
  link_directories(${${OpenGL_name}_LIBRARY_DIRS})
  if(NOT "${${OpenGL_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(BEFORE SYSTEM ${${OpenGL_name}_INCLUDE_DIRS})
  endif()
endif()


#########################################################
# FIND GLEW
#########################################################
find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  set(GLEW_name GLEW)
  set(GLEW_FOUND TRUE)
elseif(GLEW_FOUND)
  set(GLEW_name GLEW)
  set(GLEW_FOUND TRUE)
endif()
if(GLEW_name)
  list(APPEND FIND_PACKAGES_DEFINES NEUROLOTS_USE_GLEW)
  if(NOT COMMON_LIBRARY_TYPE MATCHES "SHARED")
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES GLEW)
  endif()
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} GLEW")
  link_directories(${${GLEW_name}_LIBRARY_DIRS})
  if(NOT "${${GLEW_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(BEFORE SYSTEM ${${GLEW_name}_INCLUDE_DIRS})
  endif()
endif()


#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
if(GLUT_FOUND)
  set(GLUT_name GLUT)
  set(GLUT_FOUND TRUE)
elseif(GLUT_FOUND)
  set(GLUT_name GLUT)
  set(GLUT_FOUND TRUE)
endif()
if(GLUT_name)
  list(APPEND FIND_PACKAGES_DEFINES NEUROLOTS_USE_GLUT)
  if(NOT COMMON_LIBRARY_TYPE MATCHES "SHARED")
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES GLUT)
  endif()
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} GLUT")
  link_directories(${${GLUT_name}_LIBRARY_DIRS})
  if(NOT "${${GLUT_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(BEFORE SYSTEM ${${GLUT_name}_INCLUDE_DIRS})
  endif()
endif()


#########################################################
# FIND Eigen3
#########################################################
find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
  set(Eigen3_name EIGEN3)
  set(Eigen3_FOUND TRUE)
elseif(Eigen3_FOUND)
  set(Eigen3_name Eigen3)
  set(EIGEN3_FOUND TRUE)
endif()
if(Eigen3_name)
  list(APPEND FIND_PACKAGES_DEFINES NEUROLOTS_WITH_EIGEN3)
  if(NOT COMMON_LIBRARY_TYPE MATCHES "SHARED")
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES Eigen3)
  endif()
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Eigen3")
  link_directories(${${Eigen3_name}_LIBRARY_DIRS})
  if(NOT "${${Eigen3_name}_INCLUDE_DIR}" MATCHES "-NOTFOUND")
    include_directories(BEFORE SYSTEM ${${Eigen3_name}_INCLUDE_DIR})
  endif()
endif()


#########################################################
# FIND Nsol
#########################################################
find_package(nsol REQUIRED)
if(NSOL_FOUND)
  set(nsol_name NSOL)
  set(nsol_FOUND TRUE)
elseif(nsol_FOUND)
  set(nsol_name nsol)
  set(NSOL_FOUND TRUE)
endif()
if(nsol_name)
  list(APPEND FIND_PACKAGES_DEFINES NEUROLOTS_WITH_NSOL)
  if(NOT COMMON_LIBRARY_TYPE MATCHES "SHARED")
    list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES nsol)
  endif()
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} nsol")
  link_directories(${${nsol_name}_LIBRARY_DIRS})
  if(NOT "${${nsol_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(BEFORE SYSTEM ${${nsol_name}_INCLUDE_DIRS})
  endif()
endif()


#########################################################
# FIND Zeq
#########################################################
if(NEUROLOTS_WITH_ZEQ)
  find_package(zeq)
  if(ZEQ_FOUND)
    set(zeq_name ZEQ)
    set(zeq_FOUND TRUE)
  elseif(zeq_FOUND)
    set(zeq_name zeq)
    set(ZEQ_FOUND TRUE)
  endif( )
  if(zeq_name)
    list(APPEND FIND_PACKAGES_DEFINES NEUROLOTS_WITH_ZEQ)
    if(NOT COMMON_LIBRARY_TYPE MATCHES "SHARED")
      list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES zeq)
    endif( )
    set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} zeq")
    link_directories(${${zeq_name}_LIBRARY_DIRS})
    if(NOT "${${zeq_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
      include_directories(BEFORE SYSTEM ${${zeq_name}_INCLUDE_DIRS})
    endif( )
  endif( )
endif( )


#########################################################
# FIND Bbpsdk
#########################################################
if(NEUROLOTS_WITH_BBPSDK)  
  find_package(BBPSDK)
  if(BBPSDK_FOUND)
    set(BBPSDK_name BBPSDK)
    set(BBPSDK_FOUND TRUE)
  elseif(BBPSDK_FOUND)
    set(BBPSDK_name BBPSDK)
    set(BBPSDK_FOUND TRUE)
  endif()
  if(BBPSDK_name)
    list(APPEND FIND_PACKAGES_DEFINES NEUROLOTS_WITH_BBPSDK)
    if(NOT COMMON_LIBRARY_TYPE MATCHES "SHARED")
      list(APPEND NEUROLOTS_DEPENDENT_LIBRARIES BBPSDK)
    endif()
    set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} BBPSDK")
    link_directories(${${BBPSDK_name}_LIBRARY_DIRS})
    if(NOT "${${BBPSDK_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
      include_directories(BEFORE SYSTEM ${${BBPSDK_name}_INCLUDE_DIRS})
    endif()
  endif()
endif( )



# Write defines.h and options.cmake
if(NOT PROJECT_INCLUDE_NAME)
  message(WARNING "PROJECT_INCLUDE_NAME not set, old or missing Common.cmake?")
  set(PROJECT_INCLUDE_NAME ${PROJECT_NAME})
endif()
if(NOT OPTIONS_CMAKE)
  set(OPTIONS_CMAKE ${CMAKE_CURRENT_BINARY_DIR}/options.cmake)
endif()
set(DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_INCLUDE_NAME}/defines${SYSTEM}.h")
list(APPEND COMMON_INCLUDES ${DEFINES_FILE})
set(DEFINES_FILE_IN ${DEFINES_FILE}.in)
file(WRITE ${DEFINES_FILE_IN}
  "// generated by CMake/FindPackages.cmake, do not edit.\n\n"
  "#ifndef ${PROJECT_NAME}_DEFINES_${SYSTEM}_H\n"
  "#define ${PROJECT_NAME}_DEFINES_${SYSTEM}_H\n\n")
file(WRITE ${OPTIONS_CMAKE} "# Optional modules enabled during build\n")
foreach(DEF ${FIND_PACKAGES_DEFINES})
  add_definitions(-D${DEF}=1)
  file(APPEND ${DEFINES_FILE_IN}
  "#ifndef ${DEF}\n"
  "#  define ${DEF} 1\n"
  "#endif\n")
if(NOT DEF STREQUAL SYSTEM)
  file(APPEND ${OPTIONS_CMAKE} "set(${DEF} ON)\n")
endif()
endforeach()
if(CMAKE_MODULE_INSTALL_PATH)
  install(FILES ${OPTIONS_CMAKE} DESTINATION ${CMAKE_MODULE_INSTALL_PATH}
    COMPONENT dev)
else()
  message(WARNING "CMAKE_MODULE_INSTALL_PATH not set, old or missing Common.cmake?")
endif()
file(APPEND ${DEFINES_FILE_IN}
  "\n#endif\n")

include(UpdateFile)
configure_file(${DEFINES_FILE_IN} ${DEFINES_FILE} COPYONLY)
if(Boost_FOUND) # another WAR for broken boost stuff...
  set(Boost_VERSION ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION})
endif()
if(CUDA_FOUND)
  string(REPLACE "-std=c++11" "" CUDA_HOST_FLAGS "${CUDA_HOST_FLAGS}")
  string(REPLACE "-std=c++0x" "" CUDA_HOST_FLAGS "${CUDA_HOST_FLAGS}")
endif()
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
if(QT4_FOUND)
  if(WIN32)
    set(QT_USE_QTMAIN TRUE)
  endif()
  # Configure a copy of the 'UseQt4.cmake' system file.
  if(NOT EXISTS ${QT_USE_FILE})
    message(WARNING "Can't find QT_USE_FILE!")
  else()
    set(_customUseQt4File "${CMAKE_CURRENT_BINARY_DIR}/UseQt4.cmake")
    file(READ ${QT_USE_FILE} content)
    # Change all include_directories() to use the SYSTEM option
    string(REPLACE "include_directories(" "include_directories(SYSTEM " content ${content})
    string(REPLACE "INCLUDE_DIRECTORIES(" "INCLUDE_DIRECTORIES(SYSTEM " content ${content})
    file(WRITE ${_customUseQt4File} ${content})
    set(QT_USE_FILE ${_customUseQt4File})
    include(${QT_USE_FILE})
  endif()
endif()
if(FIND_PACKAGES_FOUND)
  if(MSVC)
    message(STATUS "Configured ${PROJECT_NAME} with ${FIND_PACKAGES_FOUND}")
  else()
    message(STATUS "Configured ${PROJECT_NAME} with ${CMAKE_BUILD_TYPE}${FIND_PACKAGES_FOUND}")
  endif()
endif()

