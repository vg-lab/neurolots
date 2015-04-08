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


namespace fem
{


  class Node
  {

    public:

      Node( Eigen::Vector3f pos0_, unsigned int id_, bool contorn_ = false );
      ~Node( void );

      //Getters
      unsigned int Id( void );
      unsigned int Id2( void );
      Eigen::Vector3f Pos0( void );
      Eigen::Vector3f U( void );
      Eigen::Vector3f Pos( void );
      Eigen::Vector3f Force( void );
      Eigen::Vector3f Center( void );
      Eigen::Vector3f Tangent( void );
      bool Fixed( void );
      bool Contorn( void );

      //Setters
      void Id( unsigned int );
      void Id2( unsigned int );
      void Pos0( Eigen::Vector3f );
      void U( Eigen::Vector3f );
      void Pos( Eigen::Vector3f );
      void Force( Eigen::Vector3f );
      void Center( Eigen::Vector3f );
      void Tangent( Eigen::Vector3f );
      void Fixed( bool );
      void Contorn( bool );

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
      Nodes( void );
      ~Nodes( void );

      static void NodesToVectors( std::vector< NodePtr >& nodes,
                                 std::vector< float >& vertices,
                                 std::vector< float >& centers,
                                 std::vector< float >& tagents );

      static void ContornNodesToVectors( std::vector< NodePtr >& nodes,
                                         std::vector< float >& vertices,
                                         std::vector< float >& centers,
                                         std::vector< float >& tagents );

      static void ContornIds( std::vector< NodePtr >& nodes );


  };

} // end namespace fem

#endif // __FEM_NODE__

//EOF
