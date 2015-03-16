#ifndef __NEUROLOTS_NEURON_MORPHOLOGY__
#define __NEUROLOTS_NEURON_MORPHOLOGY__

#include "NeuronMesh.h"

#include <nsol/nsol.h>

#include <iostream>


namespace neurolots
{

  class NeuronMorphology : public nsol::NeuronMorphology
  {

    public:

      NeuronMorphology( nsol::SomaPtr soma );
      ~NeuronMorphology( void );

      //Getters
      NeuronMeshPtr NeuronMesh( void );
      bool HasNeuronMesh( void );

      //Setters
      void NeuronMesh( NeuronMeshPtr neuronMesh_ );


    private:

      NeuronMeshPtr _neuronMesh;
      bool _meshGenerated;
  };

  typedef NeuronMorphology * NeuronMorphologyPtr;

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MORPHOLOGY__

//EOF
