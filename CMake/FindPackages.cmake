

#FIND NSOL
find_package(nsol REQUIRED)
include_directories(${NSOL_INCLUDE_DIRS}) 

#FIND EIGEN 
find_package(Eigen3 REQUIRED)
include_directories(BEFORE SYSTEM ${EIGEN3_INCLUDE_DIR})


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

#FIND OPENGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

# FIND GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
add_definitions(${GLEW_DEFINITIONS})
if(NOT GLEW_FOUND)
message(ERROR " GLEW not found!")
endif(NOT GLEW_FOUND)

#FIND GLUT
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

