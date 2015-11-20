#include "NeuronsCollection.h"

#include "NeuronMorphology.h"
#include "NeuronMesh.h"

#include <cfloat>
#include <iostream>

#ifdef NEUROLOTS_USE_GMRVZEQ
#include <gmrvzeq/gmrvzeq.h>
#endif

namespace neurolots
{

  NeuronsCollection::NeuronsCollection( Camera* camera_ )
    : _camera( camera_ )
    , _dataSet( nsol::DataSet( ))
#ifdef NEUROLOTS_USE_ZEQ
    , _zeqConnection( false )
#endif
  {
    _selectedNeurons.clear( );

    std::string neurolotsShadersPath;

    if ( getenv( "NEUROLOTS_SHADERS_PATH" ) == nullptr )
    {
      std::cerr << "Environment Variable NEUROLOTS_SHADERS_PATH not defined"
                << std::endl;
      exit(-1);
    }
    else
      neurolotsShadersPath = std::string( getenv( "NEUROLOTS_SHADERS_PATH" ));

    std::string quadsPath = neurolotsShadersPath;
    quadsPath.append( "/quads" );

    std::string trianglesPath = neurolotsShadersPath;
    trianglesPath.append( "/triangles" );

    _programQuads = new Program( Program::QUADS, quadsPath );
    _programQuadsFB = new Program( Program::QUADS_FB, quadsPath );
    _programTriangles = new Program( Program::TRIANGLES, trianglesPath );
    _programTrianglesFB = new Program( Program::TRIANGLES_FB, trianglesPath );

    _programQuads->Init();
    _programQuadsFB->Init( );
    _programTriangles->Init( );
    _programTrianglesFB->Init( );


    Lod( 3.0f );
    Tng( 5.0f );
    MaxDist( 200.0f );

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
    loadFlags_ |= nsol::MORPHOLOGY | nsol::HIERARCHY;
    try{
        _dataSet.openBlueConfig< nsol::Node,
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
    _dataSet.addNeuron< nsol::Node,
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

    nsol::MiniColumns miniColumns;
    nsol::Neurons neurons;
    NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;
    NeuronMorphologyPtr morphology;

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniform3fv( 4, 1, _camera->Position( ));

    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniform3fv( 4, 1, _camera->Position( ));

    for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
    {
      miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[ j ]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          neuron = dynamic_cast< NeuronPtr >( neurons[ k ] );
          morphology = ( dynamic_cast< NeuronMorphologyPtr >
                         ( neuron->morphology( )));
          neuronMesh = morphology->NeuronMesh( );

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
              neuronMesh->PaintNeurites( );

              glUseProgram( _programTriangles->id( ));
              glUniform3fv( 3, 1, _selectedNeuronColor.data( ));
              neuronMesh->PaintSoma( );
            }
            else
            {
              glUseProgram( _programTriangles->id( ));
              glUniform3fv( 3, 1, _neuronColor.data( ));
              neuronMesh->PaintSoma( );
            }
          }
          else
          {
            glUseProgram( _programQuads->id( ));
            glUniform3fv( 3, 1, _neuronColor.data( ));
            neuronMesh->PaintNeurites( );

            glUseProgram( _programTriangles->id( ));
            glUniform3fv( 3, 1, _neuronColor.data( ));
            neuronMesh->PaintSoma( );
          }
#else
          glUseProgram( _programQuads->id( ));
          glUniform3fv( 3, 1, _neuronColor.data( ));
          neuronMesh->PaintNeurites( );

          glUseProgram( _programTriangles->id( ));
          glUniform3fv( 3, 1, _neuronColor.data( ));
          neuronMesh->PaintSoma( );

#endif
        }
      }
    }
  }

  void NeuronsCollection::PaintNeuron( const unsigned int& id_,
                                       const Eigen::Vector3f& color_ )
  {
    nsol::MiniColumns miniColumns;
    nsol::Neurons neurons;
    NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;
    for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
    {
      miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[ j ]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          neuron = ( NeuronPtr ) neurons[ k ];
          if ( neuron->gid( ) == id_ )
          {
            neuronMesh =
              (( NeuronMorphologyPtr )neuron->morphology( ))->NeuronMesh( );

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
            neuronMesh->PaintNeurites( );

            glUseProgram( _programTriangles->id( ));
            glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
            glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
            glUniformMatrix4fv( 2, 1, GL_FALSE,
                                neuron->vecTransform( ).data( ));
            glUniform3fv( 3, 1, color.data( ));
            glUniform3fv( 4, 1,_camera->Position( ));
            neuronMesh->PaintSoma( );
          }
        }
      }
    }
  }

  void NeuronsCollection::PaintNeuron( const NeuronPtr& neuron,
                                       const Eigen::Vector3f& color_ )
  {
    NeuronMeshPtr neuronMesh;
    if ( !neuron )
      return;
    neuronMesh =
        (( NeuronMorphologyPtr )neuron->morphology( ))->NeuronMesh( );
    if( !neuronMesh )
      return;

    std::vector< float > color;
    color.resize( 3 );
    color[0] = color_.x( );
    color[1] = color_.y( );
    color[2] = color_.z( );

    unsigned int query;
    unsigned int numPrimitives;

    glGenQueries( 1, &query );
    glBeginQuery( GL_PRIMITIVES_GENERATED, query );

    glUseProgram( _programTrianglesFB->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniformMatrix4fv( 2, 1, GL_FALSE,
                        neuron->vecTransform( ).data( ));
    glUniform3fv( 3, 1, color.data( ));
    glUniform3fv( 4, 1,_camera->Position( ));
    glUniform1fv( 5, 1, &_lod );
    glUniform1fv( 6, 1, &_tng );
    glUniform1fv( 7, 1, &_maxDist );
    neuronMesh->PaintSoma( );

    glUseProgram( _programQuadsFB->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniformMatrix4fv( 2, 1, GL_FALSE,
                        neuron->vecTransform( ).data( ));
    glUniform3fv( 3, 1, color.data( ));
    glUniform3fv( 4, 1,_camera->Position( ));
    glUniform1fv( 5, 1, &_lod );
    glUniform1fv( 6, 1, &_tng );
    glUniform1fv( 7, 1, &_maxDist );
    neuronMesh->PaintNeurites( );

    glEndQuery( GL_PRIMITIVES_GENERATED );
    glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );

    std::cout << "Numero de primitivas generadas: " << numPrimitives
              << std::endl;
  }

  void NeuronsCollection::AddLod( const float& addLod_ )
  {
    _lod += addLod_;
    if ( _lod < 1.0f )
      _lod = 1.0f;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( 5, 1, &_lod );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( 5, 1, &_lod );
  }

  void NeuronsCollection::AddTng( const float& addTng_ )
  {
    _tng += addTng_;
    if ( _tng < 0.0f )
      _tng = 0.0f;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( 6, 1, &_tng );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( 6, 1, &_tng );
  }

  void NeuronsCollection::AddMaxDist( const float& addMaxDist_ )
  {
    _maxDist += addMaxDist_;
    if( _maxDist < 2 )
      _maxDist = 2;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( 7, 1, &_maxDist );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( 7, 1, &_maxDist );
  }



  void NeuronsCollection::focusOnNeuron( unsigned int id )
  {
    nsol::MiniColumns miniColumns;
    nsol::Neurons neurons;
    NeuronPtr neuron = nullptr;
    bool found = false;
    for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
    {
      miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[ j ]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          if ( neurons[k]->gid( ) == id )
          {
            neuron = ( NeuronPtr ) neurons[ k ];
            found = true;
            break;
          }
        }
        if ( found )
          break;
      }
      if ( found )
        break;
    }

    if( found )
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
        (( NeuronMorphologyPtr )neuron_->morphology( ))->NeuronMesh( );
    if( !neuronMesh )
      return;

    std::vector< float > color;
    color.resize( 3 );
    color[0] = 1.0f;
    color[1] = 0.0f;
    color[2] = 0.0f;

    unsigned int query;
    unsigned int numPrimitives;

    glGenQueries( 1, &query );
    glBeginQuery( GL_PRIMITIVES_GENERATED, query );

    glUseProgram( _programTrianglesFB->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniformMatrix4fv( 2, 1, GL_FALSE,
                        neuron_->vecTransform( ).data( ));
    glUniform3fv( 3, 1, color.data( ));
    glUniform3fv( 4, 1,_camera->Position( ));
    glUniform1fv( 5, 1, &_lod );
    glUniform1fv( 6, 1, &_tng );
    glUniform1fv( 7, 1, &_maxDist );
    neuronMesh->PaintSoma( );

    glUseProgram( _programQuadsFB->id( ));
    glUniformMatrix4fv( 0, 1, GL_FALSE, _camera->ProjectionMatrix( ));
    glUniformMatrix4fv( 1, 1, GL_FALSE, _camera->ViewMatrix( ));
    glUniformMatrix4fv( 2, 1, GL_FALSE,
                        neuron_->vecTransform( ).data( ));
    glUniform3fv( 3, 1, color.data( ));
    glUniform3fv( 4, 1,_camera->Position( ));
    glUniform1fv( 5, 1, &_lod );
    glUniform1fv( 6, 1, &_tng );
    glUniform1fv( 7, 1, &_maxDist );
    neuronMesh->PaintNeurites( );

    glEndQuery( GL_PRIMITIVES_GENERATED );
    glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );

    std::cout << "Numero de primitivas generadas: " << numPrimitives
              << std::endl;
    // glDisable( GL_RASTERIZER_DISCARD );
  }

  // GETTERS

  ColumnsPtr NeuronsCollection::Columns( void )
  {
    return &_dataSet.columns( );
  }

  std::vector< unsigned int > NeuronsCollection::neuronIDs( void )
  {
    std::vector< unsigned int > ids;
    nsol::MiniColumns miniColumns;
    nsol::Neurons neurons;

    for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
    {
      miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[ j ]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          ids.push_back( neurons[ k ]->gid( ));
        }
      }
    }

    return ids;
  }

  NeuronPtr NeuronsCollection::neuronById( unsigned int id )
  {
    nsol::MiniColumns miniColumns;
    nsol::Neurons neurons;
    for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
    {
      miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[ j ]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          if ( neurons[k]->gid( ) == id )
          {
            return ( NeuronPtr ) neurons[ k ];
          }
        }
      }
    }
    return nullptr;
  }



