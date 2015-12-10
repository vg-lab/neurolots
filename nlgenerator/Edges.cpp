/**
 * @file    Edges.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "Edges.h"

namespace nlgenerator
{

  Edge::Edge( unsigned int node_,
              nlfem::NodePtr middleNode_)
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

  nlfem::NodePtr Edge::MiddleNode( void )
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

  nlfem::NodePtr Edges::AddEdge( std::vector< nlfem::NodePtr >& nodes,
                                 nlfem::NodePtr node0, nlfem::NodePtr node1 )
  {
    unsigned int idMin;
    unsigned int idMax;

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
    for ( unsigned int i=0; i < _edges[idMin].size();  i++)
    {
      unsigned int id = _edges[idMin][i].Node();
      if(id == idMax )
      {
        return _edges[idMin][i].MiddleNode( );
      }
    }

    nlfem::NodePtr node = _MiddleNode( nodes, node0, node1 );
    _edges[idMin].push_back( Edge( idMax, node ));

    return node;
  }

  void Edges::Clear( void )
  {
    _edges.clear( );
  }

  nlfem::NodePtr Edges::_MiddleNode( std::vector< nlfem::NodePtr >& nodes,
                                     nlfem::NodePtr node0, nlfem::NodePtr node1)
  {
    Eigen::Vector3f pos = ( node0->Pos( ) + node1->Pos( )) / 2.0f;
    int id = int( nodes.size( ));
    bool contorn;

    if(( contorn = (node0->Contorn( ) && node1->Contorn( ))))
    {
      pos = (pos - _center).normalized();
      pos = _center + _radius * pos;
    }

    nlfem::NodePtr node = new nlfem::Node( pos, id, contorn );
    node->Center( _center );
    nodes.push_back( node );

    return node;
  }

}// end namespace nlgenerator
