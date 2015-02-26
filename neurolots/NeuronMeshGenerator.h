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

namespace neurolots
{

  class NeuronMeshGenerator
  {

    public:

      NeuronMeshGenerator( void );
      ~NeuronMeshGenerator( void );


      static nsol::NeuronMorphologyPtr ReadMorphology( const char * file_name );
      static void GenerateStructure( nsol::NeuronMorphologyPtr morpho,
                                     std::vector< float > & vertices,
                                     std::vector< unsigned int> & mesh );
      static void GenerateMesh( nsol::NeuronMorphologyPtr morpho,
                                std::vector< float > & vertices,
                                std::vector< float > & centers,
                                std::vector< float > & tangents,
                                std::vector< unsigned int > & mesh );


    private:

      static void _GenerateStructure( nsol::SectionPtr section,
                                      std::vector< float > & vertices,
                                      std::vector< unsigned int > & mesh,
                                      std::vector< unsigned int > & ids);
      static void _GenerateMesh( nsol::SectionPtr section,
                                 std::vector< float > & vertices,
                                 std::vector< float > & centers,
                                 std::vector< float > & tangents,
                                 std::vector< unsigned int > & mesh);

      static unsigned int _NumNodes( nsol::NeuronMorphologyPtr morpho );
      static unsigned int _NumNodes( nsol::SectionPtr section );

      static unsigned int _MaxId( nsol::NeuronMorphologyPtr morpho );
      static unsigned int _MaxId( nsol::SectionPtr section );

      static void _CreateSphere( Eigen::Vector3f center, float radius,
                                 std::vector< float > & vertices,
                                 std::vector< float > & centers,
                                 std::vector< float > & tangents,
                                 std::vector< unsigned int > & mesh );

  };

} // end namespace neurolots

#endif // __NEUROLOTS_NEURON_MESH_GENERATOR__

//EOF
