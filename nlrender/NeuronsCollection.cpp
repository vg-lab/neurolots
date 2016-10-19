/**
 * @file    NeuronsCollection.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "NeuronsCollection.h"

#include "NeuronMorphology.h"
#include "NeuronMesh.h"

#include <cfloat>
#include <iostream>

#include "../nlgeometry/SpatialHashTable.h"

namespace nlrender
{

  NeuronsCollection::NeuronsCollection( Camera* camera_ )
    : _camera( camera_ )
    , _paintSoma( true )
    , _paintNeurites( true )
    , _paintSelectedSoma( true )
    , _paintSelectedNeurites( true )
    , _dataSet( )
    , _selectionChange( false )
#ifdef NEUROLOTS_USE_ZEROEQ
    , _zeqConnection( false )
#endif
    , _tessMethod( HOMOGENEOUS )
  {
    _selectedNeurons.clear( );

    _programQuads = new Program( Program::QUADS );
    _programQuadsFB = new Program( Program::QUADS_FB );
    _programTriangles = new Program( Program::TRIANGLES );
    _programTrianglesFB = new Program( Program::TRIANGLES_FB );

    _programQuads->init();
    _programQuadsFB->init( );
    _programTriangles->init( );
    _programTrianglesFB->init( );

    lod( 3.0f );
    tng( 0.5f );
    maxDist( 0.1f );

    neuronColor( Eigen::Vector3f( 0.0f, 0.5f, 0.7f ));
    selectedNeuronColor( Eigen::Vector3f( 0.7f, 0.5f, 0.0f ));

    //Construcction of trasnform feeback buffers and object

    _tbos.resize( 2 );
    glGenBuffers( 2, _tbos.data( ));

    glGenTransformFeedbacks( 1, &_tfo );
    glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, _tfo );

    glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, _tbos[0] );
    glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 1, _tbos[1] );

    glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
  }

  NeuronsCollection::~NeuronsCollection( void )
  {

  }

  // PUBLIC

  void NeuronsCollection::loadBlueConfig(
#ifdef NSOL_USE_BRION
                                          const std::string& blueConfig_,
                                          const std::string& target_,
                                          int loadFlags_
#else
                                          const std::string& /*blueConfig_*/,
                                          const std::string& /*target_*/,
                                          int /*loadFlags_*/
