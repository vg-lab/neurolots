/**
 * @file    NeuronMeshGenerator.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLGENERATOR_NEURON_MESH_GENERATOR__
#define __NLGENERATOR_NEURON_MESH_GENERATOR__

#include <nsol/nsol.h>

#include <iostream>
#include <vector>

#include <Eigen/Dense>

#include "VectorizedNode.h"
#include "Icosphere.h"

#include <nlgenerator/api.h>

namespace nlgenerator
{

  typedef VectorizedNode * VectorizedNodePtr;

  class NeuronMeshGenerator
  {

  public:

    NLGENERATOR_API
    NeuronMeshGenerator( void );

    NLGENERATOR_API
    ~NeuronMeshGenerator( void );

    NLGENERATOR_API
    static void GenerateMesh(
      nsol::NeuronMorphologyPtr& morpho,
      std::vector< float >& vertices,
      std::vector< float >& centers,
      std::vector< float >& tangents,
      std::vector< unsigned int >& mesh,
      unsigned int& somaEnd);

    NLGENERATOR_API
    static void GenerateMesh(
      const nsol::NeuronMorphologyPtr& morpho,
      const float& alphaRadius,
      const std::vector< float >& alphaFirstNodes,
      std::vector< float >& vertices,
      std::vector< float >& centers,
      std::vector< float >& tangents,
      std::vector< unsigned int >& mesh,
      unsigned int& somaEnd);


    NLGENERATOR_API
    static void GenerateSoma(
      const nsol::SomaPtr& soma,
      std::vector< VectorizedNodePtr >& firstNodes,
      std::vector< float >& vertices,
      std::vector< float >& centers,
      std::vector< float >& tangents,
      std::vector< unsigned int >& mesh );

    NLGENERATOR_API
    static void GenerateSoma(
      const nsol::SomaPtr& soma,
      const float& alphaRadius,
      std::vector< VectorizedNodePtr >& firstNodes,
      const std::vector< float >& alphaFirstNodes,
      std::vector< float >& vertices,
      std::vector< float >& centers,
      std::vector< float >& tangents,
      std::vector< unsigned int >& mesh );

    NLGENERATOR_API
    static void VectorizeMorphology(
      const nsol::NeuronMorphologyPtr& morpho,
      std::vector< VectorizedNodePtr >& vNodes,
      std::vector< VectorizedNodePtr >& firstNodes);

    NLGENERATOR_API
    static void CalculateTangents( std::vector< VectorizedNodePtr >& vNodes );

    NLGENERATOR_API
    static void CalculateGeometry( std::vector< VectorizedNodePtr >& vNodes,
                                   std::vector< float >& vertices,
                                   std::vector< float >& centers,
                                   std::vector< float >& tangents );

  private:

    static void _VectorizeMorphology(
      const nsol::SectionPtr& section,
      std::vector< VectorizedNodePtr >& vNodes,
      VectorizedNodePtr vFatherNode );

    static unsigned int _NumNodes( const nsol::NeuronMorphologyPtr& morpho );
    static unsigned int _NumNodes( const nsol::SectionPtr& section );

    static unsigned int _MaxId( const nsol::NeuronMorphologyPtr& morpho );
    static unsigned int _MaxId( const nsol::SectionPtr& section );


    static void _CreateQuadPipe( const GeometricPrimitivePtr& geom0,
                                 const GeometricPrimitivePtr& geom1,
                                 std::vector< unsigned int >& mesh );

  };

} // end namespace nlgenerator

#endif // __NLGENERATOR_NEURON_MESH_GENERATOR__

//EOF
