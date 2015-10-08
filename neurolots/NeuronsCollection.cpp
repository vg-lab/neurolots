#include "NeuronsCollection.h"

#include "NeuronMorphology.h"
#include "NeuronMesh.h"
#include "Neuron.h"

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
    , _cont( 0 )
#ifdef NEUROLOTS_USE_ZEQ
    , _zeqConnection( false )
#endif
  {
    _selectedNeurons.clear( );

    std::string neurolotsShadersPath( getenv( "NEUROLOTS_SHADERS_PATH" ));
    if( neurolotsShadersPath.data( ) == NULL )
    {
      std::cerr << "Environment Variable NEUROLOTS_SHADERS_PATH not defined"
                << std::endl;
      exit(-1);
    }

    std::string quadsPath = neurolotsShadersPath;
    quadsPath.append( "/quads" );

    std::string trianglesPath = neurolotsShadersPath;
    trianglesPath.append( "/triangles" );

    _programQuads = new Program( Program::QUADS, quadsPath );

    _programTriangles = new Program( Program::TRIANGLES, trianglesPath );

    _programQuads->Init();
    _programTriangles->Init();

    Lod( 3.0f );
    Tng( 5.0f );
    MaxDist( 200.0f );

    NeuronColor( Eigen::Vector3f( 0.0f, 0.5f, 0.7f ));
    SelectedNeuronColor( Eigen::Vector3f( 0.7f, 0.5f, 0.0f ));
  }

  NeuronsCollection::~NeuronsCollection( void )
  {

  }

  // PUBLIC

  void NeuronsCollection::loadBlueConfig(
#ifdef NSOL_USE_BBPSDK
                                          const std::string& blueConfig_,
                                          const std::string& target_
#else
                                          const std::string& /*blueConfig_*/,
                                          const std::string& /*target_*/
#endif
    )
  {
#ifdef NSOL_USE_BBPSDK
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
                                            nsol::MORPHOLOGY | nsol::HIERARCHY,
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
    nsol::NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                        _camera->ViewMatrix( ));
    glUniformMatrix4fv( _programQuads->uProy( ), 1, GL_FALSE,
                        _camera->ProjectionMatrix( ));
    glUniform3fv( _programQuads->uCameraPos( ), 1, _camera->Position( ));

    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( _programTriangles->uView( ), 1, GL_FALSE,
                        _camera->ViewMatrix());
    glUniformMatrix4fv( _programTriangles->uProy( ), 1, GL_FALSE,
                        _camera->ProjectionMatrix( ));
    glUniform3fv( _programTriangles->uCameraPos( ), 1,
                  _camera->Position( ));


    for( unsigned int i = 0; i < _dataSet.columns( ).size( ); i++ )
    {
      miniColumns = _dataSet.columns( )[ i ]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[ j ]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          neuron = neurons[ k ];
          neuronMesh =
            (( NeuronMorphologyPtr )neuron->morphology( ))->NeuronMesh( );
          std::vector< float > tMatrix;
          tMatrix.resize( 16 );
          for(int matrixRow = 0; matrixRow < 4; matrixRow++ )
          {
            for(int matrixCol = 0; matrixCol < 4; matrixCol++)
            {
              tMatrix[ matrixCol * 4 + matrixRow ] =
                neuron->transform( )[ matrixRow ][ matrixCol ];
            }
          }
          neuronMesh->PaintSoma( true );
#ifdef NEUROLOTS_USE_ZEQ

          if( _zeqConnection )
          {
            if( _IsSelected( neuron ) )
            {
              glUseProgram( _programQuads->id( ));
              glUniform3fv( _programQuads->uColor( ), 1,
                            _selectedNeuronColor.data( ));

              glUseProgram( _programTriangles->id( ));
              glUniform3fv( _programTriangles->uColor( ), 1,
                            _selectedNeuronColor.data( ));
              neuronMesh->PaintNeurites( true );
            }
            else
            {
              glUseProgram( _programQuads->id( ));
              glUniform3fv( _programQuads->uColor( ), 1,
                            _neuronColor.data( ));

              glUseProgram( _programTriangles->id( ));
              glUniform3fv( _programTriangles->uColor( ), 1,
                            _neuronColor.data( ));
              neuronMesh->PaintNeurites( false );
            }
          }
          else
          {
            glUseProgram( _programQuads->id( ));
            glUniform3fv( _programQuads->uColor( ), 1,
                          _neuronColor.data( ));

            glUseProgram( _programTriangles->id( ));
            glUniform3fv( _programTriangles->uColor( ), 1,
                          _neuronColor.data( ));
            neuronMesh->PaintNeurites( true );
          }
#else
          glUseProgram( _programQuads->id( ));
          glUniform3fv( _programQuads->uColor( ), 1,
              _neuronColor.data( ));

          glUseProgram( _programTriangles->id( ));
          glUniform3fv( _programTriangles->uColor( ), 1,
                          _neuronColor.data( ));
          neuronMesh->PaintNeurites( true );

#endif

          glUseProgram( _programQuads->id( ));
          glUniformMatrix4fv( _programQuads->uModel( ), 1, GL_FALSE,
                              tMatrix.data( ));

          glUseProgram( _programTriangles->id( ));
          glUniformMatrix4fv( _programTriangles->uModel( ), 1, GL_FALSE,
                              tMatrix.data( ));

          neuronMesh->Paint( );

        }
      }
    }
  }

  void NeuronsCollection::AddLod( float AddLod_ )
  {
    _lod += AddLod_;
    if ( _lod < 1.0f )
      _lod = 1.0f;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uLod( ), 1, &_lod );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uLod( ), 1, &_lod );
  }

  void NeuronsCollection::AddTng( float AddTng_ )
  {
    _tng += AddTng_;
    if ( _tng < 0.0f )
      _tng = 0.0f;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uTng( ), 1, &_tng );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uTng( ), 1, &_tng );
  }

  void NeuronsCollection::AddMaxDist( float AddMaxDist_ )
  {
    _maxDist += AddMaxDist_;
    if( _maxDist < 2 )
      _maxDist = 2;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uMaxDist( ), 1, &_maxDist );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uMaxDist( ), 1, &_maxDist );
  }

  // GETTERS

  ColumnsPtr NeuronsCollection::Columns( void )
  {
    return &_dataSet.columns( );
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
    glUniform1fv( _programQuads->uLod( ), 1, &_lod );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uLod( ), 1, &_lod );
  }

  void NeuronsCollection::Tng( float tng_ )
  {
    _tng = tng_;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uTng( ), 1, &_tng );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uTng( ), 1, &_tng );
  }

  void NeuronsCollection::MaxDist( float maxDist_ )
  {
    _maxDist = maxDist_;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uMaxDist( ), 1, &_maxDist );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uMaxDist( ), 1, &_maxDist );
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
            neuronMesh = new NeuronMesh( morpho, _programTriangles,
                                         _programQuads, _camera );
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

    std::cout << "pivot: " << center.x( ) << " " << center.y( ) << " "
        << center.z( ) << std::endl;
    std::cout << "radius: " << radius << std::endl;
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
