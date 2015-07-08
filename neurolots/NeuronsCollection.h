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


#include "Program.h"
#include "Camera.h"

#include <nsol/nsol.h>

#include <Eigen/Dense>

#include <string>

#include <set>

#ifdef NEUROLOTS_WITH_ZEQ

#include <zeq/zeq.h>
#include <zeq/hbp/hbp.h>
#include <servus/uri.h>

#include <pthread.h>
#include <mutex>
#include <boost/bind.hpp>

#endif



namespace neurolots
{

  typedef nsol::Columns* ColumnsPtr;

  class NeuronsCollection
  {

    public:

      NeuronsCollection( const std::string& fileName, Camera* camera_ );

#ifdef NEUROLOTS_WITH_ZEQ

      NeuronsCollection( const std::string& uri_, const std::string& filName,
          Camera* camera_ );

#endif

      ~NeuronsCollection( void );


      void Paint( void );

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

      void NeuronColor( Eigen::Vector3f neuronColor_ );
      void SelectedNeuronColor( Eigen::Vector3f selectedNeuronColor_ );


    private:

      void _Init( void );

      void _GenerateMeshes( void );

      bool _IsSelected( nsol::NeuronPtr neuron_ );

      void _DefaultCamera( void );

#ifdef NEUROLOTS_WITH_ZEQ

      void _OnFocusEvent( const zeq::Event& event_ );
      void _OnSelectionEvent( const zeq::Event& event_ );
      void _OnSelectionFocusEvent( const zeq::Event& event_ );
      static void* _Subscriber( void* collection_ );

#endif

      Program* _programTriangles;
      Program* _programQuads;
      Camera* _camera;

      float _lod;
      float _tng;
      float _maxDist;

      std::vector< float > _neuronColor;
      std::vector< float > _selectedNeuronColor;

      nsol::Columns _columns;

      unsigned int _cont;

      bool _zeqConnection;

      std::set<unsigned int> _selectedNeurons;

#ifdef NEUROLOTS_WITH_ZEQ

      servus::URI _uri;
      zeq::Subscriber* _subscriber;

      pthread_t _subscriberThread;

#endif
      Eigen::Vector3f _defaultPivot;
      float _defaultRadius;
  };

} // end namespace neurolots

#endif // __NEUROLOTS_NEURONS_COLLECTION__

//EOF
