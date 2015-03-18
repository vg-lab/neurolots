#include "Icosphere.h"


namespace neurolots
{

  Icosphere::Icosphere( Eigen::Vector3f center_, float radius_,
                        unsigned int level )
    : _center( center_ )
    , _radius( radius_ )
  {

    _nodes.clear( );
    _tetrahedra.clear( );
    _quads.clear( );

    fem::NodePtr n0 = new fem::Node( _center + Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), 0 );
    n0->Center( _center );
    _nodes.push_back( n0 );

    fem::NodePtr n1 = new fem::Node( _center + Eigen::Vector3f( 0.0f, _radius, 0.0f ), 1,
                                     true );
    n1->Center( _center );
    _nodes.push_back( n1 );
    fem::NodePtr n2 = new fem::Node( _center + Eigen::Vector3f( 0.0f, -_radius, 0.0f ), 2,
                                     true );
    n2->Center( _center );
    _nodes.push_back( n2 );

    fem::NodePtr n3 = new fem::Node( _center + Eigen::Vector3f( _radius, 0.0f, 0.0f ), 3,
                                     true );
    n3->Center( _center );
    _nodes.push_back( n3 );
    fem::NodePtr n4 = new fem::Node( _center + Eigen::Vector3f( -_radius, 0.0f, 0.0f ), 4,
                                     true );
    n4->Center( _center );
    _nodes.push_back( n4 );

    fem::NodePtr n5 = new fem::Node( _center + Eigen::Vector3f( 0.0f, 0.0f, _radius ), 5,
                                     true );
    n5->Center( _center );
    _nodes.push_back( n5 );
    fem::NodePtr n6 = new fem::Node( _center + Eigen::Vector3f( 0.0f, 0.0f, -_radius ), 6,
                                     true );
    n6->Center( _center );
    _nodes.push_back( n6 );

    _tetrahedra.push_back( new fem::Tetrahedron( n0, n1, n3, n5 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n1, n5, n4 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n5, n3, n2 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n5, n2, n4 ));

    _tetrahedra.push_back( new fem::Tetrahedron( n0, n1, n4, n6 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n1, n6, n3 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n6, n4, n2 ));
    _tetrahedra.push_back( new fem::Tetrahedron( n0, n6, n2, n3 ));

    _quads.push_back( new Quad( n4, n5, n3, n1 ));
    _quads.push_back( new Quad( n4, n2, n3, n5 ));
    _quads.push_back( new Quad( n3, n6, n4, n1 ));
    _quads.push_back( new Quad( n3, n2, n4, n6 ));

    _DivideSphere( level );
  }

  Icosphere::~Icosphere( void )
  {

  }

  void Icosphere::CalculateSoma( std::vector< VectorizedNodePtr >& firstNodes )
  {
    for ( unsigned int i = 0; i < firstNodes.size( ); i++ )
    {
      VectorizedNodePtr vNode = firstNodes[i];
      vNode->Tangent(( vNode->Position( ) - _center).normalized( ));

      QuadPtr quad = Quads::NearerQuad( _quads, vNode->Position( ));

      Eigen::Vector3f desp = vNode->Position( ) - quad->Center( );

      quad->Node0( )->Pos( quad->Node0( )->Pos( )+ desp );
      quad->Node0( )->Center( vNode->Position( ));
      quad->Node0( )->Fixed(true);

      quad->Node1( )->Pos( quad->Node1( )->Pos( )+ desp );
      quad->Node1( )->Center( vNode->Position( ));
      quad->Node1( )->Fixed(true);

      quad->Node2( )->Pos( quad->Node2( )->Pos( )+ desp );
      quad->Node2( )->Center( vNode->Position( ));
      quad->Node2( )->Fixed(true);

      quad->Node3( )->Pos( quad->Node3( )->Pos( )+ desp );
      quad->Node3( )->Center( vNode->Position( ));
      quad->Node3( )->Fixed(true);
    }
  }

  void Icosphere::PassTrianglesToVector( std::vector< float >& vertices,
                                std::vector< float >& centers,
                                std::vector< float >& tangents,
                                std::vector< unsigned int >& mesh )
  {
    fem::Tetrahedra::FacesToVectors( _tetrahedra, _nodes, vertices, centers,
                                     tangents, mesh );
  }

  void Icosphere::PassContornTrianglesToVector( std::vector< float >& vertices,
                                       std::vector< float >& centers,
                                       std::vector< float >& tangents,
                                       std::vector< unsigned int >& mesh )
  {
    fem::Tetrahedra::ContornFacesToVectors( _tetrahedra, _nodes, vertices,
                                            centers, tangents, mesh );
  }

  void Icosphere::PassQuadsToVector( std::vector< float >& vertices,
                                      std::vector< float >& centers,
                                      std::vector< float >& tangents,
                                      std::vector< unsigned int >& mesh )
  {
    Quads::FacesToVectors( _quads, _nodes, vertices, centers, tangents, mesh );
  }

  void Icosphere::_DivideSphere( unsigned int level )
  {
    for ( unsigned int l = 0; l < level; l++ )
    {
      Edges edges( _center, _radius );

      std::vector< fem::TetrahedronPtr > tets;
      std::vector< QuadPtr > quads;

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

      for ( unsigned int k=0; k<_quads.size(); k++ )
      {
        fem::NodePtr a = _quads[k]->Node0( );
        fem::NodePtr b = _quads[k]->Node1( );
        fem::NodePtr c = _quads[k]->Node2( );
        fem::NodePtr d = _quads[k]->Node3( );
        fem::NodePtr e = edges.AddEdge( _nodes, a, b );
        fem::NodePtr f = edges.AddEdge( _nodes, b, c );
        fem::NodePtr g = edges.AddEdge( _nodes, c, d );
        fem::NodePtr h = edges.AddEdge( _nodes, d, a );
        fem::NodePtr i = edges.AddEdge( _nodes, b, d );
        quads.push_back(new Quad(a,e,i,h));
        quads.push_back(new Quad(e,b,f,i));
        quads.push_back(new Quad(i,f,c,g));
        quads.push_back(new Quad(h,i,g,d));
      }

      _tetrahedra.clear( );
      _quads.clear( );
      _tetrahedra = tets;
      _quads = quads;
      edges.Clear( );
    }
  }



} // end namespace neurolots
