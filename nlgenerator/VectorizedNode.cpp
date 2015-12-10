/**
 * @file    VectorizedNode.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "VectorizedNode.h"

namespace nlgenerator
{

  VectorizedNode::VectorizedNode( void )
    : _primitive( nullptr )
    , _father( nullptr )
    , _bifurcation( false )
    , _firstNode( true )
  {
    _childs.clear();
  }

  VectorizedNode::~VectorizedNode( void )
  {

  }

  void VectorizedNode::AddChild( VectorizedNode* child )
  {
    if (_childs.size() == 1 )
    {
      _childs.push_back( child );
      _bifurcation = true;
    }
    else
    {
      _childs.push_back( child );
    }
  }


  //Setters
  void VectorizedNode::Id( int id_)
  {
    _id = id_;
  }

  void VectorizedNode::Position( Eigen::Vector3f position_ )
  {
    _position = position_;
  }

  void VectorizedNode::Tangent( Eigen::Vector3f tangent_ )
  {
    _tangent = tangent_;
  }

  void VectorizedNode::Radius( float radius_ )
  {
    _radius = radius_;
  }

  void VectorizedNode::Primitive( GeometricPrimitivePtr primitive_ )
  {
    _primitive = primitive_;
  }

  void VectorizedNode::Father( VectorizedNode* father_)
  {
    _father = father_;
    _firstNode = false;
  }

  //Getters
  int VectorizedNode::Id( void )
  {
    return _id;
  }

  Eigen::Vector3f VectorizedNode::Position( void )
  {
    return _position;
  }

  Eigen::Vector3f VectorizedNode::Tangent( void )
  {
    return _tangent;
  }

  float VectorizedNode::Radius( void )
  {
    return _radius;
  }

  GeometricPrimitivePtr VectorizedNode::Primitive( void )
  {
    return _primitive;
  }

  VectorizedNode* VectorizedNode::Father( void )
  {
    return _father;
  }

  std::vector < VectorizedNode* > VectorizedNode::Childs( void )
  {
    return _childs;
  }

  bool VectorizedNode::Bifurcation( void )
  {
    return _bifurcation;
  }

  bool VectorizedNode::FirstNode( void )
  {
    return _firstNode;
  }

}// end namespace nlgenerator

//EOF
