#include "NeuronsCollection.h"

namespace neurolots
{

  NeuronsCollection::NeuronsCollection( const char * file_name,
                                        Program * program_, Camera * camera_ )
    : _program( program_)
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

    glUseProgram( _program->id( ));

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

    glUseProgram( _program->id( ));

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

          glUniformMatrix4fv( _program->uModel( ), 1, GL_FALSE, tMatrix.data());
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

     glUseProgram( _program->id( ));

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
       glUniformMatrix4fv( _program->uModel( ), 1, GL_FALSE, tMatrix.data());
       neuronMesh->Paint( );
     }
   }

  void NeuronsCollection::PaintNeuron( unsigned int nColumn,
                                       unsigned int nMiniColumn,
                                       unsigned int nNeuron )
  {
    nsol::NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;


    glUseProgram( _program->id( ));

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
    glUniformMatrix4fv( _program->uModel( ), 1, GL_FALSE, tMatrix.data());
    neuronMesh->Paint( );
  }

  void NeuronsCollection::PaintNeuron( unsigned int nColumn,
                                       unsigned int nMiniColumn,
                                       unsigned int nNeuron,
                                       float x, float y, float z )
  {
    nsol::NeuronPtr neuron;
    NeuronMeshPtr neuronMesh;

    glUseProgram( _program->id( ));

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
    glUniformMatrix4fv( _program->uModel( ), 1, GL_FALSE, tMatrix.data());
    neuronMesh->Paint( );
  }

  void NeuronsCollection::Anim( void )
  {
    nsol::Neurons neurons;
    nsol::NeuronPtr n;
    nsol::NeuronMorphologyPtr morpho;
    NeuronMeshPtr neuronMesh;
    //First minicolum

    glUseProgram( _program->id( ));

    neurons =_colums[0]->miniColumns( )[0]->neurons( );

    _cont++;

    if( _cont >= neurons.size( )*100 )
    {
      _cont = 0;
    }

    std::cout << "cont " << _cont << " indice "<< _cont/100 <<std::endl;

    n = neurons[_cont/100];
    neuronMesh = ((NeuronMorphologyPtr)n->morphology( ))->NeuronMesh( );

    if(neuronMesh == nullptr )
       std::cout << "Morfología inexistente" << std::endl;

    std::vector< float > tMatrix;
    tMatrix.resize(16);
    for(int i = 0; i < 4; i++ )
    {
      for(int j = 0; j < 4; j++)
      {
        tMatrix[j*4+i] = n->transform()[i][j];
      }
    }
    glUniformMatrix4fv( _program->uModel( ), 1, GL_FALSE, tMatrix.data());
    neuronMesh->Paint( );


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
            neuronMesh = new NeuronMesh(morpho, _program, _camera );
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
      neuronMesh = new NeuronMesh(morpho, _program, _camera );
      morpho->NeuronMesh( neuronMesh );
    }
  }


}
