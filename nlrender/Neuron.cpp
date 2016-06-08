/**
 * @file    Neuron.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "Neuron.h"
#include "NeuronMorphology.h"

#include <cfloat>

namespace nlrender
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
    _init( );
  }

  Neuron::~Neuron( void )
  {

  }

  nsol::NeuronMorphologyPtr Neuron::morphology( void ) const
  {
    return _morphology;
  }

  void Neuron::morphology( nsol::NeuronMorphologyPtr newMorphology )
  {
    this->nsol::Neuron::morphology( newMorphology );
    _calculateBoundingBox( );
  }

  std::vector< float >&  Neuron::vecTransform( void )
  {
    return _vecTransform;
  }

  void Neuron::regenerateMesh( const float& alphaRadius_,
                               const std::vector< float >& alphaNeurites_ )
    {
      (( nlrender::NeuronMorphologyPtr ) _morphology )->NeuronMesh( )->
        regenerate( alphaRadius_, alphaNeurites_ );
    }

  // GETTERS

  TBoundingBox Neuron::BoundingBox( void )
  {
    return _boundingBox;
  }

  // SETTERS


  // PRIVATE

  void Neuron::_init( void )
  {
    _vecTransform.resize( 16 );

    for( int matrixRow = 0; matrixRow < 4; matrixRow++ )
    {
      for( int matrixCol = 0; matrixCol < 4; matrixCol++ )
      {
        _vecTransform[ matrixCol * 4 + matrixRow ] =
          _transform( matrixRow , matrixCol );
      }
    }
    _calculateBoundingBox( );
  }

  void Neuron::_calculateBoundingBox( void )
  {
    _boundingBox.xMax = FLT_MIN;
    _boundingBox.xMin = FLT_MAX;
    _boundingBox.yMax = FLT_MIN;
    _boundingBox.yMin = FLT_MAX;
    _boundingBox.zMax = FLT_MIN;
    _boundingBox.zMin = FLT_MAX;

    float radius;
    nsol::Vec3f center;

    if( _morphology )
    {
      radius = _morphology->soma( )->maxRadius( );
      center = _morphology->soma( )->center( );
    }
    else{
      radius = 0.0f;
      center = nsol::Vec3f( 0, 0, 0 );
    }

    nsol::Vec4f position = _transform * nsol::Vec4f( center.x( ) , center.y( ),
                                                     center.z( ), 1.0f );
    _boundingBox.xMin = position.x( ) - radius;
    _boundingBox.xMax = position.x( ) + radius;

    _boundingBox.yMin = position.y( ) - radius;
    _boundingBox.yMax = position.y( ) + radius;


    _boundingBox.zMin = position.z( ) - radius;
    _boundingBox.zMax = position.z( ) + radius;
  }

} // end namespace nlrender
