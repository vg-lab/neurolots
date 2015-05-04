#include "NeuronMorphology.h"

namespace neurolots
{

  NeuronMorphology::NeuronMorphology( nsol::SomaPtr soma_ )
    : nsol::NeuronMorphology( soma_ )
    , _neuronMesh( nullptr )
    , _meshGenerated( false )
  {

  }

  NeuronMorphology::~NeuronMorphology( void )
  {

  }

  NeuronMeshPtr NeuronMorphology::NeuronMesh( void )
  {
    return _neuronMesh;
  }

  bool NeuronMorphology::HasNeuronMesh( void )
  {
    return _meshGenerated;
  }

  void NeuronMorphology::NeuronMesh( NeuronMeshPtr neuronMesh_ )
  {
    _neuronMesh = neuronMesh_;
    _meshGenerated = true;
  }



} // end namespace neurolots

// EOF

