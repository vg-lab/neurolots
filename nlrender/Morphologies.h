/**
 * @file    Morphologies.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLRENDER_MORPHOLOGIES__
#define __NLRENDER_MORPHOLOGIES__

#include "NeuronMesh.h"

#include <nsol/nsol.h>

#include <iostream>
#include <unordered_map>

#include <neurolots/nlrender/api.h>

namespace nlrender
{
  typedef NeuronMesh * NeuronMeshPtr;
  typedef std::unordered_map< nsol::NeuronMorphologyPtr, NeuronMeshPtr >
  TMorphoMap;

  class Morphologies
  {

  public:

    NLRENDER_API
    Morphologies( void );

    NLRENDER_API
    ~Morphologies( void );

    NLRENDER_API
    void Add( nsol::NeuronMorphologyPtr morpho,
              NeuronMeshPtr neuronMesh);

    NLRENDER_API
    bool Exist( nsol::NeuronMorphologyPtr morpho );

    NLRENDER_API
    NeuronMeshPtr GetNeuronMesh( nsol::NeuronMorphologyPtr morpho );

    NLRENDER_API
    TMorphoMap::iterator Begin( void );

    NLRENDER_API
    TMorphoMap::iterator End( void );

  private:

    std::unordered_map < nsol::NeuronMorphologyPtr, NeuronMeshPtr >
    _morphoMap;

  };

} // end namespace nlrender

#endif // __NLRENDER_MORPHOLOGIES__

//EOF
