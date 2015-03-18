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

      bool Face0( void );
      bool Face1( void );
      bool Face2( void );
      bool Face3( void );

    private:

      NodePtr _node0;
      NodePtr _node1;
      NodePtr _node2;
      NodePtr _node3;

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
