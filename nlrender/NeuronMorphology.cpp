/**
 * @file    NeuronMorphology.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "NeuronMorphology.h"

namespace nlrender
{

  NeuronMorphology::NeuronMorphology( void )
    : nsol::NeuronMorphology( )
    , _neuronMesh( nullptr )
  {

  }

  NeuronMorphology::NeuronMorphology( nsol::SomaPtr soma_ )
    : nsol::NeuronMorphology( soma_ )
    , _neuronMesh( nullptr )
  {

  }

  NeuronMorphology::~NeuronMorphology( void )
  {
    if ( _neuronMesh )
      delete _neuronMesh;
  }

  NeuronMeshPtr NeuronMorphology::NeuronMesh( void )
  {
    return _neuronMesh;
  }

  bool NeuronMorphology::HasNeuronMesh( void )
  {
    return _neuronMesh;
  }

  void NeuronMorphology::NeuronMesh( NeuronMeshPtr neuronMesh_ )
  {
    _neuronMesh = neuronMesh_;
  }



} // end namespace nlrender

// EOF
