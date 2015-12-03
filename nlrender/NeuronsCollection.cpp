#include "NeuronsCollection.h"

#include "NeuronMorphology.h"
#include "NeuronMesh.h"

#include <cfloat>
#include <iostream>

#ifdef NEUROLOTS_USE_GMRVZEQ
#include <gmrvzeq/gmrvzeq.h>
#endif

#include "../nlgeometry/SpatialHashTable.h"

namespace neurolots
{

  NeuronsCollection::NeuronsCollection( Camera* camera_ )
    : _camera( camera_ )
    , _dataSet( )
#ifdef NEUROLOTS_USE_ZEQ
    , _zeqConnection( false )
#endif
    , _tessMethod( HOMOGENEOUS )
  {
    _selectedNeurons.clear( );

    std::string neurolotsShadersPath;

    if ( getenv( "NEUROLOTS_SHADERS_PATH" ) == nullptr )
    {
      // std::cerr << "Environment Variable NEUROLOTS_SHADERS_PATH not defined"
      //           << std::endl;
      _programQuads = new Program( Program::QUADS );
      _programQuadsFB = new Program( Program::QUADS_FB );
      _programTriangles = new Program( Program::TRIANGLES );
      _programTrianglesFB = new Program( Program::TRIANGLES_FB );
    }
    else
    {
      neurolotsShadersPath = std::string( getenv( "NEUROLOTS_SHADERS_PATH" ));

      std::string quadsPath = neurolotsShadersPath;
      quadsPath.append( "/quads" );

      std::string trianglesPath = neurolotsShadersPath;
      trianglesPath.append( "/triangles" );

      _programQuads = new Program( Program::QUADS, quadsPath );
      _programQuadsFB = new Program( Program::QUADS_FB, quadsPath );
      _programTriangles = new Program( Program::TRIANGLES, trianglesPath );
      _programTrianglesFB = new Program( Program::TRIANGLES_FB, trianglesPath );
    }
    _programQuads->Init();
    _programQuadsFB->Init( );
    _programTriangles->Init( );
    _programTrianglesFB->Init( );

    lod( 3.0f );
    tng( 0.5f );
    maxDist( 0.1f );

    NeuronColor( Eigen::Vector3f( 0.0f, 0.5f, 0.7f ));
    SelectedNeuronColor( Eigen::Vector3f( 0.7f, 0.5f, 0.0f ));

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
#ifdef NSOL_USE_BBPSDK
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
#ifdef NSOL_USE_BBPSDK
    loadFlags_ |= nsol::MORPHOLOGY | nsol::CORTICAL_HIERARCHY;
    try{
        _dataSet.loadFromBlueConfig< nsol::Node,
                                     nsol::Section,
                                     nsol::Dendrite,
                                     nsol::Axon,
                                     nsol::Soma,
                                     NeuronMorphology,
                                     Neuron,
                                     nsol::MiniColumn,
                                     nsol::Column >( blueConfig_,
                                                     loadFlags_,
                                                     target_ );
        _GenerateMeshes( );
        _Init( );

        _DefaultCamera( );
        _camera->Pivot( _defaultPivot );
        _camera->Radius( _defaultRadius );
      }
      catch( ... )
      {
        std::cerr << "Error: can't load file: " << blueConfig_ << std::endl;
        exit(-1);
      }
#else
    std::cerr << "Error: Bbpsdk support not built-in" << std::endl;
    exit( -1 );
#endif
  }

  void NeuronsCollection::loadSwc( const std::string& swcFile_ )
  {
    _dataSet.loadNeuronFromSwc< nsol::Node,
                                nsol::Section,
                                nsol::Dendrite,
                                nsol::Axon,
                                nsol::Soma,
                                NeuronMorphology,
                                Neuron >( swcFile_, 1 );
    _GenerateMeshes( );
    _Init( );

    _DefaultCamera( );
    _camera->Pivot( _defaultPivot );
    _camera->Radius( _defaultRadius );
  }

