#include "Quad.h"

namespace neurolots
{
  Quad::Quad( fem::NodePtr node0_, fem::NodePtr node1_, fem::NodePtr node2_,
              fem::NodePtr node3_ )
    : _node0( node0_ )
    , _node1( node1_ )
    , _node2( node2_ )
    , _node3( node3_ )
    , _free( true )
  {
    _center = ( node0_->Pos( ) + node1_->Pos( ) + node2_->Pos( )
              + node3_->Pos( )) / 4.0f;
  }

  Quad::~Quad( void )
  {

  }

  fem::NodePtr Quad::Node0( void )
  {
    return _node0;
  }

  fem::NodePtr Quad::Node1( void )
  {
    return _node1;
  }

  fem::NodePtr Quad::Node2( void )
  {
    return _node2;
  }

  fem::NodePtr Quad::Node3( void )
  {
    return _node3;
  }

  Eigen::Vector3f Quad::Center( void )
  {
    return _center;
  }

  bool Quad::Free( void )
  {
    return _free;
  }

  //Setter
  void Quad::Free( bool free_ )
  {
    _free = free_;
  }

  Quads::Quads( void )
  {

  }

  Quads::~Quads( void )
  {

  }

  void Quads::FacesToVectors( std::vector< QuadPtr >& quads,
                              std::vector< fem::NodePtr >& nodes,
                              std::vector< float >& vertices,
                              std::vector< float >& centers,
                              std::vector< float >& tangents,
                              std::vector< unsigned int >& mesh )
  {
    fem::Nodes::ContornNodesToVectors( nodes, vertices, centers, tangents );

    for ( unsigned int i=0; i < quads.size(); i++ )
    {
      QuadPtr quad = quads[i];

      mesh.push_back( quad->Node0( )->Id2( ) );
      mesh.push_back( quad->Node1( )->Id2( ) );
      mesh.push_back( quad->Node3( )->Id2( ) );
      mesh.push_back( quad->Node2( )->Id2( ) );

    }
  }

  QuadPtr Quads::NearerQuad( std::vector< QuadPtr >& quads,
                             Eigen::Vector3f position )
  {
    QuadPtr nearerQuad = nullptr;
    QuadPtr quad;

    float minDist =  FLT_MAX;
    float dist;

    for ( unsigned int i = 0; i < quads.size( ); i++ )
    {
      quad = quads[i];
      if ( quad->Free( ))
      {
        dist = ( quad->Center( ) - position ).norm( );
        if ( dist < minDist)
        {
          nearerQuad = quad;
          minDist = dist;
        }
      }
    }
    if( nearerQuad != nullptr )
    {
      nearerQuad->Free( false );
    }

    return nearerQuad;
  }

}// end namespace neurolots
