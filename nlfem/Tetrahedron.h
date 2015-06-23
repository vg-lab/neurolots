/**
 * @file    Tetrahedron.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __FEM_TETRAHEDRON__
#define __FEM_TETRAHEDRON__

//Eigen
#include <Eigen/Dense>

#include "Node.h"

#include <iostream>
#include <vector>

#include <neurolots/api.h>


namespace fem
{


  class Tetrahedron
  {

    public:

      NEUROLOTS_API
      Tetrahedron( NodePtr node0_, NodePtr node1_, NodePtr node2_,
                   NodePtr node3_ );

      NEUROLOTS_API
      ~Tetrahedron( void );

      //Getters
      NEUROLOTS_API
      NodePtr Node0( void );

      NEUROLOTS_API
      NodePtr Node1( void );

      NEUROLOTS_API
      NodePtr Node2( void );

      NEUROLOTS_API
      NodePtr Node3( void );

      NEUROLOTS_API
      Eigen::MatrixXf B0( void );

      NEUROLOTS_API
      Eigen::MatrixXf B1( void );

      NEUROLOTS_API
      Eigen::MatrixXf B2( void );

      NEUROLOTS_API
      Eigen::MatrixXf B3( void );

      NEUROLOTS_API
      float Volume( void );

      NEUROLOTS_API
      bool Face0( void );

      NEUROLOTS_API
      bool Face1( void );

      NEUROLOTS_API
      bool Face2( void );

      NEUROLOTS_API
      bool Face3( void );

      // Setters
      NEUROLOTS_API
      void B0( Eigen::MatrixXf b0_ );

      NEUROLOTS_API
      void B1( Eigen::MatrixXf b1_ );

      NEUROLOTS_API
      void B2( Eigen::MatrixXf b2_ );

      NEUROLOTS_API
      void B3( Eigen::MatrixXf b3_ );

      NEUROLOTS_API
      void Volume( float volume_ );

    private:

      NodePtr _node0;
      NodePtr _node1;
      NodePtr _node2;
      NodePtr _node3;

      Eigen::MatrixXf _b0;
      Eigen::MatrixXf _b1;
      Eigen::MatrixXf _b2;
      Eigen::MatrixXf _b3;

      float _volume;

  };

  typedef Tetrahedron* TetrahedronPtr;

  class Tetrahedra
  {
    public:

      NEUROLOTS_API
      Tetrahedra( void );

      NEUROLOTS_API
      ~Tetrahedra( void );

      NEUROLOTS_API
      static void FacesToVectors( std::vector< TetrahedronPtr >& tetrahedra,
                                  std::vector< NodePtr >& nodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

      NEUROLOTS_API
      static void ContornFacesToVectors(
                                  std::vector< TetrahedronPtr >& tetrahedra,
                                  std::vector< NodePtr >& nodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

  };

} // end namespace fem

#endif // __FEM_TETRAHEDRON__

//EOF
