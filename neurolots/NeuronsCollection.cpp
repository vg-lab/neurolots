#include "NeuronsCollection.h"

namespace neurolots
{

  NeuronsCollection::NeuronsCollection( const char * file_name,
                                        Program * programTriangles_,
                                        Program * programQuads_,
                                        Camera * camera_ )
    : _programTriangles( programTriangles_ )
    , _programQuads( programQuads_ )
    , _camera( camera_ )
    , _cont( 0 )
  {

    std::string fName(file_name);

    if ( !(fName.length() < 5 ||
         fName.compare( fName.length( ) - 3, 3, "swc" )))
    {
      std::cout << "Leyendo swc" << std::endl;

           nsol::SwcReaderTemplated< nsol::Node,
                                     nsol::Segment,
                                     nsol::Section,
                                     nsol::Dendrite,
                                     nsol::Axon,
                                     nsol::Soma,
                                     NeuronMorphology,
                                     nsol::Neuron > swcReader;

           nsol::NeuronPtr neuron = swcReader.readNeuron( file_name );
           nsol::MiniColumnPtr miniColumn = new nsol::MiniColumn( );
           miniColumn->addNeuron( neuron );
           nsol::ColumnPtr column = new nsol::Column();
           column->addMiniColumn( miniColumn );

           _colums.clear( );

           _colums.insert( std::map< const  unsigned int , nsol::ColumnPtr >
                           ::value_type( 0 , column ));

    }

#ifdef NSOL_WITH_BBPSDK
    else
    {
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

            _colums = bbpsdkReader.readExperiment( file_name, 0);
    }
#endif
    _GenerateMeshes( );

  }

  NeuronsCollection::~NeuronsCollection( void )
  {

  }

  // PUBLIC

  void NeuronsCollection::Init( void )
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

  void NeuronsCollection::Paint( void )
  {
    nsol::MiniColumns miniColumns;
    nsol::Neurons neurons;
    nsol::NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;

    float lod;
    float tng;
    float max;

    lod = _programQuads->lod( );
    tng = _programQuads->tng( );
    max = _programQuads->maxDist( );

    std::vector< float > color;
    color.push_back( 0.2 ); color.push_back( 0.5 ); color.push_back( 0.7 );

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                        _camera->GetViewMatrix());
    glUniformMatrix4fv( _programQuads->uProy( ), 1, GL_FALSE,
                        _camera->GetProjectionMatrix( ));
    glUniform3fv( _programQuads->uCameraPos( ), 1, _camera->GetCameraPos( ));
    glUniform3fv( _programQuads->uColor( ), 1, color.data( ));

    glUniform1fv( _programQuads->uLod( ), 1, &lod);
    glUniform1fv( _programQuads->uTng( ), 1, &tng);
    glUniform1fv( _programQuads->uMaxDist( ), 1, &max);

    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( _programTriangles->uView( ), 1, GL_FALSE,
                        _camera->GetViewMatrix());
    glUniformMatrix4fv( _programTriangles->uProy( ), 1, GL_FALSE,
                        _camera->GetProjectionMatrix( ));
    glUniform3fv( _programTriangles->uCameraPos( ), 1,
                  _camera->GetCameraPos( ));
    glUniform3fv( _programTriangles->uColor( ), 1, color.data( ));