#endif
    )
  {
    _dataSet.close( );
#ifdef NSOL_USE_BRION
    loadFlags_ |= nsol::MORPHOLOGY | nsol::CORTICAL_HIERARCHY;
    try{
        _dataSet.loadBlueConfigHierarchy< nsol::Node,
                                          nsol::Section,
                                          nsol::Dendrite,
                                          nsol::Axon,
                                          nsol::Soma,
                                          NeuronMorphology,
                                          Neuron,
                                          nsol::MiniColumn,
                                          nsol::Column >( blueConfig_,
                                                          target_ );

        _dataSet.loadAllMorphologies< nsol::Node,
                                      nsol::Section,
                                      nsol::Dendrite,
                                      nsol::Axon,
                                      nsol::Soma,
                                      NeuronMorphology,
                                      Neuron,
                                      nsol::MiniColumn,
                                      nsol::Column >( );
        _init( );

        _defaultCamera( );
        _camera->Pivot( _defaultPivot );
        _camera->Radius( _defaultRadius );
      }
      catch( ... )
      {
        std::cerr << "Error: can't load file: " << blueConfig_ << std::endl;
        exit(-1);
      }
#else
    std::cerr << "Error: Brion support not built-in" << std::endl;
    exit( -1 );
#endif
  }

  void NeuronsCollection::loadSwc( const std::string& swcFile_ )
  {
    _dataSet.close( );
    _dataSet.loadNeuronFromFile< nsol::Node,
                                nsol::Section,
                                nsol::Dendrite,
                                nsol::Axon,
                                nsol::Soma,
                                NeuronMorphology,
                                Neuron >( swcFile_, 1 );
    _init( );

    _defaultCamera( );
    _camera->Pivot( _defaultPivot );
    _camera->Radius( _defaultRadius );
  }

  void NeuronsCollection::loadScene( const std::string& xmlFile_ )
  {
    _dataSet.close( );
    _dataSet.loadXmlScene< nsol::Node,
                           nsol::Section,
                           nsol::Dendrite,
                           nsol::Axon,
                           nsol::Soma,
                           NeuronMorphology,
                           Neuron >( xmlFile_ );
    _init( );

    _defaultCamera( );
    _camera->Pivot( _defaultPivot );
    _camera->Radius( _defaultRadius );
  }

  void NeuronsCollection::setZeqUri( const std::string&
#ifdef NEUROLOTS_USE_ZEROEQ
                                     session
#endif
    )
  {
#ifdef NEUROLOTS_USE_ZEROEQ

    _zeqConnection = true;
    _zeroeqSession =  session.empty( ) ? zeroeq::DEFAULT_SESSION : session ;
    _subscriber = new zeroeq::Subscriber( _zeroeqSession );

    _subscriber->subscribe(
        lexis::data::SelectedIDs::ZEROBUF_TYPE_IDENTIFIER( ),
        [&]( const void* data, size_t size )
        { _onSelectionEvent( lexis::data::SelectedIDs::create( data, size ));});

#ifdef NEUROLOTS_USE_GMRVLEX

    _subscriber->subscribe(
        zeroeq::gmrv::FocusedIDs::ZEROBUF_TYPE_IDENTIFIER( ),
        [&]( const void* data, size_t size )
        { _onFocusEvent( zeroeq::gmrv::FocusedIDs::create( data, size ));});

#endif

    _subscriberThread =
        new std::thread( [&](){ while ( true ) _subscriber->receive( 10000 ); });

#endif
  }

  void NeuronsCollection::paint( void )
  {
    NeuronPtr neuron;
    NeuronMorphologyPtr morpho;
    NeuronMeshPtr neuronMesh;

    _programQuads->projectionMatrix( _camera->ProjectionMatrix( ));
    _programQuads->viewMatrix( _camera->ViewMatrix( ));
    _programQuads->cameraPosition( _camera->Position( ));

    _programTriangles->projectionMatrix( _camera->ProjectionMatrix( ));
    _programTriangles->viewMatrix( _camera->ViewMatrix( ));
    _programTriangles->cameraPosition( _camera->Position( ));



    std::vector< float > color;
    bool pSoma = false;
    bool pNeurites = false;

    for ( const auto& element: _dataSet.neurons( ))
    {
      neuron = dynamic_cast< NeuronPtr >( element.second );
      morpho = dynamic_cast< NeuronMorphologyPtr >( neuron->morphology( ));
      if ( !morpho )
        continue;
      neuronMesh = morpho->NeuronMesh( );
      if( !neuronMesh )
        continue;

#ifdef NEUROLOTS_USE_ZEROEQ
      if( _zeqConnection )
      {
        if( _isSelected( neuron ) )
        {
          color = _selectedNeuronColor;
          pSoma = _paintSelectedSoma;
          pNeurites = _paintSelectedNeurites;

        }
        else
        {
          color = _neuronColor;
          pSoma = _paintSoma;
          pNeurites = _paintNeurites;
        }
      }
      else
      {
        color = _neuronColor;
        pSoma = _paintSoma;
        pNeurites = _paintNeurites;
      }
#else
      color = _neuronColor;
      pSoma = _paintSoma;
      pNeurites = _paintNeurites;
#endif

      if( pSoma )
      {
        _programTriangles->modelMatrix( neuron->vecTransform( ).data( ));
        _programTriangles->color( color.data( ));
        _programTriangles->tessellationDistanceFunc( &_tessMethod );
        neuronMesh->paintSoma( );
      }
      if( pNeurites )
      {
        _programQuads->modelMatrix( neuron->vecTransform( ).data( ));
        _programQuads->color( color.data( ));
        _programQuads->tessellationDistanceFunc( &_tessMethod );
        neuronMesh->paintNeurites( );
      }
    }
  }

  void NeuronsCollection::paintNeuron( const unsigned int& id_ )
  {
    NeuronPtr neuron = nullptr;
    nsol::NeuronsMap::iterator it;
    it = _dataSet.neurons( ).find( id_ );
    if ( it != _dataSet.neurons( ).end( ))
    {
      neuron = dynamic_cast< NeuronPtr >( it->second );
      paintNeuron( neuron );
    }
  }

  void NeuronsCollection::paintNeuron( const NeuronPtr& neuron )
  {
    NeuronMeshPtr neuronMesh = nullptr;
    NeuronMorphologyPtr morpho = nullptr;
    if ( !neuron )
      return;
    morpho = dynamic_cast< NeuronMorphologyPtr >( neuron->morphology( ));
    if( !morpho )
      return;
    neuronMesh = morpho->NeuronMesh( );
    if( !neuronMesh )
      return;

    if( _paintSoma )
    {
      _programTriangles->projectionMatrix( _camera->ProjectionMatrix( ));
      _programTriangles->viewMatrix( _camera->ViewMatrix( ));
      _programTriangles->modelMatrix( neuron->vecTransform( ).data( ));
      _programTriangles->color( _neuronColor.data( ));
      _programTriangles->cameraPosition( _camera->Position( ));
      _programTriangles->levelOfTessellation( &_lod );
      _programTriangles->alphaTangentModule( &_tng );
      _programTriangles->maximumDistance( &_maxDist);
      _programTriangles->tessellationDistanceFunc( &_tessMethod );
      neuronMesh->paintSoma( );
    }
    if( _paintNeurites )
    {
      _programQuads->projectionMatrix( _camera->ProjectionMatrix( ));
      _programQuads->viewMatrix( _camera->ViewMatrix( ));
      _programQuads->modelMatrix( neuron->vecTransform( ).data( ));
      _programQuads->color( _neuronColor.data( ));
      _programQuads->cameraPosition( _camera->Position( ));
      _programQuads->levelOfTessellation( &_lod );
      _programQuads->alphaTangentModule( &_tng );
      _programQuads->maximumDistance( &_maxDist);
      _programQuads->tessellationDistanceFunc( &_tessMethod );
      neuronMesh->paintNeurites( );
    }
  }

  void NeuronsCollection::focusOnNeuron( unsigned int id )
  {
    NeuronPtr neuron = nullptr;
    nsol::NeuronsMap::iterator it;

    it = _dataSet.neurons( ).find( id );
    if( it != _dataSet.neurons( ).end( ))
    {
      neuron = dynamic_cast< NeuronPtr >( it->second );
    }

    if( neuron )
    {
      TBoundingBox bb = neuron->BoundingBox( );
      Eigen::Vector3f center = Eigen::Vector3f(( bb.xMin + bb.xMax ) / 2,
                                               ( bb.yMin + bb.yMax ) / 2,
                                               ( bb.zMin + bb.zMax ) / 2 );
      Eigen::Vector3f corner( bb.xMin, bb.yMin, bb.zMin );

      float radius = ( center - corner ).norm( ) /  sin( _camera->Fov( ));
      _camera->TargetPivotRadius( center, radius );
    }
  }

  void NeuronsCollection::focusOnNeuron( NeuronPtr neuron_ )
  {
    if ( !neuron_ )
      return;
    TBoundingBox bb = neuron_->BoundingBox( );
    Eigen::Vector3f center = Eigen::Vector3f(( bb.xMin + bb.xMax ) / 2,
                                             ( bb.yMin + bb.yMax ) / 2,
                                             ( bb.zMin + bb.zMax ) / 2 );
    Eigen::Vector3f corner( bb.xMin, bb.yMin, bb.zMin );

    float radius = ( center - corner ).norm( ) /  sin( _camera->Fov( ));
    _camera->TargetPivotRadius( center, radius );
  }

  void NeuronsCollection::focusAll( void )
  {
    _camera->TargetPivotRadius( _defaultPivot, _defaultRadius );
  }

  void NeuronsCollection::extractMesh(
    NeuronPtr neuron_,
    const std::string& outFileName )
  {
    NeuronMeshPtr neuronMesh;
    if ( !neuron_ )
      return;
    neuronMesh =
      dynamic_cast< NeuronMorphologyPtr >(
        neuron_->morphology( ))->NeuronMesh( );
    if( !neuronMesh )
      return;

    _programTrianglesFB->projectionMatrix( _camera->ProjectionMatrix( ));
    _programTrianglesFB->viewMatrix( _camera->ViewMatrix( ));
    _programTrianglesFB->modelMatrix( neuron_->vecTransform( ).data( ));
    _programTrianglesFB->cameraPosition( _camera->Position( ));
    _programTrianglesFB->levelOfTessellation( &_lod );
    _programTrianglesFB->alphaTangentModule( &_tng );
    _programTrianglesFB->maximumDistance( &_maxDist);

    _programQuadsFB->projectionMatrix( _camera->ProjectionMatrix( ));
    _programQuadsFB->viewMatrix( _camera->ViewMatrix( ));
    _programQuadsFB->modelMatrix( neuron_->vecTransform( ).data( ));
    _programQuadsFB->cameraPosition( _camera->Position( ));
    _programQuadsFB->levelOfTessellation( &_lod );
    _programQuadsFB->alphaTangentModule( &_tng );
    _programQuadsFB->maximumDistance( &_maxDist);

    glDisable( GL_CULL_FACE );
    glEnable( GL_RASTERIZER_DISCARD );

    unsigned int query;
    unsigned int numPrimitives;
    unsigned int trianglesSize;
    unsigned int quadsSize;
    unsigned int size;

    std::vector< float > _backVertices;
    std::vector< float > _backNormals;

    glGenQueries( 1, &query );

// Triangles

    glBeginQuery( GL_PRIMITIVES_GENERATED, query );

    _programTrianglesFB->tessellationDistanceFunc( &_tessMethod );
    neuronMesh->paintSoma( );

    glEndQuery( GL_PRIMITIVES_GENERATED );
    glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );

    trianglesSize = numPrimitives * 9;

