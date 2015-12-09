/**
 * @file    Tetrahedron.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "Tetrahedron.h"

namespace nlfem
{

  Tetrahedron::Tetrahedron( NodePtr node0_, NodePtr node1_, NodePtr node2_,
                            NodePtr node3_ )
    : _node0( node0_ )
    , _node1( node1_ )
    , _node2( node2_ )
    , _node3( node3_ )
  {

  }

  Tetrahedron::~Tetrahedron( void )
  {

  }

  //Getters

  NodePtr Tetrahedron::Node0( void )
  {
    return _node0;
  }

  NodePtr Tetrahedron::Node1( void )
  {
    return _node1;
  }

  NodePtr Tetrahedron::Node2( void )
  {
    return _node2;
  }

  NodePtr Tetrahedron::Node3( void )
  {
    return _node3;
  }

  Eigen::MatrixXf Tetrahedron::B0( void )
  {
    return _b0;
  }

  Eigen::MatrixXf Tetrahedron::B1( void )
  {
    return _b1;
  }

  Eigen::MatrixXf Tetrahedron::B2( void )
  {
    return _b2;
  }

  Eigen::MatrixXf Tetrahedron::B3( void )
  {
    return _b3;
  }

  float Tetrahedron::Volume( void )
  {
    return _volume;
  }

  bool Tetrahedron::Face0( void )
  {
    return _node0->Contorn( ) && _node1->Contorn( ) && _node2->Contorn( );
  }

  bool Tetrahedron::Face1( void )
  {
    return _node0->Contorn( ) && _node2->Contorn( ) && _node3->Contorn( );
  }

  bool Tetrahedron::Face2( void )
  {
    return _node0->Contorn( ) && _node3->Contorn( ) && _node1->Contorn( );
  }

  bool Tetrahedron::Face3( void )
  {
    return _node1->Contorn( ) && _node3->Contorn( ) && _node2->Contorn( );
  }

  //Setters

  void Tetrahedron::B0( Eigen::MatrixXf b0_ )
  {
    _b0 = b0_;
  }

  void Tetrahedron::B1( Eigen::MatrixXf b1_ )
  {
    _b1 = b1_;
  }

  void Tetrahedron::B2( Eigen::MatrixXf b2_ )
  {
    _b2 = b2_;
  }

  void Tetrahedron::B3( Eigen::MatrixXf b3_ )
  {
    _b3 = b3_;
  }

  void Tetrahedron::Volume( float volume_ )
  {
    _volume = volume_;
  }

  Tetrahedra::Tetrahedra( void )
  {

  }

  Tetrahedra::~Tetrahedra( void )
  {

  }

  void Tetrahedra::FacesToVectors( std::vector< TetrahedronPtr >& tetrahedra,
                                   std::vector< NodePtr >& nodes,
                                   std::vector< float >& vertices,
                                   std::vector< float >& centers,
                                   std::vector< float >& tangents,
                                   std::vector< unsigned int >& mesh )
  {
    Nodes::NodesToVectors( nodes, vertices, centers, tangents);

    mesh.clear( );

    for ( unsigned int i = 0; i < tetrahedra.size( ); i++ )
    {
      TetrahedronPtr tet = tetrahedra[i];
      int n0 = tet->Node0( )->Id( );
      int n1 = tet->Node1( )->Id( );
      int n2 = tet->Node2( )->Id( );
      int n3 = tet->Node3( )->Id( );

      //face 0
      mesh.push_back( n0 ); mesh.push_back( n1 ); mesh.push_back( n2 );
      //face 1
      mesh.push_back( n0 ); mesh.push_back( n2 ); mesh.push_back( n3 );
      //face 2
      mesh.push_back( n0 ); mesh.push_back( n3 ); mesh.push_back( n1 );
      //face 3
      mesh.push_back( n1 ); mesh.push_back( n3 ); mesh.push_back( n2 );
    }
  }

  void Tetrahedra::ContornFacesToVectors(
                                   std::vector< TetrahedronPtr >& tetrahedra,
                                   std::vector< NodePtr >& nodes,
                                   std::vector< float >& vertices,
                                   std::vector< float >& centers,
                                   std::vector< float >& tangents,
                                   std::vector< unsigned int >& mesh )
  {
    Nodes::ContornNodesToVectors( nodes, vertices, centers, tangents );

    for ( unsigned int i = 0; i < tetrahedra.size( ); i++ )
    {
      TetrahedronPtr tet = tetrahedra[i];
      int n0 = tet->Node0( )->Id2( );
      int n1 = tet->Node1( )->Id2( );
      int n2 = tet->Node2( )->Id2( );
      int n3 = tet->Node3( )->Id2( );

      bool fixed0 = tet->Node0( )->Fixed( );
      bool fixed1 = tet->Node1( )->Fixed( );
      bool fixed2 = tet->Node2( )->Fixed( );
      bool fixed3 = tet->Node3( )->Fixed( );

      if( tet->Face0( )  && !( fixed0 & fixed1 & fixed2 ))
      {
        mesh.push_back( n0 ); mesh.push_back( n1 ); mesh.push_back( n2 );
      }
      if( tet->Face1( )  && !( fixed0 && fixed2 && fixed3 ))
      {
        mesh.push_back( n0 ); mesh.push_back( n2 ); mesh.push_back( n3 );
      }
      if( tet->Face2( )  && !( fixed0 && fixed3 && fixed1 ))
      {
        mesh.push_back( n0 ); mesh.push_back( n3 ); mesh.push_back( n1 );
      }
      if( tet->Face3( )  && !( fixed1 && fixed3 && fixed2 ))
      {
        mesh.push_back( n1 ); mesh.push_back( n3 ); mesh.push_back( n2 );
      }
    }
  }

} //end namespace nlfem
