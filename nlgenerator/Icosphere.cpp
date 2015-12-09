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

    nlfem::NodePtr n0 = new nlfem::Node( _center + Eigen::Vector3f( 0.0f, 0.0f,
                                     0.0f ), 0 );
    n0->Center( _center );
    _nodes.push_back( n0 );

    nlfem::NodePtr n1 = new nlfem::Node(
      _center + Eigen::Vector3f( 0.0f, _radius, 0.0f ), 1, true );
    n1->Center( _center );
    _nodes.push_back( n1 );
    nlfem::NodePtr n2 = new nlfem::Node(
      _center + Eigen::Vector3f( 0.0f, -_radius, 0.0f ), 2, true );
    n2->Center( _center );
    _nodes.push_back( n2 );

    nlfem::NodePtr n3 = new nlfem::Node(
      _center + Eigen::Vector3f( _radius, 0.0f, 0.0f ), 3, true );
    n3->Center( _center );
    _nodes.push_back( n3 );
    nlfem::NodePtr n4 = new nlfem::Node(
      _center + Eigen::Vector3f( -_radius, 0.0f, 0.0f ), 4, true );
    n4->Center( _center );
    _nodes.push_back( n4 );

    nlfem::NodePtr n5 = new nlfem::Node(
      _center + Eigen::Vector3f( 0.0f, 0.0f, _radius ), 5, true );
    n5->Center( _center );
    _nodes.push_back( n5 );
    nlfem::NodePtr n6 = new nlfem::Node(
      _center + Eigen::Vector3f( 0.0f, 0.0f, -_radius ), 6, true );
    n6->Center( _center );
    _nodes.push_back( n6 );

    _tetrahedra.push_back( new nlfem::Tetrahedron( n0, n1, n3, n5 ));
    _tetrahedra.push_back( new nlfem::Tetrahedron( n0, n1, n5, n4 ));
    _tetrahedra.push_back( new nlfem::Tetrahedron( n0, n5, n3, n2 ));
    _tetrahedra.push_back( new nlfem::Tetrahedron( n0, n5, n2, n4 ));

    _tetrahedra.push_back( new nlfem::Tetrahedron( n0, n1, n4, n6 ));
    _tetrahedra.push_back( new nlfem::Tetrahedron( n0, n1, n6, n3 ));
    _tetrahedra.push_back( new nlfem::Tetrahedron( n0, n6, n4, n2 ));
    _tetrahedra.push_back( new nlfem::Tetrahedron( n0, n6, n2, n3 ));

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
    Eigen::Vector3f exe( 0.0f, 0.0f, 1.0f );

    Eigen::Vector3f va( 0.0f, 1.0f, 0.0f );
    Eigen::Vector3f vb( -1.0f, 0.0f, 0.0f );
    Eigen::Vector3f vc( 0.0f, -1.0f, 0.0f );
    Eigen::Vector3f vd( 1.0f, 0.0f, 0.0f );

    Eigen::Vector3f center;
    Eigen::Vector3f tangent;
    std::vector< Eigen::Vector3f > position;
    position.resize( 4 );
    Eigen::Vector3f positionVb;
    Eigen::Vector3f positionVc;
    Eigen::Vector3f positionVd;

    Eigen::Quaternion< float > q;

    nlfem::Nodes::ContornIds( _nodes );

    for ( unsigned int i = 0; i < firstNodes.size( ); i++ )
    {
      VectorizedNodePtr vNode = firstNodes[i];

      QuadPtr quad = Quads::NearerQuad( _quads, vNode->Position( ));

      center = vNode->Position( );
      tangent = ( vNode->Position( ) - _center).normalized( );

      quad->Node0( )->Center( center);
      quad->Node0( )->Tangent( tangent );
      quad->Node0( )->Fixed(true);
      quad->Node1( )->Center( center);
      quad->Node1( )->Tangent( tangent );
      quad->Node1( )->Fixed(true);
      quad->Node2( )->Center( center);
      quad->Node2( )->Tangent( tangent );
      quad->Node2( )->Fixed(true);
      quad->Node3( )->Center( center);
      quad->Node3( )->Tangent( tangent );
      quad->Node3( )->Fixed(true);


      q.setFromTwoVectors(exe,tangent);
      position[0] = q._transformVector( va ) * vNode->Radius( ) + center;
      position[1] = q._transformVector( vb ) * vNode->Radius( ) + center;
      position[2] = q._transformVector( vc ) * vNode->Radius( ) + center;
      position[3] = q._transformVector( vd ) * vNode->Radius( ) + center;


      unsigned int offset = 0;

      float minDist = FLT_MAX;
      float dist;
      Eigen::Vector3f pos = quad->Node0( )->Pos( );

      for ( unsigned int j = 0; j < 4; j++ )
      {
        dist =  ( pos - position[j]).norm( );
        if ( dist < minDist )
        {
          minDist = dist;
          offset = j;
        }
      }

      std::vector< nlfem::NodePtr > nodes(4);

      quad->Node0( )->Pos( position[offset] );
      nodes[offset] = quad->Node0( );
      offset  ++;
      offset %= 4;
      quad->Node1( )->Pos( position[offset] );
      nodes[offset] = quad->Node1( );
      offset  ++;
      offset %= 4;
      quad->Node2( )->Pos( position[offset] );
      nodes[offset] = quad->Node2( );
      offset  ++;
      offset %= 4;
      quad->Node3( )->Pos( position[offset] );
      nodes[offset] = quad->Node3( );


      vNode->Primitive( new GeometricPrimitive( nodes[0]->Id2( ),
                                                nodes[1]->Id2( ),
                                                nodes[2]->Id2( ),
                                                nodes[3]->Id2( )));
      vNode->Tangent( tangent );
    }

    nlfem::Fem fem( _nodes, _tetrahedra );
    fem.Solve( );

    _CalculateCenters( );
  }

  void Icosphere::TrianglesToVector( std::vector< float >& vertices,
                                     std::vector< float >& centers,
                                     std::vector< float >& tangents,
                                     std::vector< unsigned int >& mesh )
  {
    nlfem::Tetrahedra::FacesToVectors( _tetrahedra, _nodes, vertices, centers,
                                       tangents, mesh );
  }

  void Icosphere::TrianglesToVector(
    std::vector< VectorizedNodePtr >& /* firstNodes */,
    std::vector< float >& vertices,
    std::vector< float >& centers,
    std::vector< float >& tangents,
    std::vector< unsigned int >& mesh )
  {
    Quads::FacesToVectors( _quads, _nodes, vertices, centers, tangents, mesh );
  }

  void Icosphere::QuadsToVector( std::vector< float >& vertices,
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

      std::vector< nlfem::TetrahedronPtr > tets;
      std::vector< QuadPtr > quads;

      unsigned int size = ( unsigned int ) _tetrahedra.size( );

      for ( unsigned int k = 0; k < size; k++ )
      {
        nlfem::NodePtr a = _tetrahedra[k]->Node0( );
        nlfem::NodePtr b = _tetrahedra[k]->Node1( );
        nlfem::NodePtr c = _tetrahedra[k]->Node2( );
        nlfem::NodePtr d = _tetrahedra[k]->Node3( );

        nlfem::NodePtr e =  edges.AddEdge( _nodes, a, b );
        nlfem::NodePtr f =  edges.AddEdge( _nodes, a, c );
        nlfem::NodePtr g =  edges.AddEdge( _nodes, a, d );
        nlfem::NodePtr h =  edges.AddEdge( _nodes, b, c );
        nlfem::NodePtr i =  edges.AddEdge( _nodes, c, d );
        nlfem::NodePtr j =  edges.AddEdge( _nodes, d, b );
        //4 Externos
        tets.push_back( new nlfem::Tetrahedron( a, e, f, g ));
        tets.push_back( new nlfem::Tetrahedron( b, h, e, j ));
        tets.push_back( new nlfem::Tetrahedron( c, i, f, h ));
        tets.push_back( new nlfem::Tetrahedron( d, j, g, i ));
            //4 Internos
        tets.push_back( new nlfem::Tetrahedron( e, f, j, h ));
        tets.push_back( new nlfem::Tetrahedron( i, f, h, j ));
        tets.push_back( new nlfem::Tetrahedron( e, f, g, j ));
        tets.push_back( new nlfem::Tetrahedron( i, f, j, g ));
      }

      for ( unsigned int k=0; k<_quads.size(); k++ )
      {
        nlfem::NodePtr a = _quads[k]->Node0( );
        nlfem::NodePtr b = _quads[k]->Node1( );
        nlfem::NodePtr c = _quads[k]->Node2( );
        nlfem::NodePtr d = _quads[k]->Node3( );
        nlfem::NodePtr e = edges.AddEdge( _nodes, a, b );
        nlfem::NodePtr f = edges.AddEdge( _nodes, b, c );
        nlfem::NodePtr g = edges.AddEdge( _nodes, c, d );
        nlfem::NodePtr h = edges.AddEdge( _nodes, d, a );
        nlfem::NodePtr i = edges.AddEdge( _nodes, b, d );
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

  void Icosphere::_CalculateCenters( void )
  {
    nlfem::NodePtr node;
    for ( unsigned int i = 0; i < _nodes.size( ); i++ )
    {
      node = _nodes[i];
      if ( node->Contorn( ) && !node->Fixed() ){
        node->Center( Eigen::Vector3f(0.0f, 0.0f, 0.0f));
      }
    }

    Eigen::Vector3f v0;
    Eigen::Vector3f v1;
    Eigen::Vector3f v2;
    Eigen::Vector3f v3;
    Eigen::Vector3f exe0;
    Eigen::Vector3f exe1;
    Eigen::Vector3f t;

    for ( unsigned int i = 0; i < _tetrahedra.size( ); i++ )
    {
      nlfem::TetrahedronPtr tet = _tetrahedra[i];

      bool fixed0 = tet->Node0( )->Fixed( );
      bool fixed1 = tet->Node1( )->Fixed( );
      bool fixed2 = tet->Node2( )->Fixed( );
      bool fixed3 = tet->Node3( )->Fixed( );

      v0 = tet->Node0( )->Pos( );
      v1 = tet->Node1( )->Pos( );
      v2 = tet->Node2( )->Pos( );
      v3 = tet->Node3( )->Pos( );

      if( tet->Face0( )  && !( fixed0 & fixed1 & fixed2 ))
      {
        exe0 = ( v1 - v0 ).normalized( );
        exe1 = ( v2 - v0 ).normalized( );
        t = exe0.cross( exe1 );
        if( !tet->Node0( )->Fixed() )
        {
          tet->Node0( )->AddCenter( t );
        }
        if( !tet->Node1( )->Fixed() )
        {
          tet->Node1( )->AddCenter( t );
        }
        if( !tet->Node2( )->Fixed() )
        {
          tet->Node2( )->AddCenter( t );
        }
      }
      if( tet->Face1( )  && !( fixed0 && fixed2 && fixed3 ))
      {
        exe0 = ( v2 - v0 ).normalized( );
        exe1 = ( v3 - v0 ).normalized( );
        t = exe0.cross( exe1 );
        if( !tet->Node0( )->Fixed() )
        {
          tet->Node0( )->AddCenter( t );
        }
        if( !tet->Node2( )->Fixed() )
        {
          tet->Node2( )->AddCenter( t );
        }
        if( !tet->Node3( )->Fixed() )
        {
          tet->Node3( )->AddCenter( t );
        }
      }
      if( tet->Face2( )  && !( fixed0 && fixed3 && fixed1 ))
      {
        exe0 = ( v3 - v0 ).normalized( );
        exe1 = ( v1 - v0 ).normalized( );
        t = exe0.cross( exe1 );
        if( !tet->Node0( )->Fixed() )
        {
          tet->Node0( )->AddCenter( t );
        }
        if( !tet->Node3( )->Fixed() )
        {
          tet->Node3( )->AddCenter( t );
        }
        if( !tet->Node1( )->Fixed() )
        {
          tet->Node1( )->AddCenter( t );
        }
      }
      if( tet->Face3( )  && !( fixed1 && fixed3 && fixed2 ))
      {
        exe0 = ( v3 - v1 ).normalized( );
        exe1 = ( v2 - v1 ).normalized( );
        t = exe0.cross( exe1 );
        if( !tet->Node1( )->Fixed() )
        {
          tet->Node1( )->AddCenter( t );
        }
        if( !tet->Node2( )->Fixed() )
        {
          tet->Node2( )->AddCenter( t );
        }
        if( !tet->Node2( )->Fixed() )
        {
          tet->Node3( )->AddCenter( t );
        }
      }
    }

    for ( unsigned int i = 0; i < _nodes.size( ); i++ )
    {
      node = _nodes[i];
      if ( node->Contorn( ) && !node->Fixed( )){
        node->Center( node->Pos() - ( node->Center( )).normalized( ) );
      }
    }
  }


} // end namespace neurolots
