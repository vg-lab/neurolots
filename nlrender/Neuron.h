#ifndef __NEUROLOTS_NEURON__
#define __NEUROLOTS_NEURON__

#include <nsol/nsol.h>

#include <neurolots/nlrender/api.h>

namespace neurolots
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
            const nsol::Matrix4_4f transform_ = nsol::Matrix4_4f::IDENTITY,
            const nsol::MiniColumnPtr miniColum_ = nullptr,
            const TMorphologicalType morphologicalType_ =
            nsol::Neuron::UNDEFINED,
            const TFunctionalType functionalType_ =
            nsol::Neuron::UNDEFINED_FUNCTIONAL_TYPE );

    NLRENDER_API
    ~Neuron( void );

    NLRENDER_API
    void Init( void );

    NLRENDER_API
    std::vector< float >& vecTransform( void );

    // GETTERS
    NLRENDER_API
    TBoundingBox BoundingBox( void );

    // SETTERS

  private:

    void _CalculateBoundingBox( void );

    std::vector< float > _vecTransform;
    TBoundingBox _boundingBox;

  };

  typedef Neuron* NeuronPtr;

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON__
