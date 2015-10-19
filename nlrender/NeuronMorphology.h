#ifndef __NEUROLOTS_NEURON_MORPHOLOGY__
#define __NEUROLOTS_NEURON_MORPHOLOGY__

#include "NeuronMesh.h"

#include <nsol/nsol.h>

#include <iostream>

#include <neurolots/nlrender/api.h>

namespace neurolots
{

  class NeuronMorphology : public nsol::NeuronMorphology
  {

  public:

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

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MORPHOLOGY__

//EOF
