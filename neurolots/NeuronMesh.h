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
#include <NeuronMeshGenerator.h>
#include <Icosphere.h>

//Eigen
#include <Eigen/Dense>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>


namespace neurolots
{


  class NeuronMesh
  {

    public:

      NeuronMesh( nsol::NeuronMorphologyPtr morpho_, Program * _program,
                  Camera * _camera );
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


      nsol::NeuronMorphologyPtr _morpho;

      Program * program_;
      Camera * camera_;

      GLuint vao_;
      GLuint * vbo_;

      GLuint _vaoLines;
      GLuint * _vboLines;

      int size_;
      std::vector<float> color_;

      bool _isInit;

  };

  typedef NeuronMesh * NeuronMeshPtr;

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MESH__

//EOF
