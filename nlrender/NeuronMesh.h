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
#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
#include <GL/glew.h>
#endif
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
#include "../nlgenerator/NeuronMeshGenerator.h"
#include "../nlgenerator/Icosphere.h"
#include "../nlgeometry/Facet.h"

//Eigen
#include <Eigen/Dense>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include <neurolots/nlrender/api.h>

namespace neurolots
{


  class NeuronMesh
  {

  public:

    NLRENDER_API
    NeuronMesh( const nsol::NeuronMorphologyPtr& morpho_ );

    NLRENDER_API
    ~NeuronMesh( void );

    NLRENDER_API
    void Init( void );

    NLRENDER_API
    void Regenerate( const float& alphaRadius_,
                     const std::vector< float >& alphaNeurites_ );

    NLRENDER_API
    void PaintSoma( void ) const;

    NLRENDER_API
    void PaintNeurites( void ) const;

    NLRENDER_API
    void WriteOBJ( const std::string& fileName_, Vertices& vertices_,
                   const Facets& facets_ ) const;

  private:

    nsol::NeuronMorphologyPtr _morpho;

    GLuint vao_;
    GLuint * vbo_;

    unsigned int _size;
    unsigned int _somaEnd;

    bool _isInit;
  };

  typedef NeuronMesh * NeuronMeshPtr;

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MESH__

//EOF
