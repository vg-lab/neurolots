#include "Neuron.h"

#include <cfloat>

namespace neurolots
{

  Neuron::Neuron( const nsol::NeuronMorphologyPtr neuronMorphology_,
                  const unsigned short layer_,
                  const unsigned int gid_,
                  const nsol::Matrix4_4f transform_,
                  const nsol::MiniColumnPtr miniColum_,
                  const TMorphologicalType morphologicalType_,
                  const TFunctionalType functionalType_ )
    : nsol::Neuron( neuronMorphology_, layer_, gid_, transform_, miniColum_,
        morphologicalType_, functionalType_ )
  {
    _boundingBox.xMax = FLT_MIN;
    _boundingBox.xMin = FLT_MAX;
    _boundingBox.yMax = FLT_MIN;
    _boundingBox.yMin = FLT_MAX;
    _boundingBox.zMax = FLT_MIN;
    _boundingBox.zMin = FLT_MAX;
  }

  Neuron::~Neuron( void )
  {

  }

  void Neuron::Init( void )
  {
    _vecTransform.resize( 16 );

    for(int matrixRow = 0; matrixRow < 4; matrixRow++ )
    {
      for(int matrixCol = 0; matrixCol < 4; matrixCol++)
      {
        _vecTransform[ matrixCol * 4 + matrixRow ] =
          _transform[ matrixRow ][ matrixCol ];
      }
    }
    _CalculateBoundingBox( );
  }

  std::vector< float >&  Neuron::vecTransform( void )
  {
    return _vecTransform;
  }

  // GETTERS

  TBoundingBox Neuron::BoundingBox( void )
  {
    return _boundingBox;
  }

  // SETTERS


  // PRIVATE

  void Neuron::_CalculateBoundingBox( void )
  {
    float radius = _morphology->soma( )->maxRadius( );
    nsol::Vec3f center = _morphology->soma( )->center( );

    nsol::Vec4f position = _transform * nsol::Vec4f( center.x( ) , center.y( ),
                                                     center.z( ), 1.0f );

    _boundingBox.xMin = position.x( ) - radius;
    _boundingBox.xMax = position.x( ) + radius;

    _boundingBox.yMin = position.y( ) - radius;
    _boundingBox.yMax = position.y( ) + radius;


    _boundingBox.zMin = position.z( ) - radius;
    _boundingBox.zMax = position.z( ) + radius;
  }

} // end namespace neurolots
