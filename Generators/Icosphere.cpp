#include "Icosphere.h"


namespace neurolots
{

  Icosphere::Icosphere( Eigen::Vector3f center_, float radius_, int level )
    : _center( center_ )
    , _radius( radius_ )
  {
    _nodes.clear( );
    _tetrahedra.clear( );

    fem::NodePtr n0 = new fem::Node( _center + Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), 0 );
    _nodes.push_back( n0 );

    fem::NodePtr n1 = new fem::Node( _center + Eigen::Vector3f( 0.0f, _radius, 0.0f ), 1,
                                     true );
    _nodes.push_back( n1 );
    fem::NodePtr n2 = new fem::Node( _center + Eigen::Vector3f( 0.0f, -_radius, 0.0f ), 2,
                                     true );
    _nodes.push_back( n2 );

    fem::NodePtr n3 = new fem::Node( _center + Eigen::Vector3f( _radius, 0.0f, 0.0f ), 3,
                                     true );
    _nodes.push_back( n3 );
    fem::NodePtr n4 = new fem::Node( _center + Eigen::Vector3f( -_radius, 0.0f, 0.0f ), 4,
                                     true );
    _nodes.push_back( n4 );

    fem::NodePtr n5 = new fem::Node( _center + Eigen::Vector3f( 0.0f, 0.0f, _radius ), 5,
                                     true );
    _nodes.push_back( n5 );
    fem::NodePtr n6 = new fem::Node( _center + Eigen::Vector3f( 0.0f, 0.0f, -_radius ), 6,
                                     true );
    _nodes.push_back( n6 );

    _tetrahedra.push_back( new fem::Tetrahedron( n0, n1, n3, n5 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n1, n5, n4 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n5, n3, n2 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n5, n2, n4 ));

    _tetrahedra.push_back( new fem::Tetrahedron( n0, n1, n4, n6 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n1, n6, n3 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n6, n4, n2 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n6, n2, n3 ));

    _DivideSphere( level );
  }

  Icosphere::~Icosphere( void )
  {

  }

  void Icosphere::PassToVector( std::vector< float >& vertices,
                                std::vector< float >& centers,
                                std::vector< unsigned int >& mesh )
  {
    fem::Tetrahedra::FacesToVectors( _tetrahedra, _nodes, vertices, centers,
                                     mesh );
  }

  void Icosphere::PassContornToVector( std::vector< float >& vertices,
                                       std::vector< float >& centers,
                                       std::vector< unsigned int >& mesh )
  {
    fem::Tetrahedra::ContornFacesToVectors( _tetrahedra, _nodes, vertices, centers,
                                     mesh );
  }

  void Icosphere::_DivideSphere( int level )
  {
    for ( unsigned int l = 0; l < level; l++ )
    {
      Edges edges( _center, _radius );

      std::vector< fem::TetrahedronPtr > tets;

      unsigned int size = _tetrahedra.size( );

      for ( unsigned int k = 0; k < size; k++ )
      {
        fem::NodePtr a = _tetrahedra[k]->Node0( );
        fem::NodePtr b = _tetrahedra[k]->Node1( );
        fem::NodePtr c = _tetrahedra[k]->Node2( );
        fem::NodePtr d = _tetrahedra[k]->Node3( );

        fem::NodePtr e =  edges.AddEdge( _nodes, a, b );
        fem::NodePtr f =  edges.AddEdge( _nodes, a, c );
        fem::NodePtr g =  edges.AddEdge( _nodes, a, d );
        fem::NodePtr h =  edges.AddEdge( _nodes, b, c );
        fem::NodePtr i =  edges.AddEdge( _nodes, c, d );
        fem::NodePtr j =  edges.AddEdge( _nodes, d, b );
        //4 Externos
        tets.push_back( new fem::Tetrahedron( a, e, f, g ));
        tets.push_back( new fem::Tetrahedron( b, h, e, j ));
        tets.push_back( new fem::Tetrahedron( c, i, f, h ));
        tets.push_back( new fem::Tetrahedron( d, j, g, i ));
            //4 Internos
        tets.push_back( new fem::Tetrahedron( e, f, j, h ));
        tets.push_back( new fem::Tetrahedron( i, f, h, j ));
        tets.push_back( new fem::Tetrahedron( e, f, g, j ));
        tets.push_back( new fem::Tetrahedron( i, f, j, g ));
      }
      _tetrahedra.clear( );
      _tetrahedra = tets;
      edges.Clear( );
    }
  }



} // end namespace neurolots