#ifdef NEUROLOTS_USE_ZEQ

  zeq::Subscriber* NeuronsCollection::Subscriber( void )
  {
    return _subscriber;
  }

#endif

  // SETTER

  void NeuronsCollection::Lod( float lod_ )
  {
    _lod = lod_;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( 5, 1, &_lod );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( 5, 1, &_lod );
  }

  void NeuronsCollection::Tng( float tng_ )
  {
    _tng = tng_;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( 6, 1, &_tng );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( 6, 1, &_tng );
  }

  void NeuronsCollection::MaxDist( float maxDist_ )
  {
    _maxDist = maxDist_;

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
      nsol::MiniColumns miniColumns;
      NeuronPtr neuron;
      nsol::Neurons neurons;
      NeuronMorphologyPtr morpho;
      NeuronMeshPtr neuronMesh;


      for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
      {
        miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
        for( unsigned int j = 0; j < miniColumns.size( ); j++ )
        {
          neurons = miniColumns[ j ]->neurons( );
          for( unsigned int k = 0; k < neurons.size( ); k++ )
          {
            neuron = ( NeuronPtr )neurons[ k ];
            morpho = ( NeuronMorphologyPtr )neuron->morphology( );
            neuronMesh = morpho->NeuronMesh( );
            neuron->Init( );
            neuronMesh->Init( );
          }
        }
      }
    }

  void NeuronsCollection::_GenerateMeshes( void )
  {
    nsol::ColumnPtr colum;
    nsol::MiniColumnPtr miniColum;
    nsol::NeuronPtr neuron;
    NeuronMorphologyPtr morpho;
    NeuronMeshPtr neuronMesh;
    for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
    {
      colum = _dataSet.columns( )[ i ];
      for( unsigned int j = 0; j < colum->miniColumns( ).size( ); j++ )
      {
        miniColum = colum->miniColumns( )[ j ];
        for( unsigned int k = 0; k < miniColum->neurons( ).size( ); k++ )
        {
          neuron = miniColum->neurons( )[ k ];
          morpho = ( NeuronMorphologyPtr )neuron->morphology( );

          if( !morpho->HasNeuronMesh( ) )
          {
            neuronMesh = new NeuronMesh( morpho );
            morpho->NeuronMesh( neuronMesh );
          }
        }
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
