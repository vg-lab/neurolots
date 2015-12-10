/**
 * @file    Tetrahedron.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLFEM_TETRAHEDRON__
#define __NLFEM_TETRAHEDRON__

//Eigen
#include <Eigen/Dense>

#include "Node.h"

#include <iostream>
#include <vector>

#include <nlfem/api.h>


namespace nlfem
{


  class Tetrahedron
  {

    public:

      NLFEM_API
      Tetrahedron( NodePtr node0_, NodePtr node1_, NodePtr node2_,
                   NodePtr node3_ );

      NLFEM_API
      ~Tetrahedron( void );

      //Getters
      NLFEM_API
      NodePtr Node0( void );

      NLFEM_API
      NodePtr Node1( void );

      NLFEM_API
      NodePtr Node2( void );

      NLFEM_API
      NodePtr Node3( void );

      NLFEM_API
      Eigen::MatrixXf B0( void );

      NLFEM_API
      Eigen::MatrixXf B1( void );

      NLFEM_API
      Eigen::MatrixXf B2( void );

      NLFEM_API
      Eigen::MatrixXf B3( void );

      NLFEM_API
      float Volume( void );

      NLFEM_API
      bool Face0( void );

      NLFEM_API
      bool Face1( void );

      NLFEM_API
      bool Face2( void );

      NLFEM_API
      bool Face3( void );

      // Setters
      NLFEM_API
      void B0( Eigen::MatrixXf b0_ );

      NLFEM_API
      void B1( Eigen::MatrixXf b1_ );

      NLFEM_API
      void B2( Eigen::MatrixXf b2_ );

      NLFEM_API
      void B3( Eigen::MatrixXf b3_ );

      NLFEM_API
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

      NLFEM_API
      Tetrahedra( void );

      NLFEM_API
      ~Tetrahedra( void );

      NLFEM_API
      static void FacesToVectors( std::vector< TetrahedronPtr >& tetrahedra,
                                  std::vector< NodePtr >& nodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

      NLFEM_API
      static void ContornFacesToVectors(
                                  std::vector< TetrahedronPtr >& tetrahedra,
                                  std::vector< NodePtr >& nodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

  };

} // end namespace nlfem

#endif // __NLFEM_TETRAHEDRON__

//EOF