  void NeuronsCollection::loadScene( const std::string& xmlFile_ )
  {
     _dataSet.loadScene< nsol::Node,
                         nsol::Section,
                         nsol::Dendrite,
                         nsol::Axon,
                         nsol::Soma,
                         NeuronMorphology,
                         Neuron >( xmlFile_ );
     _GenerateMeshes( );
     _Init( );

     _DefaultCamera( );
     _camera->Pivot( _defaultPivot );
     _camera->Radius( _defaultRadius );
  }

  void NeuronsCollection::setZeqUri( const std::string&
#ifdef NEUROLOTS_USE_ZEQ
                                     uri_
#endif
    )
  {
#ifdef NEUROLOTS_USE_ZEQ
    _zeqConnection = true;
    _uri =  servus::URI( uri_ );
    _subscriber = new zeq::Subscriber( _uri );

    _subscriber->registerHandler( zeq::hbp::EVENT_SELECTEDIDS,
        boost::bind( &NeuronsCollection::_OnSelectionEvent , this, _1 ));

#ifdef NEUROLOTS_USE_GMRVZEQ
    _subscriber->registerHandler( zeq::gmrv::EVENT_FOCUSEDIDS,
        boost::bind( &NeuronsCollection::_OnFocusEvent , this, _1 ));
#endif
    pthread_create( &_subscriberThread, NULL, _Subscriber, this );
#endif
  }

