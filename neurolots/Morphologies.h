/**
 * @file    Morphologies.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_MORPHOLOGIES__
#define __NEUROLOTS_MORPHOLOGIES__

#include "NeuronMesh.h"

#include <nsol/nsol.h>

#include <iostream>
#include <unordered_map>


namespace neurolots
{
  typedef NeuronMesh * NeuronMeshPtr;
  typedef std::unordered_map< nsol::NeuronMorphologyPtr, NeuronMeshPtr > TMorphoMap;

  class Morphologies
  {

    public:

      Morphologies( void );
      ~Morphologies( void );

      void Add( nsol::NeuronMorphologyPtr morpho,
                          NeuronMeshPtr neuronMesh);
      bool Exist( nsol::NeuronMorphologyPtr morpho );

      NeuronMeshPtr GetNeuronMesh( nsol::NeuronMorphologyPtr morpho );


      TMorphoMap::iterator Begin( void );
      TMorphoMap::iterator End( void );

    private:

      std::unordered_map < nsol::NeuronMorphologyPtr, NeuronMeshPtr > _morphoMap;

  };

} // end namespace neurolots

#endif // __NEUROLOTS_MORPHOLOGIES__

//EOF
