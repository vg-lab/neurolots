/**
 * @file    Node.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __FEM_NODE__
#define __FEM_NODE__

//Eigen
#include <Eigen/Dense>

#include <iostream>
#include <vector>

#include <neurolots/nlfem/api.h>


namespace fem
{


  class Node
  {

    public:

      NLFEM_API
      Node( Eigen::Vector3f pos0_, unsigned int id_, bool contorn_ = false );

      NLFEM_API
     ~Node( void );

      //Getters
      NLFEM_API
      unsigned int Id( void );

      NLFEM_API
      unsigned int Id2( void );

      NLFEM_API
      Eigen::Vector3f Pos0( void );

      NLFEM_API
      Eigen::Vector3f U( void );

      NLFEM_API
      Eigen::Vector3f Pos( void );

      NLFEM_API
      Eigen::Vector3f Force( void );

      NLFEM_API
      Eigen::Vector3f Center( void );

      NLFEM_API
      Eigen::Vector3f Tangent( void );

      NLFEM_API
      bool Fixed( void );

      NLFEM_API
      bool Contorn( void );

      //Setters
      NLFEM_API
      void Id( unsigned int );

      void Id2( unsigned int );

      NLFEM_API
      void Pos0( Eigen::Vector3f );

      NLFEM_API
      void U( Eigen::Vector3f );

      NLFEM_API
      void Pos( Eigen::Vector3f );

      NLFEM_API
      void Force( Eigen::Vector3f );

      NLFEM_API
      void Center( Eigen::Vector3f );

      NLFEM_API
      void Tangent( Eigen::Vector3f );

      NLFEM_API
      void Fixed( bool );

      NLFEM_API
      void Contorn( bool );

      NLFEM_API
      void AddCenter( Eigen::Vector3f add );


    private:
      unsigned int _id;
      unsigned int _id2;

      Eigen::Vector3f _pos0;
      Eigen::Vector3f _u;
      Eigen::Vector3f _pos;
      Eigen::Vector3f _force;
      Eigen::Vector3f _center;
      Eigen::Vector3f _tangent;

      bool _fixed;
      bool _contorn;

  };

  typedef Node* NodePtr;

  class Nodes
  {
    public:
      NLFEM_API
      Nodes( void );

      NLFEM_API
      ~Nodes( void );

      NLFEM_API
      static void NodesToVectors( std::vector< NodePtr >& nodes,
                                 std::vector< float >& vertices,
                                 std::vector< float >& centers,
                                 std::vector< float >& tagents );

      NLFEM_API
      static void ContornNodesToVectors( std::vector< NodePtr >& nodes,
                                         std::vector< float >& vertices,
                                         std::vector< float >& centers,
                                         std::vector< float >& tagents );

      NLFEM_API
      static void ContornIds( std::vector< NodePtr >& nodes );


  };

} // end namespace fem

#endif // __FEM_NODE__

//EOF