    glUniform1fv( _programQuads->uLod( ), 1, &lod);
    glUniform1fv( _programQuads->uTng( ), 1, &tng);
    glUniform1fv( _programQuads->uMaxDist( ), 1, &max);

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
    }

  }

  void NeuronsCollection::PaintMiniColum( unsigned int nColumn,
                                          unsigned int nMiniColumn )
   {
     nsol::Neurons neurons;
     nsol::NeuronPtr neuron;
     NeuronMeshPtr neuronMesh;

     float lod;
        float tng;
        float max;

        lod = _programQuads->lod( );
        tng = _programQuads->tng( );
        max = _programQuads->maxDist( );

        std::vector< float > color0;
        color0.push_back( 0.3 ); color0.push_back( 0.5 ); color0.push_back( 0.7 );

        std::vector< float > color1;
        color1.push_back( 0.7 ); color1.push_back( 0.5 ); color1.push_back( 0.3 );


        glUseProgram( _programQuads->id( ));
        glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                            _camera->GetViewMatrix());
        glUniformMatrix4fv( _programQuads->uProy( ), 1, GL_FALSE,
                            _camera->GetProjectionMatrix( ));
        glUniform3fv( _programQuads->uCameraPos( ), 1, _camera->GetCameraPos( ));
        ;

        glUniform1fv( _programQuads->uLod( ), 1, &lod);
        glUniform1fv( _programQuads->uTng( ), 1, &tng);
        glUniform1fv( _programQuads->uMaxDist( ), 1, &max);

        glUseProgram( _programTriangles->id( ));
        glUniformMatrix4fv( _programTriangles->uView( ), 1, GL_FALSE,
                            _camera->GetViewMatrix());
        glUniformMatrix4fv( _programTriangles->uProy( ), 1, GL_FALSE,
                            _camera->GetProjectionMatrix( ));
        glUniform3fv( _programTriangles->uCameraPos( ), 1,
                      _camera->GetCameraPos( ));

        glUniform1fv( _programQuads->uLod( ), 1, &lod);
        glUniform1fv( _programQuads->uTng( ), 1, &tng);
        glUniform1fv( _programQuads->uMaxDist( ), 1, &max);

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
       glUniform3fv( _programQuads->uColor( ), 1, color0.data( ));

       glUseProgram( _programTriangles->id( ));
       glUniformMatrix4fv( _programTriangles->uModel( ), 1, GL_FALSE,
                                     tMatrix.data());
       glUniform3fv( _programQuads->uColor( ), 1, color1.data( ));

       neuronMesh->Paint( );
     }
   }

  void NeuronsCollection::PaintNeuron( unsigned int nColumn,
                                       unsigned int nMiniColumn,
                                       unsigned int nNeuron )
  {
    nsol::NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;


    float lod;
    float tng;
    float max;

    lod = _programQuads->lod( );
    tng = _programQuads->tng( );
    max = _programQuads->maxDist( );

    std::vector< float > color;
    color.push_back( 0.2 ); color.push_back( 0.5 ); color.push_back( 0.7 );

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                        _camera->GetViewMatrix());
    glUniformMatrix4fv( _programQuads->uProy( ), 1, GL_FALSE,
                        _camera->GetProjectionMatrix( ));
    glUniform3fv( _programQuads->uCameraPos( ), 1, _camera->GetCameraPos( ));
    glUniform3fv( _programQuads->uColor( ), 1, color.data( ));

    glUniform1fv( _programQuads->uLod( ), 1, &lod);
    glUniform1fv( _programQuads->uTng( ), 1, &tng);
    glUniform1fv( _programQuads->uMaxDist( ), 1, &max);


    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( _programTriangles->uView( ), 1, GL_FALSE,
                       _camera->GetViewMatrix());
    glUniformMatrix4fv( _programTriangles->uProy( ), 1, GL_FALSE,
                       _camera->GetProjectionMatrix( ));
    glUniform3fv( _programTriangles->uCameraPos( ), 1,
                 _camera->GetCameraPos( ));
    glUniform3fv( _programTriangles->uColor( ), 1, color.data( ));

    glUniform1fv( _programQuads->uLod( ), 1, &lod);
    glUniform1fv( _programQuads->uTng( ), 1, &tng);
    glUniform1fv( _programQuads->uMaxDist( ), 1, &max);

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

    float lod;
    float tng;
    float max;

    lod = _programQuads->lod( );
    tng = _programQuads->tng( );
    max = _programQuads->maxDist( );

    std::vector< float > color;
    color.push_back( 0.2 ); color.push_back( 0.5 ); color.push_back( 0.7 );

    glUseProgram( _programQuads->id( ));
    glUniformMatrix4fv( _programQuads->uView( ), 1, GL_FALSE,
                        _camera->GetViewMatrix());
    glUniformMatrix4fv( _programQuads->uProy( ), 1, GL_FALSE,
                        _camera->GetProjectionMatrix( ));
    glUniform3fv( _programQuads->uCameraPos( ), 1, _camera->GetCameraPos( ));
    glUniform3fv( _programQuads->uColor( ), 1, color.data( ));

    glUniform1fv( _programQuads->uLod( ), 1, &lod);
    glUniform1fv( _programQuads->uTng( ), 1, &tng);
    glUniform1fv( _programQuads->uMaxDist( ), 1, &max);


    glUseProgram( _programTriangles->id( ));
    glUniformMatrix4fv( _programTriangles->uView( ), 1, GL_FALSE,
                       _camera->GetViewMatrix());
    glUniformMatrix4fv( _programTriangles->uProy( ), 1, GL_FALSE,
                       _camera->GetProjectionMatrix( ));
    glUniform3fv( _programTriangles->uCameraPos( ), 1,
                 _camera->GetCameraPos( ));
    glUniform3fv( _programTriangles->uColor( ), 1, color.data( ));
    glUniform1fv( _programQuads->uLod( ), 1, &lod);
    glUniform1fv( _programQuads->uTng( ), 1, &tng);
    glUniform1fv( _programQuads->uMaxDist( ), 1, &max);

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

  // PRIVATE

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
