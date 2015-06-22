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
#ifdef Darwin
#include <gl.h>
#include <glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

//Neurolots
#include "Program.h"
#include "Camera.h"
#include "../nlgenerator/NeuronMeshGenerator.h"
#include "../nlgenerator/Icosphere.h"

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

      NeuronMesh( nsol::NeuronMorphologyPtr morpho_,
                  Program * programTriangles_, Program * programQuads_,
                  Camera * _camera );
      ~NeuronMesh( void );

      void Init( void );
      void Paint( void );

      //Getters
      bool PaintSoma( void );
      bool PaintNeurites( void );

      //Setters
      void PaintSoma( bool paintSoma_ );
      void PaintNeurites( bool paintNeurites_ );

    private:

      nsol::NeuronMorphologyPtr _morpho;

      Program * _programTriangles;
      Program * _programQuads;
      Camera * camera_;

      GLuint vao_;
      GLuint * vbo_;

      unsigned int _size;
      unsigned int _somaEnd;

      bool _isInit;

      bool _paintSoma;
      bool _paintNeurites;
  };

  typedef NeuronMesh * NeuronMeshPtr;

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MESH__

//EOF
