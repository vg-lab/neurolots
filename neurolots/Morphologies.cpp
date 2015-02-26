#include "Morphologies.h"

using namespace std;
using namespace nsol;

namespace neurolots
{

  Morphologies::Morphologies( void )
  {
  }

  Morphologies::~Morphologies( void )
  {
  }

  void Morphologies::Add( NeuronMorphologyPtr morpho,
                                    NeuronMeshPtr neuronMesh )
  {
    _morphoMap.insert(map< NeuronMorphologyPtr, NeuronMeshPtr >
                      ::value_type(morpho,neuronMesh));

  }

  bool Morphologies::Exist( NeuronMorphologyPtr morpho )
  {
    return !( _morphoMap.find(morpho) ==  _morphoMap.end( ));
  }

  NeuronMeshPtr Morphologies::GetNeuronMesh( nsol::NeuronMorphologyPtr morpho )
  {
    map< NeuronMorphologyPtr, NeuronMeshPtr >::iterator itt;
    itt = _morphoMap.find( morpho );

    if( itt == _morphoMap.end( ))
      return nullptr;
    else
      return itt->second;
  }

  TMorphoMap::iterator Morphologies::Begin( void )
  {
    return _morphoMap.begin( );
  }

  TMorphoMap::iterator Morphologies::End( void )
  {
    return _morphoMap.end( );
  }

} // end namespace neurolots

// EOF
