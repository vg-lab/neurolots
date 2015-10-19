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
#include <nsol/DataSet.h>

#include <Eigen/Dense>

#include <string>

#include <set>

#ifdef NEUROLOTS_USE_ZEQ

#include <zeq/zeq.h>
#include <zeq/hbp/hbp.h>
#include <servus/uri.h>

#include <pthread.h>
#include <mutex>
#include <boost/bind.hpp>

#endif

#include <neurolots/nlrender/api.h>

namespace neurolots
{

  typedef nsol::Columns* ColumnsPtr;

  class NeuronsCollection
  {

  public:

    NLRENDER_API
    NeuronsCollection( Camera* camera_ );

    NLRENDER_API
    ~NeuronsCollection( void );

    NLRENDER_API
    void loadBlueConfig( const std::string& blueConfig_,
                         const std::string& target_ = std::string( "" ));

    NLRENDER_API
    void loadSwc( const std::string& swcFile_ );

    NLRENDER_API
    void loadScene( const std::string& xmlFile_ );

    NLRENDER_API
    void setZeqUri( const std::string& uri_ );

    NLRENDER_API
    void Paint( void );

    NLRENDER_API
    void AddLod( float AddLod );

    NLRENDER_API
    void AddTng( float AddTng );

    NLRENDER_API
    void AddMaxDist( float AddMaxDist );

    //Getters
    NLRENDER_API
    ColumnsPtr Columns( void );

#ifdef NEUROLOTS_USE_ZEQ

    NLRENDER_API
    zeq::Subscriber* Subscriber( void );

#endif

    //Setters

    NLRENDER_API
    void Lod( float lod_ );

    NLRENDER_API
    void Tng( float tng_ );

    NLRENDER_API
    void MaxDist( float maxDist_ );

    NLRENDER_API
    void NeuronColor( Eigen::Vector3f neuronColor_ );

    NLRENDER_API
    void SelectedNeuronColor( Eigen::Vector3f selectedNeuronColor_ );


    private:

    void _Init( void );

    void _GenerateMeshes( void );

    bool _IsSelected( nsol::NeuronPtr neuron_ );

    void _DefaultCamera( void );

#ifdef NEUROLOTS_USE_ZEQ

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

    nsol::DataSet _dataSet;
    unsigned int _cont;

    std::set<unsigned int> _selectedNeurons;

#ifdef NEUROLOTS_USE_ZEQ
    bool _zeqConnection;

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