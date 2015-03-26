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


namespace fem
{


  class Tetrahedron
  {

    public:

      Tetrahedron( NodePtr node0_, NodePtr node1_, NodePtr node2_,
                   NodePtr node3_ );
      ~Tetrahedron( void );

      //Getters
      NodePtr Node0( void );
      NodePtr Node1( void );
      NodePtr Node2( void );
      NodePtr Node3( void );

      Eigen::MatrixXf B0( void );
      Eigen::MatrixXf B1( void );
      Eigen::MatrixXf B2( void );
      Eigen::MatrixXf B3( void );

      float Volume( void );

      bool Face0( void );
      bool Face1( void );
      bool Face2( void );
      bool Face3( void );

      // Setters
      void B0( Eigen::MatrixXf b0_ );
      void B1( Eigen::MatrixXf b1_ );
      void B2( Eigen::MatrixXf b2_ );
      void B3( Eigen::MatrixXf b3_ );

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

      Tetrahedra( void );
      ~Tetrahedra( void );

      static void FacesToVectors( std::vector< TetrahedronPtr >& tetrahedra,
                                  std::vector< NodePtr >& nodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

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
