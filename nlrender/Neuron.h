/**
 * @file    Neuron.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLRENDER_NEURON__
#define __NLRENDER_NEURON__

#include <nsol/nsol.h>

#include <nlrender/api.h>

namespace nlrender
{
  typedef struct
  {
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zMin;
    float zMax;
  } TBoundingBox;

  class Neuron : public nsol::Neuron
  {

  public:

    NLRENDER_API
    Neuron( const nsol::NeuronMorphologyPtr neuronMorphology_ = nullptr,
            const unsigned short layer_ = 0,
            const unsigned int gid_ = 0,
            const nsol::Matrix4_4f transform_ = nsol::Matrix4_4fIdentity,
            const nsol::MiniColumnPtr miniColum_ = nullptr,
            const TMorphologicalType morphologicalType_ =
            nsol::Neuron::UNDEFINED,
            const TFunctionalType functionalType_ =
            nsol::Neuron::UNDEFINED_FUNCTIONAL_TYPE );

    NLRENDER_API
    ~Neuron( void );

    NLRENDER_API
    nsol::NeuronMorphologyPtr morphology( void ) const;

    NLRENDER_API
    void morphology( nsol::NeuronMorphologyPtr newMorphology );

    NLRENDER_API
    std::vector< float >& vecTransform( void );

    NLRENDER_API
    void regenerateMesh( const float& alphaRadius_,
                         const std::vector< float >& alphaNeurites_ );

    // GETTERS
    NLRENDER_API
    TBoundingBox BoundingBox( void );

    // SETTERS

  private:

    void _init( void );

    void _calculateBoundingBox( void );

    std::vector< float > _vecTransform;
    TBoundingBox _boundingBox;

  };

  typedef Neuron* NeuronPtr;

} // end namespace nlrender

#endif // __NLRENDER_NEURON__
