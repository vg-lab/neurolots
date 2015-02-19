/**
 * @file    NeuronMesh.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_NEURON_MESH__
#define __NEUROLOTS_NEURON_MESH__

//OpenGL
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

//Neurolots
#include "Program.h"
#include "Camera.h"

//Eigen
#include <Eigen/Dense>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace neurolots {

class NeuronMesh {

public:

NeuronMesh( std::string & file_name, Program * _program, Camera * _camera,
      Eigen::Vector3d _desp, Eigen::Vector3d _color );
  ~NeuronMesh( void );

  void Init( void );
  void Paint( void );
  void CreateLinesMesh( );



private:
  void Load( std::string & file_name );
  void ReadOBJ( std::string & file_name );
  void ReadMOBJ( std::string & file_name );

  std::vector<float> vertices_;
  std::vector<float> tangents_;
  std::vector<float> centers_;
  std::vector<unsigned int> mesh_;
  std::vector<int> nodeIndices_;

  std::vector<float> verticesLine_;
  std::vector<int> meshLine_;

  Program * program_;
  Camera * camera_;

  GLuint vao_;
  GLuint * vbo_;

  int size_;
  std::vector<float> desp_;
  std::vector<float> color_;

};

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MESH__

//EOF
