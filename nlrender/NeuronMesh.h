/**
 * @file    NeuronMesh.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLRENDER_NEURON_MESH__
#define __NLRENDER_NEURON_MESH__

//OpenGL
#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
#include <GL/glew.h>
#endif
#ifdef Darwin
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

//Neurolots
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

#include <nlrender/api.h>

namespace nlrender
{

  class NeuronMesh
  {

  public:

    NLRENDER_API
    NeuronMesh( const nsol::NeuronMorphologyPtr& morpho_ );

    NLRENDER_API
    ~NeuronMesh( void );



    NLRENDER_API
    void regenerate( const float& alphaRadius_,
                     const std::vector< float >& alphaNeurites_ );

    NLRENDER_API
    void paintSoma( void ) const;

    NLRENDER_API
    void paintNeurites( void ) const;

    NLRENDER_API
    void writeOBJ( const std::string& fileName_,
                   nlgeometry::Vertices& vertices_,
                   const nlgeometry::Facets& facets_ ) const;

  private:

    void _init( void );

    nsol::NeuronMorphologyPtr _morpho;

    unsigned int _vao;
    std::vector< unsigned int > _vbos;

    unsigned int _size;
    unsigned int _somaEnd;
  };

  typedef NeuronMesh * NeuronMeshPtr;

} // end namespace nlrender

#endif // __NLRENDER_NEURON_MESH__

//EOF
