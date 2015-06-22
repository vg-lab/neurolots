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

#include <neurolots/api.h>


namespace fem
{


  class Node
  {

    public:

      NEUROLOTS_API
      Node( Eigen::Vector3f pos0_, unsigned int id_, bool contorn_ = false );

      NEUROLOTS_API
     ~Node( void );

      //Getters
      NEUROLOTS_API
      unsigned int Id( void );

      NEUROLOTS_API
      unsigned int Id2( void );

      NEUROLOTS_API
      Eigen::Vector3f Pos0( void );

      NEUROLOTS_API
      Eigen::Vector3f U( void );

      NEUROLOTS_API
      Eigen::Vector3f Pos( void );

      NEUROLOTS_API
      Eigen::Vector3f Force( void );

      NEUROLOTS_API
      Eigen::Vector3f Center( void );

      NEUROLOTS_API
      Eigen::Vector3f Tangent( void );

      NEUROLOTS_API
      bool Fixed( void );

      NEUROLOTS_API
      bool Contorn( void );

      //Setters
      NEUROLOTS_API
      void Id( unsigned int );

      void Id2( unsigned int );

      NEUROLOTS_API
      void Pos0( Eigen::Vector3f );

      NEUROLOTS_API
      void U( Eigen::Vector3f );

      NEUROLOTS_API
      void Pos( Eigen::Vector3f );

      NEUROLOTS_API
      void Force( Eigen::Vector3f );

      NEUROLOTS_API
      void Center( Eigen::Vector3f );

      NEUROLOTS_API
      void Tangent( Eigen::Vector3f );

      NEUROLOTS_API
      void Fixed( bool );

      NEUROLOTS_API
      void Contorn( bool );

      NEUROLOTS_API
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
      NEUROLOTS_API
      Nodes( void );

      NEUROLOTS_API
      ~Nodes( void );

      NEUROLOTS_API
      static void NodesToVectors( std::vector< NodePtr >& nodes,
                                 std::vector< float >& vertices,
                                 std::vector< float >& centers,
                                 std::vector< float >& tagents );

      NEUROLOTS_API
      static void ContornNodesToVectors( std::vector< NodePtr >& nodes,
                                         std::vector< float >& vertices,
                                         std::vector< float >& centers,
                                         std::vector< float >& tagents );

      NEUROLOTS_API
      static void ContornIds( std::vector< NodePtr >& nodes );


  };

} // end namespace fem

#endif // __FEM_NODE__

//EOF
