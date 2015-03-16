#include "Edges.h"

namespace neurolots
{

  Edge::Edge( unsigned int node_,
              fem::NodePtr middleNode_)
    : _node( node_ )
    , _middleNode( middleNode_ )
  {

  }

  Edge::~Edge( void )
  {

  }

  unsigned int Edge::Node( void )
  {
    return _node;
  }

  fem::NodePtr Edge::MiddleNode( void )
  {
    return _middleNode;
  }




  Edges::Edges( Eigen::Vector3f center_, float radius_ )
    : _center( center_ )
    , _radius( radius_ )
  {
    _edges.clear( );
  }

  Edges::~Edges( void )
  {
    _edges.clear( );
  }

  fem::NodePtr Edges::AddEdge( std::vector< fem::NodePtr >& nodes,
                               fem::NodePtr node0, fem::NodePtr node1 )
  {
    int idMin;
    int idMax;

    if( node0->Id() > node1->Id( ))
    {
      idMin = node1->Id( );
      idMax = node0->Id( );
    }
    else
    {
      idMin = node0->Id( );
      idMax = node1->Id( );
    }

    if( idMax + 1 > _edges.size( ))
    {
      _edges.resize( idMax + 1 );
    }


    //Conprube if the edge exist
    for(int i=0; i < _edges[idMin].size();  i++)
    {
      int id = _edges[idMin][i].Node();
      if(id == idMax )
      {
        return _edges[idMin][i].MiddleNode( );
      }
    }

    fem::NodePtr node = _MiddleNode( nodes, node0, node1 );
    _edges[idMin].push_back( Edge( idMax, node ));

    return node;
  }

  void Edges::Clear( void )
  {
    _edges.clear( );
  }

  fem::NodePtr Edges::_MiddleNode( std::vector< fem::NodePtr >& nodes,
                                   fem::NodePtr node0, fem::NodePtr node1)
  {
    Eigen::Vector3f pos = ( node0->Pos( ) + node1->Pos( )) / 2.0f;
    int id = nodes.size();
    bool contorn;

    if(( contorn = (node0->Contorn( ) && node1->Contorn( ))))
    {
      pos.normalize( );
      pos = _center + _radius * pos;
    }

    fem::NodePtr node = new fem::Node( pos, id, contorn );
    nodes.push_back( node );

    return node;
  }

}// end namespace neurolots