// Transform feedback buffers allocate

    glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * trianglesSize, nullptr,
                  GL_STATIC_READ );
    glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * trianglesSize, nullptr,
                  GL_STATIC_READ );


// Transform feedback
    glBeginQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query );

    glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, _tfo );
    glBeginTransformFeedback( GL_TRIANGLES );

    _programTrianglesFB->tessellationDistanceFunc( &_tessMethod );
    neuronMesh->paintSoma( );

    glEndTransformFeedback( );
    glFlush( );

    glEndQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN );
    glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );

    trianglesSize = numPrimitives * 9;

    glBindVertexArray( 0 );
    glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );

// Gpu data dump to cpu
    _backVertices.resize( trianglesSize );
    _backNormals.resize( trianglesSize );

    glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
    glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * trianglesSize,
                         _backVertices.data( ));
    glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
    glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * trianglesSize,
                         _backNormals.data( ));

// Quads
    glBeginQuery( GL_PRIMITIVES_GENERATED, query );

    _programQuadsFB->tessellationDistanceFunc( &_tessMethod );
    neuronMesh->paintNeurites( );

    glEndQuery( GL_PRIMITIVES_GENERATED );
    glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );

    quadsSize = numPrimitives * 9;

// Transform feedback buffers allocate

    glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * quadsSize, nullptr,
                  GL_STATIC_READ );
    glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * quadsSize, nullptr,
                  GL_STATIC_READ );

