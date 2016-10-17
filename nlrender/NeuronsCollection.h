/**
 * @file    NeuronsCollection.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLRENDER_NEURONS_COLLECTION__
#define __NLRENDER_NEURONS_COLLECTION__


#include "Program.h"
#include "Camera.h"

#include <nsol/nsol.h>
#include <nsol/DataSet.h>

#include <Eigen/Dense>

#include <string>

#include <set>

#ifdef NEUROLOTS_USE_ZEROEQ

#include <zeroeq/zeroeq.h>
#include <thread>

#ifdef NEUROLOTS_USE_LEXIS

#include <lexis/lexis.h>

#ifdef NEUROLOTS_USE_GMRVLEX

#include <gmrvlex/gmrvlex.h>

#endif

#endif

#include <pthread.h>
#include <mutex>
#include <boost/bind.hpp>

#endif

#include <nlrender/api.h>
#include "Neuron.h"
#include "../nlgeometry/Facet.h"

namespace nlrender
{

  typedef nsol::Columns* ColumnsPtr;

  class NeuronsCollection
  {

  public:

    typedef enum
    {
      HOMOGENEOUS = 0,
      LINEAR
    }TTessMethod;

    NLRENDER_API
    NeuronsCollection( Camera* camera_ );

    NLRENDER_API
    ~NeuronsCollection( void );

    NLRENDER_API
    void loadBlueConfig( const std::string& blueConfig_,
                         const std::string& target_ = std::string( "" ),

                         int loadFlags_ = nsol::MORPHOLOGY |
                         nsol::CORTICAL_HIERARCHY );

    NLRENDER_API
    void loadSwc( const std::string& swcFile_ );

    NLRENDER_API
    void loadScene( const std::string& xmlFile_ );

    NLRENDER_API
    void setZeqUri( const std::string& uri_ );

    NLRENDER_API
    void paint( void );

    NLRENDER_API
    void paintNeuron( const unsigned int& id_ );

    NLRENDER_API
    void paintNeuron( const NeuronPtr& neuron );

    NLRENDER_API
    void focusOnNeuron( unsigned int id );

    NLRENDER_API
    void focusOnNeuron( NeuronPtr neuron_ );

    NLRENDER_API
    void focusAll( void );

    NLRENDER_API
    void extractMesh( NeuronPtr neuron_,
                      const std::string& outFileName = std::string("out.obj"));

    //Getters

    NLRENDER_API
    Eigen::Vector3f neuronColor( void );

    NLRENDER_API
    Eigen::Vector3f selectedNeuronColor( void );

    NLRENDER_API
    ColumnsPtr columns( void );

    NLRENDER_API
    nsol::DataSet & DataSet( void );

    NLRENDER_API
    const nsol::DataSet & DataSet( void ) const;

    NLRENDER_API
    std::vector< unsigned int > neuronIDs( void );

    NLRENDER_API
    NeuronPtr neuronById( unsigned int id );

#ifdef NEUROLOTS_USE_ZEROEQ

    NLRENDER_API
    zeroeq::Subscriber* Subscriber( void );

#endif

    NLRENDER_API
    bool selectionChange( void );

    //Setters

    NLRENDER_API
    void lod( float lod_ );

    NLRENDER_API
    void tng( float tng_ );

    NLRENDER_API
    void maxDist( float maxDist_ );

    NLRENDER_API
    void neuronColor( const Eigen::Vector3f& neuronColor_ );

    NLRENDER_API
    void selectedNeuronColor( const Eigen::Vector3f& selectedNeuronColor_ );

    NLRENDER_API
    void paintSoma( bool paintSoma_ );

    NLRENDER_API
    void paintNeurites( bool paintNeurites_ );

    NLRENDER_API
    void paintSelectedSoma( bool paintSelectedSoma_ );

    NLRENDER_API
    void paintSelectedNeurites( bool paintSelectedNeurites_ );

    NLRENDER_API
    void tessMethod( const TTessMethod& tessMethod_ )
    {
      _tessMethod = ( unsigned int ) tessMethod_;
    }

  private:

    void _init( void );

    bool _isSelected( nsol::NeuronPtr neuron_ );

    void _defaultCamera( void );

    void _vectorToMesh( const std::vector< float >& vecVertices_,
                        const std::vector< float >& vecNormals_,
                        nlgeometry::Vertices& vertices_,
                        nlgeometry::Facets& facets_ ) const;

#ifdef NEUROLOTS_USE_ZEROEQ

#ifdef NEUROLOTS_USE_GMRVLEX
    void _onFocusEvent( zeroeq::gmrv::ConstFocusedIDsPtr event_ );
#endif

#ifdef NEUROLOTS_USE_LEXIS
    void _onSelectionEvent( lexis::data::ConstSelectedIDsPtr event_ );
    void _onSelectionFocusEvent( lexis::data::ConstSelectedIDsPtr event_ );
#endif
#endif

    Program* _programTriangles;
    Program* _programQuads;

    Program* _programTrianglesFB;
    Program* _programQuadsFB;

    unsigned int _tfo;
    std::vector< unsigned int > _tbos;

    Camera* _camera;

    float _lod;
    float _tng;
    float _maxDist;

    std::vector< float > _neuronColor;
    std::vector< float > _selectedNeuronColor;

    bool _paintSoma;
    bool _paintNeurites;
    bool _paintSelectedSoma;
    bool _paintSelectedNeurites;

    nsol::DataSet _dataSet;

    std::set<unsigned int> _selectedNeurons;

    bool _selectionChange;

#ifdef NEUROLOTS_USE_ZEROEQ
    bool _zeqConnection;

    std::string _zeroeqSession;
    zeroeq::Subscriber* _subscriber;

    std::thread* _subscriberThread;
#endif

    Eigen::Vector3f _defaultPivot;
    float _defaultRadius;

    unsigned int _tessMethod;
  };

} // end namespace nlrender

#endif // __NLRENDER_NEURONS_COLLECTION__

//EOF
