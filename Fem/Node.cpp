#include "Node.h"

namespace fem
{

  /////////////////////////////
  // Class Node
  /////////////////////////////

  Node::Node( Eigen::Vector3f pos0_, unsigned int id_, bool contorn_ )
    : _id( id_ )
    , _pos0( pos0_ )
    , _u( Eigen::Vector3f::Zero( ))
    , _pos( pos0_ )
    , _force( Eigen::Vector3f::Zero( ))
    , _center( Eigen::Vector3f::Zero( ))
    , _tangent( Eigen::Vector3f::Zero( ))
    , _fixed( false )
    , _contorn( contorn_)
  {

  }

  Node::~Node( void )
  {

  }

  //Getters
  unsigned int Node::Id( void )
  {
    return _id;
  }

  unsigned int Node::Id2( void )
  {
    return _id2;
  }

  Eigen::Vector3f Node::Pos0( void )
  {
    return _pos0;
  }

  Eigen::Vector3f Node::U( void )
  {
    return _u;
  }

  Eigen::Vector3f Node::Pos( void )
  {
    return _pos;
  }

  Eigen::Vector3f Node::Force( void )
  {
    return _force;
  }

  Eigen::Vector3f Node::Center( void )
  {
    return _center;
  }

  Eigen::Vector3f Node::Tangent( void )
  {
    return _tangent;
  }

  bool Node::Fixed( void )
  {
    return _fixed;
  }

  bool Node::Contorn( void )
  {
    return _contorn;
  }

  //Setters
  void Node::Id( unsigned int id_ )
  {
    _id = id_;
  }

  void Node::Id2( unsigned int id2_ )
  {
    _id2 = id2_;
  }

  void Node::Pos0( Eigen::Vector3f pos0_ )
  {
    _pos0 = pos0_;
  }

  void Node::U( Eigen::Vector3f u_ )
  {
    _u = u_;
  }

  void Node::Pos( Eigen::Vector3f pos_ )
  {
    _pos = pos_;
  }

  void Node::Force( Eigen::Vector3f force_ )
  {
    _force = force_;
  }

  void Node::Center( Eigen::Vector3f center_ )
  {
    _center = center_;
  }

  void Node::Tangent( Eigen::Vector3f tangent_ )
  {
    _tangent = tangent_;
  }

  void Node::Fixed( bool fixed_ )
  {
    _fixed = fixed_;
  }

  void Node::Contorn( bool contorn_ )
  {
    _contorn = contorn_;
  }



  /////////////////////////////
  // Class Nodes
  /////////////////////////////

  Nodes::Nodes( void )
  {

  }

  Nodes::~Nodes( void )
  {

  }

  void Nodes::NodesToVectors( std::vector< NodePtr >& nodes,
                             std::vector< float >& vertices,
                             std::vector< float >& centers,
                             std::vector< float >& tangents )
  {

    for ( int i = 0; i < nodes.size( ); i++ )
    {
      NodePtr node = nodes[i];

      vertices.push_back( node->Pos( ).x( ));
      vertices.push_back( node->Pos( ).y( ));
      vertices.push_back( node->Pos( ).z( ));

      centers.push_back( node->Center( ).x());
      centers.push_back( node->Center( ).y());
      centers.push_back( node->Center( ).z());

      tangents.push_back( node->Tangent( ).x());
      tangents.push_back( node->Tangent( ).y());
      tangents.push_back( node->Tangent( ).z());
    }
  }

  void Nodes::ContornNodesToVectors( std::vector< NodePtr >& nodes,
                                     std::vector< float >& vertices,
                                     std::vector< float >& centers,
                                     std::vector< float >& tangents  )
    {

      int contornNodes = 0;
      for ( int i = 0; i < nodes.size( ); i++ )
      {
        NodePtr node = nodes[i];

        if( node->Contorn( ))
        {
          node->Id2( contornNodes );
          contornNodes ++;

          vertices.push_back( node->Pos( ).x( ));
          vertices.push_back( node->Pos( ).y( ));
          vertices.push_back( node->Pos( ).z( ));

          centers.push_back( node->Center( ).x());
          centers.push_back( node->Center( ).y());
          centers.push_back( node->Center( ).z());

          tangents.push_back( node->Tangent( ).x());
          tangents.push_back( node->Tangent( ).y());
          tangents.push_back( node->Tangent( ).z());
        }
      }
    }

}// end namespace fem