//Transform feedback
    glBeginQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query );

    glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, _tfo );
    glBeginTransformFeedback( GL_TRIANGLES );

    _programQuadsFB->tessellationDistanceFunc( &_tessMethod );
    neuronMesh->paintNeurites( );

    glEndTransformFeedback( );
    glFlush( );

    glEndQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN );
    glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );

    quadsSize = numPrimitives * 9;

    glBindVertexArray( 0 );
    glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );

    glDisable( GL_RASTERIZER_DISCARD );

// Gpu data dump to cpu
    size = trianglesSize + quadsSize;
    _backVertices.resize( size );
    _backNormals.resize( size );

    glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
    glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * quadsSize,
                        &_backVertices[trianglesSize] );
    glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
    glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * quadsSize,
                        &_backNormals[trianglesSize] );

    nlgeometry::Vertices vertices;
    nlgeometry::Facets facets;

    _vectorToMesh( _backVertices, _backNormals, vertices, facets );

    _backVertices.clear( );
    _backNormals.clear( );
    neuronMesh->writeOBJ( outFileName, vertices, facets );

  }

  // GETTERS

  Eigen::Vector3f NeuronsCollection::neuronColor( void )
  {
    Eigen::Vector3f color( _neuronColor[0], _neuronColor[1], _neuronColor[2] );
    return color;
  }

  Eigen::Vector3f NeuronsCollection::selectedNeuronColor( void )
  {
    Eigen::Vector3f color( _selectedNeuronColor[0], _selectedNeuronColor[1],
                           _selectedNeuronColor[2] );
    return color;
  }

  ColumnsPtr NeuronsCollection::columns( void )
  {
    return &_dataSet.columns( );
  }

  nsol::DataSet &NeuronsCollection::DataSet( void )
  {
    return _dataSet;
  }

  const nsol::DataSet &NeuronsCollection::DataSet( void ) const
  {
    return _dataSet;
  }

  std::vector< unsigned int > NeuronsCollection::neuronIDs( void )
  {
    std::vector< unsigned int > ids;

    for ( const auto& element: _dataSet.neurons( ))
    {
      ids.push_back( element.second->gid( ));
    }

    return ids;
  }

  NeuronPtr NeuronsCollection::neuronById( unsigned int id )
  {
    nsol::NeuronsMap::iterator it;

    it = _dataSet.neurons( ).find( id );
    if( it == _dataSet.neurons( ).end( ))
      return nullptr;

    return dynamic_cast< NeuronPtr >( it->second );

  }



