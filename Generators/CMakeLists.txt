add_library( generators SHARED GeometricPrimitive.cpp NeuronMeshGenerator.cpp 
             VectorizedNode.cpp Edges.cpp Icosphere.cpp )

target_link_libraries(generators  ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} 
                                 ${GLEW_LIBRARIES} ${NSOL_LIBRARIES})