  void NeuronsCollection::Paint( void )
  {
    NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniform3fv( 4, 1, _camera->Position( ));

    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniform3fv( 4, 1, _camera->Position( ));

    for ( auto ele: _dataSet.neurons( ))
    {
      neuron = dynamic_cast< NeuronPtr >( ele.second );
      neuronMesh = dynamic_cast< NeuronMorphologyPtr >( neuron->morphology( ))
        ->NeuronMesh( );

      glUseProgram( _programQuads->id( ));
      glUniformMatrix4fv( 2, 1, GL_FALSE, neuron->vecTransform( ).data( ));

      glUseProgram( _programTriangles->id( ));
      glUniformMatrix4fv( 2, 1, GL_FALSE, neuron->vecTransform( ).data( ));

#ifdef NEUROLOTS_USE_ZEQ

      if( _zeqConnection )
      {
        if( _IsSelected( neuron ) )
        {
          glUseProgram( _programQuads->id( ));
          glUniform3fv( 3, 1, _selectedNeuronColor.data( ));
          glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
          neuronMesh->PaintNeurites( );

          glUseProgram( _programTriangles->id( ));
          glUniform3fv( 3, 1, _selectedNeuronColor.data( ));
          glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
          neuronMesh->PaintSoma( );
        }
        else
        {
          glUseProgram( _programTriangles->id( ));
          glUniform3fv( 3, 1, _neuronColor.data( ));
          glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
          neuronMesh->PaintSoma( );
        }
      }
      else
      {
        glUseProgram( _programQuads->id( ));
        glUniform3fv( 3, 1, _neuronColor.data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
        neuronMesh->PaintNeurites( );

        glUseProgram( _programTriangles->id( ));
        glUniform3fv( 3, 1, _neuronColor.data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
        neuronMesh->PaintSoma( );
      }
#else
      glUseProgram( _programQuads->id( ));
      glUniform3fv( 3, 1, _neuronColor.data( ));
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
      neuronMesh->PaintNeurites( );

      glUseProgram( _programTriangles->id( ));
      glUniform3fv( 3, 1, _neuronColor.data( ));
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
      neuronMesh->PaintSoma( );
#endif
    }
  }

  void NeuronsCollection::PaintNeuron( const unsigned int& id_,
                                       const Eigen::Vector3f& color_ )
  {
    NeuronPtr neuron = nullptr;
    NeuronMeshPtr neuronMesh = nullptr;
    nsol::NeuronsMap::iterator it;

    it = _dataSet.neurons( ).find( id_ );
    if ( it != _dataSet.neurons( ).end( ))
    {
      neuron = dynamic_cast< NeuronPtr >( it->second );
      neuronMesh =  dynamic_cast< NeuronMorphologyPtr >(
        it->second->morphology( ))->NeuronMesh( );
    }
    if ( neuronMesh && neuron )
    {
      std::vector< float > color;
      color.resize( 3 );
      color[0] = color_.x( );
      color[1] = color_.y( );
      color[2] = color_.z( );

      glUseProgram( _programQuads->id( ));
      glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
      glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
      glUniformMatrix4fv( 2, 1, GL_FALSE,
                          neuron->vecTransform( ).data( ));
      glUniform3fv( 3, 1, color.data( ));
      glUniform3fv( 4, 1,_camera->Position( ));
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
      neuronMesh->PaintNeurites( );

      glUseProgram( _programTriangles->id( ));
      glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
      glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
      glUniformMatrix4fv( 2, 1, GL_FALSE,
                          neuron->vecTransform( ).data( ));
      glUniform3fv( 3, 1, color.data( ));
      glUniform3fv( 4, 1,_camera->Position( ));
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
      neuronMesh->PaintSoma( );
    }
  }

  void NeuronsCollection::PaintNeuron( const NeuronPtr& neuron,
                                       const Eigen::Vector3f& color_ )
  {
    NeuronMeshPtr neuronMesh;
    if ( !neuron )
      return;
    neuronMesh =
      dynamic_cast< NeuronMorphologyPtr >(
        neuron->morphology( ))->NeuronMesh( );
    if( !neuronMesh )
      return;

    std::vector< float > color;
    color.resize( 3 );
    color[0] = color_.x( );
    color[1] = color_.y( );
    color[2] = color_.z( );

    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniformMatrix4fv( 2, 1, GL_FALSE,
                        neuron->vecTransform( ).data( ));
    glUniform3fv( 3, 1, color.data( ));
    glUniform3fv( 4, 1,_camera->Position( ));
    glUniform1fv( 5, 1, &_lod );
    glUniform1fv( 6, 1, &_tng );
    glUniform1fv( 7, 1, &_maxDist );
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
    neuronMesh->PaintSoma( );

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniformMatrix4fv( 2, 1, GL_FALSE,
                        neuron->vecTransform( ).data( ));
    glUniform3fv( 3, 1, color.data( ));
    glUniform3fv( 4, 1,_camera->Position( ));
    glUniform1fv( 5, 1, &_lod );
    glUniform1fv( 6, 1, &_tng );
    glUniform1fv( 7, 1, &_maxDist );
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
    neuronMesh->PaintNeurites( );
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

  void NeuronsCollection::extractMesh( NeuronPtr neuron_ )
  {
    NeuronMeshPtr neuronMesh;
    if ( !neuron_ )
      return;
    neuronMesh =
      dynamic_cast< NeuronMorphologyPtr >(
        neuron_->morphology( ))->NeuronMesh( );
    if( !neuronMesh )
      return;

    glUseProgram( _programTrianglesFB->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniformMatrix4fv( 2, 1, GL_FALSE,
                        neuron_->vecTransform( ).data( ));
    glUniform3fv( 4, 1,_camera->Position( ));
    glUniform1fv( 5, 1, &_lod );
    glUniform1fv( 6, 1, &_tng );
    glUniform1fv( 7, 1, &_maxDist );

    glUseProgram( _programQuadsFB->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniformMatrix4fv( 2, 1, GL_FALSE,
                        neuron_->vecTransform( ).data( ));
    glUniform3fv( 4, 1,_camera->Position( ));
    glUniform1fv( 5, 1, &_lod );
    glUniform1fv( 6, 1, &_tng );
    glUniform1fv( 7, 1, &_maxDist );

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

    glUseProgram( _programTrianglesFB->id( ));
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
    neuronMesh->PaintSoma( );

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

    glUseProgram( _programTrianglesFB->id( ));
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
    neuronMesh->PaintSoma( );

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

    glUseProgram( _programQuadsFB->id( ));
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
    neuronMesh->PaintNeurites( );

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

    glUseProgram( _programQuadsFB->id( ));
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &_tessMethod );
    neuronMesh->PaintNeurites( );

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

    Vertices vertices;
    Facets facets;

    _VectorToMesh( _backVertices, _backNormals, vertices, facets );

    _backVertices.clear( );
    _backNormals.clear( );

    neuronMesh->WriteOBJ( std::string("out.obj"), vertices, facets );

  }

  // GETTERS

  ColumnsPtr NeuronsCollection::Columns( void )
  {
    return &_dataSet.columns( );
  }

  std::vector< unsigned int > NeuronsCollection::neuronIDs( void )
  {
    std::vector< unsigned int > ids;

    for ( auto ele: _dataSet.neurons( ))
    {
      ids.push_back( ele.second->gid( ));
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



#ifdef NEUROLOTS_USE_ZEQ

  zeq::Subscriber* NeuronsCollection::Subscriber( void )
  {
    return _subscriber;
  }

#endif

  // SETTER

  void NeuronsCollection::lod( float lod_ )
  {
    if( lod_ < 1 )
      _lod = 1;
    else
      _lod = lod_;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( 5, 1, &_lod );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( 5, 1, &_lod );
  }

  void NeuronsCollection::tng( float tng_ )
  {
    if ( tng_ > 1.0 )
      _tng = 20.f;
    else if ( tng_ < 0.0f )
      _tng = 0.0f;
    else
      _tng = tng_ * 20.0f;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( 6, 1, &_tng );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( 6, 1, &_tng );
  }

  void NeuronsCollection::maxDist( float maxDist_ )
  {
    if ( maxDist_ > 1.0f )
      _maxDist = _camera->FarPlane( );
    else if ( maxDist_ < 0.0f )
      _maxDist = 0.0f;
    else
      _maxDist = maxDist_ * _camera->FarPlane( );

    glUseProgram( _programQuads->id( ));
    glUniform1fv( 7, 1, &_maxDist );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( 7, 1, &_maxDist );
  }

  void NeuronsCollection::NeuronColor( Eigen::Vector3f neuronColor_ )
  {
    _neuronColor.resize( 3 );
    _neuronColor[ 0 ] = neuronColor_.x( );
    _neuronColor[ 1 ] = neuronColor_.y( );
    _neuronColor[ 2 ] = neuronColor_.z( );
  }

  void NeuronsCollection::SelectedNeuronColor( Eigen::Vector3f neuronColor_ )
  {
    _selectedNeuronColor.resize( 3 );
    _selectedNeuronColor[ 0 ] = neuronColor_.x( );
    _selectedNeuronColor[ 1 ] = neuronColor_.y( );
    _selectedNeuronColor[ 2 ] = neuronColor_.z( );
  }

  // PRIVATE

  void NeuronsCollection::_Init( void )
  {
    NeuronPtr neuron;
    NeuronMorphologyPtr morpho;
    NeuronMeshPtr neuronMesh;

    for ( auto ele: _dataSet.neurons( ))
    {
      neuron = dynamic_cast< NeuronPtr >( ele.second );
      morpho =  dynamic_cast< NeuronMorphologyPtr >( neuron->morphology( ));
      neuronMesh = morpho->NeuronMesh( );

      neuron->Init( );
      neuronMesh->Init( );
    }
  }

  void NeuronsCollection::_GenerateMeshes( void )
  {
    NeuronMorphologyPtr morpho;
    NeuronMeshPtr neuronMesh;

    for( auto ele: _dataSet.neurons( ))
    {
      morpho = dynamic_cast< NeuronMorphologyPtr >( ele.second->morphology( ));
      if( !morpho->HasNeuronMesh( ) )
      {
        neuronMesh = new NeuronMesh( morpho );
        morpho->NeuronMesh( neuronMesh );
      }
    }
  }

  bool NeuronsCollection::_IsSelected( nsol::NeuronPtr neuron_ )
  {
    return !( _selectedNeurons.find( neuron_->gid( )) ==
              _selectedNeurons.end( ));
  }

  void NeuronsCollection::_DefaultCamera( void )
  {
    NeuronPtr neuron;
    TBoundingBox boundingBox;

    boundingBox.xMax = FLT_MIN;
    boundingBox.xMin = FLT_MAX;
    boundingBox.yMax = FLT_MIN;
    boundingBox.yMin = FLT_MAX;
    boundingBox.zMax = FLT_MIN;
    boundingBox.zMin = FLT_MAX;


    for ( auto ele: _dataSet.neurons( ))
    {
      neuron = dynamic_cast< NeuronPtr >( ele.second );
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

  void NeuronsCollection::_VectorToMesh(
    const std::vector< float >& vecVertices_,
    const std::vector< float >& vecNormals_,
    Vertices& vertices_, Facets& facets_ ) const
  {
    vertices_.clear( );
    facets_.clear( );
    Eigen::Vector3f position;
    Eigen::Vector3f normal;

    SpatialHashTable spht;

    for ( unsigned int i = 0; i < ( unsigned int ) vecVertices_.size( ) / 9;
          i++ )
    {
      position = Eigen::Vector3f( vecVertices_[i*9], vecVertices_[i*9+1],
                                  vecVertices_[i*9+2] );
      normal = Eigen::Vector3f( vecNormals_[i*9], vecNormals_[i*9+1],
                                vecNormals_[i*9+2] );
      VertexPtr v0 = new Vertex( position, normal );
      v0 = spht.insert( v0 );

      position = Eigen::Vector3f( vecVertices_[i*9+3], vecVertices_[i*9+4],
                                  vecVertices_[i*9+5] );
      normal = Eigen::Vector3f( vecNormals_[i*9+3], vecNormals_[i*9+4],
                                vecNormals_[i*9+5] );
      VertexPtr v1 = new Vertex( position, normal );
      v1 = spht.insert( v1);

      position = Eigen::Vector3f( vecVertices_[i*9+6], vecVertices_[i*9+7],
                                  vecVertices_[i*9+8] );
      normal = Eigen::Vector3f( vecNormals_[i*9+6], vecNormals_[i*9+7],
                                vecNormals_[i*9+8] );
      VertexPtr v2 = new Vertex( position, normal );
      v2 = spht.insert( v2 );

      if( v0 != v1 && v0!=v2 && v1!=v2 )
      {
        FacetPtr f = new Facet( v0, v1, v2 );
        facets_.push_back( f );
      }
    }

    spht.vertices( vertices_ );
  }

#ifdef NEUROLOTS_USE_ZEQ

  void NeuronsCollection::_OnFocusEvent( const zeq::Event& event_ )
  {
    std::vector<unsigned int> focus =
        zeq::hbp::deserializeSelectedIDs( event_ );
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

  void NeuronsCollection::_OnSelectionEvent( const zeq::Event& event_ )
  {
    std::vector<unsigned int> selected =
        zeq::hbp::deserializeSelectedIDs( event_ );
    _selectedNeurons.clear();
    for( unsigned int i = 0; i < selected.size(); i ++)
    {
      _selectedNeurons.insert( selected[i] );
    }
  }


  void NeuronsCollection::_OnSelectionFocusEvent( const zeq::Event& event_ )
  {
    // Selection
    std::vector<unsigned int> selected =
        zeq::hbp::deserializeSelectedIDs( event_ );
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

  void* NeuronsCollection::_Subscriber( void* collection_ )
  {
    NeuronsCollection* collection = ( NeuronsCollection* )collection_;
    zeq::Subscriber* subscriber = collection->Subscriber( );
    std::cout << "Waiting Selection Events..." << std::endl;
    while ( true )
    {
      subscriber->receive( 10000 );
    }
    pthread_exit( NULL );
  }

#endif

}
