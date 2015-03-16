#include "Tetrahedron.h"

namespace fem
{

  /////////////////////////////
  // Class Tetrahedron
  /////////////////////////////


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


  /////////////////////////////
  // Class Tetrahedra
  /////////////////////////////

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
                                   std::vector< unsigned int >& mesh )
  {
    Nodes::NodesToVectors( nodes, vertices, centers);

    mesh.clear( );

    for ( int i = 0; i < tetrahedra.size( ); i++ )
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
                                   std::vector< unsigned int >& mesh )
  {
    Nodes::ContornNodesToVectors( nodes, vertices, centers);

    mesh.clear( );

    for ( int i = 0; i < tetrahedra.size( ); i++ )
    {
      TetrahedronPtr tet = tetrahedra[i];
      int n0 = tet->Node0( )->Id2( );
      int n1 = tet->Node1( )->Id2( );
      int n2 = tet->Node2( )->Id2( );
      int n3 = tet->Node3( )->Id2( );

      if( tet->Face0( ))
      {
        mesh.push_back( n0 ); mesh.push_back( n1 ); mesh.push_back( n2 );
      }
      if( tet->Face1( ))
      {
        mesh.push_back( n0 ); mesh.push_back( n2 ); mesh.push_back( n3 );
      }
      if( tet->Face2( ))
      {
        mesh.push_back( n0 ); mesh.push_back( n3 ); mesh.push_back( n1 );
      }
      if( tet->Face3( ))
      {
        mesh.push_back( n1 ); mesh.push_back( n3 ); mesh.push_back( n2 );
      }
    }
  }

} //end namespace fem
