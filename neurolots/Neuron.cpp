#include "Neuron.h"

#include <cfloat>

namespace neurolots
{

  Neuron::Neuron( const nsol::NeuronMorphologyPtr neuronMorphology_,
                  const unsigned short layer_,
                  const unsigned int gid_,
                  const nsol::Matrix4_4f transform_,
                  const nsol::MiniColumnPtr miniColum_,
                  const TNeuronType type_ )
    : nsol::Neuron( neuronMorphology_, layer_, gid_, transform_, miniColum_,
        type_ )
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
    _CalculateBoundingBox( );
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

    nsol::Vec4f position = _transform * nsol::Vec4f( center.x( ) , center.y( ), center.z( ),
        1.0f );

    _boundingBox.xMin = position.x( ) - radius;
    _boundingBox.xMax = position.x( ) + radius;

    _boundingBox.yMin = position.y( ) - radius;
    _boundingBox.yMax = position.y( ) + radius;


    _boundingBox.zMin = position.z( ) - radius;
    _boundingBox.zMax = position.z( ) + radius;
  }

} // end namespace neurolots