#ifdef NEUROLOTS_USE_ZEROEQ

  zeroeq::Subscriber* NeuronsCollection::Subscriber( void )
  {
    return _subscriber;
  }

#endif

  bool NeuronsCollection::selectionChange( void )
  {
    bool result = _selectionChange;
    _selectionChange = false;
    return result;
  }

  // SETTER

  void NeuronsCollection::lod( float lod_ )
  {
    if( lod_ < 1 )
      _lod = 1;
    else
      _lod = lod_;

    _programQuads->levelOfTessellation( &_lod );
    _programTriangles->levelOfTessellation( &_lod );
  }

  void NeuronsCollection::tng( float tng_ )
  {
    if ( tng_ > 1.0 )
      _tng = 20.f;
    else if ( tng_ < 0.0f )
      _tng = 0.0f;
    else
      _tng = tng_ * 20.0f;

    _programQuads->alphaTangentModule( &_tng );
    _programTriangles->alphaTangentModule( &_tng );
  }

  void NeuronsCollection::maxDist( float maxDist_ )
  {
    if ( maxDist_ > 1.0f )
      _maxDist = _camera->FarPlane( );
    else if ( maxDist_ < 0.0f )
      _maxDist = 0.0f;
    else
      _maxDist = maxDist_ * _camera->FarPlane( );

    _programQuads->maximumDistance( &_maxDist );
    _programTriangles->maximumDistance( &_maxDist );
  }

  void NeuronsCollection::neuronColor( const Eigen::Vector3f& neuronColor_ )
  {
    _neuronColor.resize( 3 );
    _neuronColor[ 0 ] = neuronColor_.x( );
    _neuronColor[ 1 ] = neuronColor_.y( );
    _neuronColor[ 2 ] = neuronColor_.z( );
  }

  void NeuronsCollection::selectedNeuronColor(
    const Eigen::Vector3f& neuronColor_ )
  {
    _selectedNeuronColor.resize( 3 );
    _selectedNeuronColor[ 0 ] = neuronColor_.x( );
    _selectedNeuronColor[ 1 ] = neuronColor_.y( );
    _selectedNeuronColor[ 2 ] = neuronColor_.z( );
  }

  void NeuronsCollection::paintSoma( bool paintSoma_ )
  {
    _paintSoma = paintSoma_;
  }

  void NeuronsCollection::paintNeurites( bool paintNeurites_ )
  {
    _paintNeurites = paintNeurites_;
  }

  void NeuronsCollection::paintSelectedSoma( bool paintSelectedSoma_ )
  {
    _paintSelectedSoma = paintSelectedSoma_;
  }

  void NeuronsCollection::paintSelectedNeurites( bool paintSelectedNeurites_ )
  {
    _paintSelectedNeurites = paintSelectedNeurites_;
  }

  // PRIVATE

  void NeuronsCollection::_init( void )
  {
    NeuronPtr neuron;
    NeuronMorphologyPtr morpho;

    for ( auto& element: _dataSet.neurons( ))
    {
      neuron = dynamic_cast< NeuronPtr >( element.second );
      morpho =  dynamic_cast< NeuronMorphologyPtr >( neuron->morphology( ));
      if ( ! morpho->HasNeuronMesh( ))
        morpho->NeuronMesh( new NeuronMesh( morpho ));
    }
  }

  bool NeuronsCollection::_isSelected( nsol::NeuronPtr neuron_ )
  {
    return !( _selectedNeurons.find( neuron_->gid( )) ==
              _selectedNeurons.end( ));
  }

  void NeuronsCollection::_defaultCamera( void )
  {
    NeuronPtr neuron;
    TBoundingBox boundingBox;

    boundingBox.xMax = FLT_MIN;
    boundingBox.xMin = FLT_MAX;
    boundingBox.yMax = FLT_MIN;
    boundingBox.yMin = FLT_MAX;
    boundingBox.zMax = FLT_MIN;
    boundingBox.zMin = FLT_MAX;


    for ( const auto& element: _dataSet.neurons( ))
    {
      neuron = dynamic_cast< NeuronPtr >( element.second );
      TBoundingBox box = neuron->BoundingBox( );

      if( box.xMax > boundingBox.xMax)
        boundingBox.xMax = box.xMax;
      if( box.xMin < boundingBox.xMin)
        boundingBox.xMin = box.xMin;

      if( box.yMax > boundingBox.yMax)
        boundingBox.yMax = box.yMax;
      if( box.yMin < boundingBox.yMin)
        boundingBox.yMin = box.yMin;

      if( box.zMax > boundingBox.zMax)
        boundingBox.zMax = box.zMax;
      if( box.zMin < boundingBox.zMin)
        boundingBox.zMin = box.zMin;
    }

    Eigen::Vector3f center(( boundingBox.xMin + boundingBox.xMax ) / 2,
        ( boundingBox.yMin + boundingBox.yMax ) / 2,
        ( boundingBox.zMin + boundingBox.zMax ) / 2 );
    Eigen::Vector3f corner( boundingBox.xMin, boundingBox.yMin,
         boundingBox.zMin );
    float radius = ( center - corner ).norm( ) /  sin( _camera->Fov( ));

    _defaultPivot = center;
    _defaultRadius = radius;

    // std::cout << "pivot: " << center.x( ) << " " << center.y( ) << " "
    //     << center.z( ) << std::endl;
    // std::cout << "radius: " << radius << std::endl;
  }

  void NeuronsCollection::_vectorToMesh(
    const std::vector< float >& vecVertices_,
    const std::vector< float >& vecNormals_,
    nlgeometry::Vertices& vertices_,
    nlgeometry::Facets& facets_ ) const
  {
    vertices_.clear( );
    facets_.clear( );
    Eigen::Vector3f position;
    Eigen::Vector3f normal;

    nlgeometry::SpatialHashTable spht;

    for ( unsigned int i = 0; i < ( unsigned int ) vecVertices_.size( ) / 9;
          i++ )
    {
      position = Eigen::Vector3f( vecVertices_[i*9], vecVertices_[i*9+1],
                                  vecVertices_[i*9+2] );
      normal = Eigen::Vector3f( vecNormals_[i*9], vecNormals_[i*9+1],
                                vecNormals_[i*9+2] );
      nlgeometry::VertexPtr v0 = new nlgeometry::Vertex( position, normal );
      v0 = spht.insert( v0 );

      position = Eigen::Vector3f( vecVertices_[i*9+3], vecVertices_[i*9+4],
                                  vecVertices_[i*9+5] );
      normal = Eigen::Vector3f( vecNormals_[i*9+3], vecNormals_[i*9+4],
                                vecNormals_[i*9+5] );
      nlgeometry::VertexPtr v1 = new nlgeometry::Vertex( position, normal );
      v1 = spht.insert( v1);

      position = Eigen::Vector3f( vecVertices_[i*9+6], vecVertices_[i*9+7],
                                  vecVertices_[i*9+8] );
      normal = Eigen::Vector3f( vecNormals_[i*9+6], vecNormals_[i*9+7],
                                vecNormals_[i*9+8] );
      nlgeometry::VertexPtr v2 = new nlgeometry::Vertex( position, normal );
      v2 = spht.insert( v2 );

      if( v0 != v1 && v0!=v2 && v1!=v2 )
      {
        nlgeometry::FacetPtr f = new nlgeometry::Facet( v0, v1, v2 );
        facets_.push_back( f );
      }
    }

    spht.vertices( vertices_ );
  }

