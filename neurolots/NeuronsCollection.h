/**
 * @file    NeuronsCollection.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_NEURONS_COLLECTION__
#define __NEUROLOTS_NEURONS_COLLECTION__

#include "NeuronMorphology.h"
#include "NeuronMesh.h"
#include "Program.h"
#include "Camera.h"

#include <nsol/nsol.h>

#include <Eigen/Dense>

#include <iostream>

namespace neurolots
{
  class NeuronsCollection
  {

    public:

      NeuronsCollection( const char * file_name, Program * program_,
                         Camera * camera_ );
      ~NeuronsCollection( void );

      void Init( void );
      void Paint( void );
      void PaintMiniColum( unsigned int nColumn, unsigned int nMiniColumn );
      void PaintNeuron( unsigned int nColumn, unsigned int nMiniColumn,
                        unsigned int nNeuron );
      void PaintNeuron( unsigned int nColumn, unsigned int nMiniColumn,
                        unsigned int nNeuron, float x, float y, float z );
      void Anim( void );

    private:

      void _GenerateMeshes( void );
      void _GenerateMeshes( unsigned int nColumn, unsigned int nMiniColumn,
                            unsigned int nNeuron  );

      Program * _program;
      Camera * _camera;

      std::map<unsigned int, nsol::ColumnPtr> _colums;

      int _cont;
  };

} // end namespace neurolots

#endif // __NEUROLOTS_NEURONS_COLLECTION__

//EOF
