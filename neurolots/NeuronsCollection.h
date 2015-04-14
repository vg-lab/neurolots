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

  typedef std::map<unsigned int, nsol::ColumnPtr>* ColumnsPtr;

  class NeuronsCollection
  {

    public:

      NeuronsCollection( const char * file_name, Program * programTriangles_,
                         Program * programQuads_, Camera * camera_ );

      NeuronsCollection( const char * file_name, const char * quadsPath,
                         const char * trianglesPath , Camera * camera_ );
      ~NeuronsCollection( void );



      void Paint( void );

      void PaintMiniColum( unsigned int nColumn, unsigned int nMiniColumn );
      void PaintNeuron( unsigned int nColumn, unsigned int nMiniColumn,
                        unsigned int nNeuron );
      void PaintNeuron( unsigned int nColumn, unsigned int nMiniColumn,
                        unsigned int nNeuron, float x, float y, float z );

      void AddLod( float AddLod );
      void AddTng( float AddTng );
      void AddMaxDist( float AddMaxDist );

      //Getters

      ColumnsPtr getColumns( void );

      //Setters

      void PaintSoma( bool paintSoma );
      void PaintNeurites( bool paintNeurites);

      void Lod( float lod_ );
      void Tng( float tng_ );
      void MaxDist( float maxDist_ );

      void NeuritesColor( Eigen::Vector3f neuritesColor_ );
      void SomaColor( Eigen::Vector3f somaColor_ );

      void NeuronColor( Eigen::Vector3f neuronColor_ );



    private:

      void _Init( void );

      void _GenerateMeshes( void );
      void _GenerateMeshes( unsigned int nColumn, unsigned int nMiniColumn,
                            unsigned int nNeuron  );

      Program * _programTriangles;
      Program * _programQuads;
      Camera * _camera;

      float _lod;
      float _tng;
      float _maxDist;

      std::vector< float > _neuritesColor;
      std::vector< float > _somaColor;

      std::map<unsigned int, nsol::ColumnPtr> _colums;

      unsigned int _cont;
  };

} // end namespace neurolots

#endif // __NEUROLOTS_NEURONS_COLLECTION__

//EOF