#ifdef NEUROLOTS_USE_ZEROEQ

#ifdef NEUROLOTS_USE_GMRVLEX

  void NeuronsCollection::_onFocusEvent(
    zeroeq::gmrv::ConstFocusedIDsPtr event_ )
  {
    std::vector< unsigned int > focus = std::move( event_->getIdsVector( ));

    std::set<unsigned int> focusedNeurons;

    focusedNeurons.clear();
    for( unsigned int i = 0; i < focus.size(); i ++)
    {
      focusedNeurons.insert( focus[i] );
    }

    Eigen::Vector3f center;
    float radius;

    if( focusedNeurons.size( ) > 0 )
    {
      nsol::MiniColumns miniColumns;
      NeuronPtr neuron;
      nsol::Neurons neurons;
      TBoundingBox boundingBox;

      boundingBox.xMax = FLT_MIN;
      boundingBox.xMin = FLT_MAX;
      boundingBox.yMax = FLT_MIN;
      boundingBox.yMin = FLT_MAX;
      boundingBox.zMax = FLT_MIN;
      boundingBox.zMin = FLT_MAX;

      for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
      {
        miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
        for( unsigned int j = 0; j < miniColumns.size( ); j++ )
        {
          neurons = miniColumns[ j ]->neurons( );
          for( unsigned int k = 0; k < neurons.size( ); k++ )
          {
            neuron = ( NeuronPtr )neurons[ k ];

            if( !( focusedNeurons.find( neuron->gid( )) ==
                focusedNeurons.end( )))
            {
              TBoundingBox box = neuron->BoundingBox( );

              if( box.xMax > boundingBox.xMax)
                boundingBox.xMax = box.xMax;
              if( box.xMin < boundingBox.xMin)
                boundingBox.xMin = box.xMin;

              if( box.yMax > boundingBox.yMax)
                boundingBox.yMax = box.yMax;
              if( box.yMin < boundingBox.yMin)
                boundingBox.yMin = box.yMin;

              if( box.zMax > boundingBox.zMax)
                boundingBox.zMax = box.zMax;
              if( box.zMin < boundingBox.zMin)
                boundingBox.zMin = box.zMin;
            }
          }
        }
      }
      center = Eigen::Vector3f(( boundingBox.xMin + boundingBox.xMax ) / 2,
          ( boundingBox.yMin + boundingBox.yMax ) / 2,
          ( boundingBox.zMin + boundingBox.zMax ) / 2 );
      Eigen::Vector3f corner( boundingBox.xMin, boundingBox.yMin,
             boundingBox.zMin );

      radius = ( center - corner ).norm( ) /  sin( _camera->Fov( ));


    }
    else
    {
      center = _defaultPivot;
      radius = _defaultRadius;
    }

    _camera->TargetPivotRadius( center, radius );
  }

