/**
 * @file    NeuronMorphology.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLRENDER_NEURON_MORPHOLOGY__
#define __NLRENDER_NEURON_MORPHOLOGY__

#include "NeuronMesh.h"

#include <nsol/nsol.h>

#include <iostream>

#include <nlrender/api.h>

namespace nlrender
{

  class NeuronMorphology : public nsol::NeuronMorphology
  {

  public:

    NLRENDER_API
    NeuronMorphology( void );

    NLRENDER_API
    NeuronMorphology( nsol::SomaPtr soma );

    NLRENDER_API
    ~NeuronMorphology( void );

    //Getters
    NLRENDER_API
    NeuronMeshPtr NeuronMesh( void );

    NLRENDER_API
    bool HasNeuronMesh( void );

    //Setters
    NLRENDER_API
    void NeuronMesh( NeuronMeshPtr neuronMesh_ );


  private:

    NeuronMeshPtr _neuronMesh;
    bool _meshGenerated;
  };

  typedef NeuronMorphology * NeuronMorphologyPtr;

} // end namespace nlrender

#endif // __NLRENDER_NEURON_MORPHOLOGY__

//EOF
