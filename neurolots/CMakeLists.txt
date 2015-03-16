add_executable( neurolots Main.cpp Program.cpp Camera.cpp NeuronMesh.cpp 
                NeuronMorphology.cpp  NeuronsCollection.cpp) 

target_link_libraries( neurolots  ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} 
                                  ${GLEW_LIBRARIES} ${NSOL_LIBRARIES} 
                                  generators fem )
 