#endif // NEUROLOTS_USE_GMRVLEX

#ifdef NEUROLOTS_USE_LEXIS
  void NeuronsCollection::_onSelectionEvent(
    lexis::data::ConstSelectedIDsPtr selection )
  {
    std::vector<unsigned int> selected = std::move( selection->getIdsVector( ));

    _selectedNeurons.clear();

    for( unsigned int i = 0; i < selected.size(); i ++)
    {
      _selectedNeurons.insert( selected[i] );
    }
    _selectionChange = true;
  }


  void NeuronsCollection::_onSelectionFocusEvent(
    lexis::data::ConstSelectedIDsPtr selFocus )
  {
    // Selection
    std::vector<unsigned int> selected = std::move( selFocus->getIdsVector( ));

    _selectedNeurons.clear();
    for( unsigned int i = 0; i < selected.size(); i ++)
    {
      _selectedNeurons.insert( selected[i] );
    }

    // Focus
    Eigen::Vector3f center;
    float radius;

    if( _selectedNeurons.size( ) > 0 )
    {
      nsol::MiniColumns miniColumns;
      NeuronPtr neuron;
      nsol::Neurons neurons;
      TBoundingBox boundingBox;

      boundingBox.xMax = FLT_MIN;
      boundingBox.xMin = FLT_MAX;
      boundingBox.yMax = FLT_MIN;
      boundingBox.yMin = FLT_MAX;
      boundingBox.zMax = FLT_MIN;
      boundingBox.zMin = FLT_MAX;


      // TO CHANGE
      for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
      {
        miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
        for( unsigned int j = 0; j < miniColumns.size( ); j++ )
        {
          neurons = miniColumns[ j ]->neurons( );
          for( unsigned int k = 0; k < neurons.size( ); k++ )
          {
            neuron = ( NeuronPtr )neurons[ k ];

            if( !( _selectedNeurons.find( neuron->gid( )) ==
                _selectedNeurons.end( )))
            {
              TBoundingBox box = neuron->BoundingBox( );

              if( box.xMax > boundingBox.xMax)
                boundingBox.xMax = box.xMax;
              if( box.xMin < boundingBox.xMin)
                boundingBox.xMin = box.xMin;

              if( box.yMax > boundingBox.yMax)
                boundingBox.yMax = box.yMax;
              if( box.yMin < boundingBox.yMin)
                boundingBox.yMin = box.yMin;

              if( box.zMax > boundingBox.zMax)
                boundingBox.zMax = box.zMax;
              if( box.zMin < boundingBox.zMin)
                boundingBox.zMin = box.zMin;
            }
          }
        }
      }
      center = Eigen::Vector3f(( boundingBox.xMin + boundingBox.xMax ) / 2,
          ( boundingBox.yMin + boundingBox.yMax ) / 2,
          ( boundingBox.zMin + boundingBox.zMax ) / 2 );
      Eigen::Vector3f corner( boundingBox.xMin, boundingBox.yMin,
             boundingBox.zMin );

      radius = ( center - corner ).norm( ) /  sin( _camera->Fov( ));


    }
    else
    {
      center = _defaultPivot;
      radius = _defaultRadius;
    }

    _camera->TargetPivotRadius( center, radius );
  }

#endif // NEUROLOTS_USE_LEXIS

#endif // NEUROLOTS_USE_ZEROEQ

}
