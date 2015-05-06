#include "NeuronsCollection.h"

namespace neurolots
{

  NeuronsCollection::NeuronsCollection( const char * file_name,
                                        const char * quadsPath,
                                        const char * trianglesPath,
                                        Camera * camera_ )
    : _camera( camera_ )
    , _cont( 0 )
  {
    _programQuads = new Program( Program::QUADS, quadsPath );
    _programTriangles = new Program( Program::TRIANGLES, trianglesPath );
    _programQuads->Init();
    _programTriangles->Init();

    Lod( 3.0f );
    Tng( 5.0f );
    MaxDist( 200.0f );

    NeuritesColor( Eigen::Vector3f( 0.3f, 0.3f, 0.3f));
    SomaColor( Eigen::Vector3f( 0.3f, 0.3f, 0.3f));

    std::string fName(file_name);

    if ( !(fName.length() < 5 ||
         fName.compare( fName.length( ) - 3, 3, "swc" )))
    {
      nsol::SwcReaderTemplated< nsol::Node,
                                nsol::Segment,
                                nsol::Section,
                                nsol::Dendrite,
                                nsol::Axon,
                                nsol::Soma,
                                NeuronMorphology,
                                nsol::Neuron > swcReader;

      nsol::NeuronPtr neuron = swcReader.readNeuron( file_name );
      if(neuron == nullptr)
      {
        std::cerr << "Error: Swc file doesn't exits" << std::endl;
        exit( -1 );
      }

      nsol::MiniColumnPtr miniColumn = new nsol::MiniColumn( );
      miniColumn->addNeuron( neuron );
      nsol::ColumnPtr column = new nsol::Column();
      column->addMiniColumn( miniColumn );

      _colums.clear( );

      _colums.insert( std::map< const  unsigned int , nsol::ColumnPtr >
                   ::value_type( 0 , column ));
    }
    else
    {
#ifdef NEUROLOTS_WITH_BBPSDK
      nsol::BBPSDKReaderTemplated< nsol::Node,
                                   nsol::Segment,
                                   nsol::Section,
                                   nsol::Dendrite,
                                   nsol::Axon,
                                   nsol::Soma,
                                   NeuronMorphology,
                                   nsol::Neuron,
                                   nsol::MiniColumn,
                                   nsol::Column > bbpsdkReader;

      try{
      _colums = bbpsdkReader.readExperiment( file_name, 0);
      }
      catch( int e )
      {
        std::cerr << "Error: can't load file: " << file_name << std::endl;
        exit(-1);
      }
#else
      std::cerr << "Error: Bbpsdk support not built-in" << std::endl;
      exit( -1 );
#endif
    }


    _GenerateMeshes( );
    _Init();
  }

  NeuronsCollection::~NeuronsCollection( void )
  {

  }

  // PUBLIC

  void NeuronsCollection::Paint( void )
  {
    nsol::MiniColumns miniColumns;
    nsol::Neurons neurons;
    nsol::NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                        _camera->ViewMatrix());
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


