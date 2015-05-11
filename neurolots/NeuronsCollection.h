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
#include <cstdlib>
#include <cstring>

#ifdef NEUROLOTS_WITH_ZEQ
#include <zeq/zeq.h>
#include <zeq/hbp/hbp.h>
#include <lunchbox/uri.h>

#include <pthread.h>
#include <mutex>
#include <boost/bind.hpp>
#endif

namespace neurolots
{

  typedef std::map<unsigned int, nsol::ColumnPtr>* ColumnsPtr;

  class NeuronsCollection
  {

    public:
      NeuronsCollection( const char * file_name, Camera * camera_ );

#ifdef NEUROLOTS_WITH_ZEQ
      NeuronsCollection( const char* uri_, const char * file_name,
          Camera * camera_ );
#endif

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

      ColumnsPtr Columns( void );

#ifdef NEUROLOTS_WITH_ZEQ
      zeq::Subscriber* Subscriber( void );
#endif

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

      bool _IsSelected( nsol::NeuronPtr neuron_ );

#ifdef NEUROLOTS_WITH_ZEQ
      void _OnSelectionEvent( const zeq::Event& event_ );
      static void* _Subscriber( void* collection_ );
#endif

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

      bool _zeqConnection;

      std::set<unsigned int> _selectedNeurons;

#ifdef NEUROLOTS_WITH_ZEQ
      lunchbox::URI _uri;
      zeq::Subscriber* _subscriber;

      pthread_t _subscriberThread;
#endif

  };

} // end namespace neurolots

#endif // __NEUROLOTS_NEURONS_COLLECTION__

//EOF
