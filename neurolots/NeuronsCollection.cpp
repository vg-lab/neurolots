#include "NeuronsCollection.h"

namespace neurolots
{

  NeuronsCollection::NeuronsCollection( const char * file_name,
                                        Program * program_, Camera * camera_ )
    : _program( program_)
    , _camera( camera_ )
  {

    nsol::BBPSDKReader r;
    _colums = r.readExperiment( file_name, 0);

    _GenerateMeshes( );

  }

  NeuronsCollection::~NeuronsCollection( void )
  {

  }

  // PUBLIC

  void NeuronsCollection::Init( void )
  {
    NeuronMeshPtr neuronMesh;
    for( TMorphoMap::iterator itt = _morphos.Begin( ); itt != _morphos.End( );
         itt++ )
    {
      neuronMesh = itt->second;
      neuronMesh->Init( );
    }
  }

  void NeuronsCollection::Paint( void )
  {
    nsol::ColumnPtr colum;
    nsol::MiniColumnPtr miniColum;
    nsol::NeuronPtr neuron;
    nsol::NeuronMorphologyPtr morpho;
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
          morpho = neuron->morphology( );
          neuronMesh =  _morphos.GetNeuronMesh( morpho );

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

  // PRIVATE

  void NeuronsCollection::_GenerateMeshes( void )
  {
    nsol::ColumnPtr colum;
    nsol::MiniColumnPtr miniColum;
    nsol::NeuronPtr neuron;
    nsol::NeuronMorphologyPtr morpho;
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
          morpho = neuron->morphology( );

          if(!_morphos.Exist(morpho))
          {
            neuronMesh = new NeuronMesh(morpho, _program, _camera );
            _morphos.Add( morpho, neuronMesh );
          }
        }
      }
    }
  }




}