    for( unsigned int i = 0; i < _colums.size(); i++ )
    {
      miniColumns = _colums[i]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[j]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          neuron = neurons[k];
          neuronMesh = ((NeuronMorphologyPtr)neuron->morphology( ))->NeuronMesh( );
          std::vector< float > tMatrix;
          tMatrix.resize(16);
          for(int matrixRow = 0; matrixRow < 4; matrixRow++ )
          {
            for(int matrixCol = 0; matrixCol < 4; matrixCol++)
            {
              tMatrix[matrixCol*4+matrixRow] = 
		neuron->transform()[matrixRow][matrixCol];
            }
          }

          glUseProgram( _programQuads->id( ));
          glUniformMatrix4fv( _programQuads->uModel( ), 1, GL_FALSE,
                              tMatrix.data());

          glUseProgram( _programTriangles->id( ));
          glUniformMatrix4fv( _programTriangles->uModel( ), 1, GL_FALSE,
                              tMatrix.data());


          neuronMesh->Paint( );

        }
      }
    }

  }

  void NeuronsCollection::PaintMiniColum( unsigned int nColumn,
                                          unsigned int nMiniColumn )
   {
     nsol::Neurons neurons;
     nsol::NeuronPtr neuron;
     NeuronMeshPtr neuronMesh;

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                        _camera->ViewMatrix());
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

     //First minicolum
     neurons =_colums[nColumn]->miniColumns( )[nMiniColumn]->neurons( );
     for( unsigned int k = 0; k < neurons.size( ); k++ )
     {
       neuron = neurons[k];
       neuronMesh = ((NeuronMorphologyPtr)neuron->morphology( ))->NeuronMesh( );

       std::vector< float > tMatrix;
       tMatrix.resize(16);
       for(int i = 0; i < 4; i++ )
       {
         for(int j = 0; j < 4; j++)
         {
           tMatrix[j*4+i] = neuron->transform()[i][j];
         }
       }
       glUseProgram( _programQuads->id( ));
       glUniformMatrix4fv( _programQuads->uModel( ), 1, GL_FALSE,
                                     tMatrix.data());

       glUseProgram( _programTriangles->id( ));
       glUniformMatrix4fv( _programTriangles->uModel( ), 1, GL_FALSE,
                                     tMatrix.data());

       neuronMesh->Paint( );
     }
   }

  void NeuronsCollection::PaintNeuron( unsigned int nColumn,
                                       unsigned int nMiniColumn,
                                       unsigned int nNeuron )
  {
    nsol::NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                        _camera->ViewMatrix());
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

    neuron = _colums[nColumn]->miniColumns( )[nMiniColumn]->
             neurons( )[nNeuron];

    neuronMesh = ((NeuronMorphologyPtr)neuron->morphology( ))->NeuronMesh( );

    std::vector< float > tMatrix;
    tMatrix.resize(16);
    for(int i = 0; i < 4; i++ )
    {
      for(int j = 0; j < 4; j++)
      {
        tMatrix[j*4+i] = neuron->transform()[i][j];
      }
    }
    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uModel( ), 1, GL_FALSE,
                                  tMatrix.data());

    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( _programTriangles->uModel( ), 1, GL_FALSE,
                                  tMatrix.data());

    neuronMesh->Paint( );
  }

  void NeuronsCollection::PaintNeuron( unsigned int nColumn,
                                       unsigned int nMiniColumn,
                                       unsigned int nNeuron,
                                       float x, float y, float z )
  {
    nsol::NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                        _camera->ViewMatrix());
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

    neuron = _colums[nColumn]->miniColumns( )[nMiniColumn]->
           neurons( )[nNeuron];
    neuronMesh = ((NeuronMorphologyPtr)neuron->morphology( ))->NeuronMesh( );

    std::vector< float > tMatrix;
    tMatrix.resize(16);
    for(int i = 0; i < 4; i++ )
    {
      for(int j = 0; j < 4; j++)
      {
        tMatrix[j*4+i] = neuron->transform()[i][j];
      }
    }
    tMatrix[3*4 + 0] = x;
    tMatrix[3*4 + 1] = y;
    tMatrix[3*4 + 2] = z;
    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uModel( ), 1, GL_FALSE,
                                  tMatrix.data());

    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( _programTriangles->uModel( ), 1, GL_FALSE,
                                  tMatrix.data());

    neuronMesh->Paint( );
  }

  void NeuronsCollection::AddLod( float AddLod_ )
  {
    _lod += AddLod_;
    if ( _lod < 1.0f )
          _lod = 1.0f;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uLod( ), 1, &_lod);

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uLod( ), 1, &_lod);
  }

  void NeuronsCollection::AddTng( float AddTng_ )
  {
    _tng += AddTng_;
    if ( _tng < 0.0f )
      _tng = 0.0f;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uTng( ), 1, &_tng);

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uTng( ), 1, &_tng);
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

  //Getters

  ColumnsPtr NeuronsCollection::getColumns( void )
  {
    return &_colums;
  }

  //Setters

  void NeuronsCollection::PaintSoma( bool paintSoma )
  {
    nsol::MiniColumns miniColumns;
    nsol::NeuronPtr neuron;
    nsol::Neurons neurons;
    NeuronMorphologyPtr morpho;
    NeuronMeshPtr neuronMesh;


    for( unsigned int i = 0; i < _colums.size(); i++ )
    {
      miniColumns = _colums[i]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[j]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          neuron = neurons[k];
          morpho = (NeuronMorphologyPtr)neuron->morphology( );
          neuronMesh = morpho->NeuronMesh( );

          neuronMesh->PaintSoma( paintSoma );

        }
      }
    }
  }

  void NeuronsCollection::PaintNeurites( bool paintNeurites )
  {
    nsol::MiniColumns miniColumns;
    nsol::NeuronPtr neuron;
    nsol::Neurons neurons;
    NeuronMorphologyPtr morpho;
    NeuronMeshPtr neuronMesh;


    for( unsigned int i = 0; i < _colums.size(); i++ )
    {
      miniColumns = _colums[i]->miniColumns( );
      for( unsigned int j = 0; j < miniColumns.size( ); j++ )
      {
        neurons = miniColumns[j]->neurons( );
        for( unsigned int k = 0; k < neurons.size( ); k++ )
        {
          neuron = neurons[k];
          morpho = (NeuronMorphologyPtr)neuron->morphology( );
          neuronMesh = morpho->NeuronMesh( );

          neuronMesh->PaintNeurites( paintNeurites );

        }
      }
    }
  }

  void NeuronsCollection::Lod( float lod_ )
  {
    _lod = lod_;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uLod( ), 1, &_lod);

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uLod( ), 1, &_lod);
  }

  void NeuronsCollection::Tng( float tng_ )
  {
    _tng = tng_;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uTng( ), 1, &_tng);

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uTng( ), 1, &_tng);
  }

  void NeuronsCollection::MaxDist( float maxDist_ )
  {
    _maxDist = maxDist_;

    glUseProgram( _programQuads->id( ));
    glUniform1fv( _programQuads->uMaxDist( ), 1, &_maxDist );

    glUseProgram( _programTriangles->id( ));
    glUniform1fv( _programTriangles->uMaxDist( ), 1, &_maxDist );
  }

  void NeuronsCollection::NeuritesColor( Eigen::Vector3f neuritesColor_ )
  {
    _neuritesColor.resize( 3 );
    _neuritesColor[0] = neuritesColor_.x( );
    _neuritesColor[1] = neuritesColor_.y( );
    _neuritesColor[2] = neuritesColor_.z( );

    glUseProgram( _programQuads->id( ));
    glUniform3fv( _programQuads->uColor( ), 1, _neuritesColor.data( ));

  }

  void NeuronsCollection::SomaColor( Eigen::Vector3f somaColor_ )
  {
    _somaColor.resize( 3 );
    _somaColor[0] = somaColor_.x( );
    _somaColor[1] = somaColor_.y( );
    _somaColor[2] = somaColor_.z( );

    glUseProgram( _programTriangles->id( ));
    glUniform3fv( _programTriangles->uColor( ), 1, _somaColor.data( ));
  }

  void NeuronsCollection::NeuronColor( Eigen::Vector3f neuronColor_ )
  {
    _neuritesColor.resize( 3 );
    _neuritesColor[0] = neuronColor_.x( );
    _neuritesColor[1] = neuronColor_.y( );
    _neuritesColor[2] = neuronColor_.z( );

    glUseProgram( _programQuads->id( ));
    glUniform3fv( _programQuads->uColor( ), 1, _neuritesColor.data( ));

    _somaColor.resize( 3 );
    _somaColor[0] = neuronColor_.x( );
    _somaColor[1] = neuronColor_.y( );
    _somaColor[2] = neuronColor_.z( );

    glUseProgram( _programTriangles->id( ));
    glUniform3fv( _programTriangles->uColor( ), 1, _somaColor.data( ));
  }

  // PRIVATE

  void NeuronsCollection::_Init( void )
    {
      nsol::MiniColumns miniColumns;
      nsol::NeuronPtr neuron;
      nsol::Neurons neurons;
      NeuronMorphologyPtr morpho;
      NeuronMeshPtr neuronMesh;


      for( unsigned int i = 0; i < _colums.size(); i++ )
      {
        miniColumns = _colums[i]->miniColumns( );
        for( unsigned int j = 0; j < miniColumns.size( ); j++ )
        {
          neurons = miniColumns[j]->neurons( );
          for( unsigned int k = 0; k < neurons.size( ); k++ )
          {
            neuron = neurons[k];
            morpho = (NeuronMorphologyPtr)neuron->morphology( );
            neuronMesh = morpho->NeuronMesh( );

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
    for( unsigned int i = 0; i < _colums.size(); i++ )
    {
      colum = _colums[i];
      for( unsigned int j = 0; j < colum->miniColumns( ).size( ); j++ )
      {
        miniColum = colum->miniColumns( )[j];
        for( unsigned int k = 0; k < miniColum->neurons( ).size( ); k++ )
        {
          neuron = miniColum->neurons( )[k];
          morpho = (NeuronMorphologyPtr)neuron->morphology( );

          if( !morpho->HasNeuronMesh() )
          {
            neuronMesh = new NeuronMesh( morpho, _programTriangles,
                                         _programQuads, _camera );
            morpho->NeuronMesh( neuronMesh );
          }
        }
      }
    }
  }

  void NeuronsCollection::_GenerateMeshes( unsigned int nColumn,
                                           unsigned int nMiniColumn,
                                           unsigned int nNeuron  )
  {
    nsol::NeuronPtr neuron;
    NeuronMorphologyPtr morpho;
    NeuronMeshPtr neuronMesh;

    neuron = _colums[nColumn]->miniColumns()[nMiniColumn]->neurons( )[nNeuron];
    morpho = ( NeuronMorphologyPtr )neuron->morphology( );

    if( !morpho->HasNeuronMesh() )
    {
      neuronMesh = new NeuronMesh( morpho, _programTriangles,
                                   _programQuads, _camera );
      morpho->NeuronMesh( neuronMesh );
    }
  }


}
