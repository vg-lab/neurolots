/**
 * @file    NeuronMeshGenerator.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_NEURON_MESH_GENERATOR__
#define __NEUROLOTS_NEURON_MESH_GENERATOR__

#include <nsol/nsol.h>

#include <iostream>
#include <vector>

#include <Eigen/Dense>

#include "VectorizedNode.h"
#include "Icosphere.h"

#include <neurolots/nlgenerator/api.h>

namespace neurolots
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
      static nsol::NeuronMorphologyPtr ReadMorphology( const char * file_name );

      NLGENERATOR_API
      static void GenerateStructure( nsol::NeuronMorphologyPtr morpho,
                                     std::vector< float > & vertices,
                                     std::vector< unsigned int> & mesh );

      NLGENERATOR_API
      static void GenerateStructure( nsol::NeuronMorphologyPtr morpho,
                                     std::vector< VectorizedNodePtr > & vNodes,
                                     std::vector< float > & vertices,
                                     std::vector< unsigned int> & mesh );
      NLGENERATOR_API
      static void GenerateStructure( std::vector< VectorizedNodePtr > & vNodes,
                                     std::vector< float > & vertices,
                                     std::vector< unsigned int> & mesh );
      NLGENERATOR_API
      static void GenerateMeshQuads( nsol::NeuronMorphologyPtr morpho,
                                std::vector< float >& vertices,
                                std::vector< float >& centers,
                                std::vector< float >& tangents,
                                std::vector< unsigned int >& mesh,
                                unsigned int& somaEnd);
      NLGENERATOR_API
      static void GenerateMeshQuads( std::vector< VectorizedNodePtr >& vNodes,
                                     std::vector< float >& vertices,
                                     std::vector< float >& centers,
                                     std::vector< float >& tangents,
                                     std::vector< unsigned int >& mesh );
      NLGENERATOR_API
      static void GenerateMeshTriangles( nsol::NeuronMorphologyPtr morpho,
                                      std::vector< float >& vertices,
                                      std::vector< float >& centers,
                                      std::vector< float >& tangents,
                                      std::vector< unsigned int >& mesh );
      NLGENERATOR_API
      static void GenerateMeshTriangles(
                                      std::vector< VectorizedNodePtr > & vNodes,
                                      std::vector< float >& vertices,
                                      std::vector< float >& centers,
                                      std::vector< float >& tangents,
                                      std::vector< unsigned int >& mesh );

      NLGENERATOR_API
      static void GenerateSomaTriangles( nsol::SomaPtr soma,
                                 std::vector< VectorizedNodePtr > & firstNodes,
                                 std::vector< float >& vertices,
                                 std::vector< float >& centers,
                                 std::vector< float >& tangents,
                                 std::vector< unsigned int >& mesh );
      NLGENERATOR_API
      static void GenerateSomaQuads( nsol::SomaPtr soma,
                                 std::vector< VectorizedNodePtr > & firstNodes,
                                 std::vector< float >& vertices,
                                 std::vector< float >& centers,
                                 std::vector< float >& tangents,
                                 std::vector< unsigned int >& mesh );

      NLGENERATOR_API
      static void VectorizeMorpho( nsol::NeuronMorphologyPtr morpho,
                                   std::vector< VectorizedNodePtr >& vNodes );

      NLGENERATOR_API
      static void VectorizeMorpho( nsol::NeuronMorphologyPtr morpho,
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

      static void _GenerateStructure( nsol::SectionPtr section,
                                      std::vector< float > & vertices,
                                      std::vector< unsigned int > & mesh,
                                      std::vector< unsigned int > & ids);
      static void _GenerateStructure( nsol::SectionPtr section,
                                     std::vector< VectorizedNodePtr > & vNodes,
                                     std::vector< float > & vertices,
                                     std::vector< unsigned int> & mesh );
      static void _GenerateMeshQuads( nsol::SectionPtr section,
                                 std::vector< float > & vertices,
                                 std::vector< float > & centers,
                                 std::vector< float > & tangents,
                                 std::vector< unsigned int > & mesh);
      static void _GenerateMeshTriangles( nsol::SectionPtr section,
                                       std::vector< float > & vertices,
                                       std::vector< float > & centers,
                                       std::vector< float >& tangents,
                                       std::vector< unsigned int > & mesh);

      static void _VectorizeMorpho( nsol::SectionPtr section,
                                    std::vector< VectorizedNodePtr > & vNodes,
                                    VectorizedNodePtr vFatherNode );

      static unsigned int _NumNodes( nsol::NeuronMorphologyPtr morpho );
      static unsigned int _NumNodes( nsol::SectionPtr section );

      static unsigned int _MaxId( nsol::NeuronMorphologyPtr morpho );
      static unsigned int _MaxId( nsol::SectionPtr section );

      static void _CreateSphereQuads( Eigen::Vector3f center, float radius,
                                 std::vector< float > & vertices,
                                 std::vector< float > & centers,
                                 std::vector< float > & tangents,
                                 std::vector< unsigned int > & mesh );

      static void _CreateSphereTriangles( Eigen::Vector3f center, float radius,
                                       std::vector< float > & vertices,
                                       std::vector< float > & centers,
                                       std::vector< float >& tangents,
                                       std::vector< unsigned int > & mesh );
      static void _CreateTrianglePipe( GeometricPrimitivePtr geom0,
                               GeometricPrimitivePtr geom1,
                               std::vector< unsigned int > & mesh );

      static void _CreateQuadPipe( GeometricPrimitivePtr geom0,
                                     GeometricPrimitivePtr geom1,
                                     std::vector< unsigned int > & mesh );


  };

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MESH_GENERATOR__

//EOF
