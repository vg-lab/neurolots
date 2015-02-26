# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   neurolots
#   2014 (c) Universidad Rey Juan Carlos
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

project(neurolots)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wno-unknown-pragmas")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")


list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)


#FIND NSOL
find_package(nsol REQUIRED)
include_directories(${NSOL_INCLUDE_DIRS}) 

#FIND EIGEN 
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})


#########################################################
# FIND NVIDIA OPENGL
#########################################################
# This tries to fix problems when linking in Linux with nVidia OpenGL
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(nvidiaOpenGL)
  if ( NVIDIA_OPENGL_gl_LIBRARY )
    set( CMAKE_EXE_LINKER_FLAGS "-L${NVIDIA_OPENGL_gl_LIBRARY_PATH}" )
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

add_executable(neurolots neurolots/Main.cpp neurolots/Program.cpp 
               neurolots/Camera.cpp neurolots/NeuronMesh.cpp 
               neurolots/NeuronMeshGenerator.cpp neurolots/Morphologies.cpp
               neurolots/NeuronsCollection.cpp ) 

target_link_libraries(neurolots  ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} 
                                 ${GLEW_LIBRARIES} ${NSOL_LIBRARIES})
 
install(FILES README.md DESTINATION doc )
