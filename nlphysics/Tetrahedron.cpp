/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/gmrvvis/neurolots>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include "Tetrahedron.h"

namespace nlphysics
{

  Tetrahedron::Tetrahedron( NodePtr node0_, NodePtr node1_, NodePtr node2_,
                            NodePtr node3_ )
    : _node0( node0_ )
    , _node1( node1_ )
    , _node2( node2_ )
    , _node3( node3_ )
    , _volume( 0.0f )
  {

  }

  Tetrahedron::~Tetrahedron( void )
  {

  }

  NodePtr& Tetrahedron::node0( void )
  {
    return _node0;
  }

  NodePtr& Tetrahedron::node1( void )
  {
    return _node1;
  }

  NodePtr& Tetrahedron::node2( void )
  {
    return _node2;
  }

  NodePtr& Tetrahedron::node3( void )
  {
    return _node3;
  }

  Eigen::MatrixXf& Tetrahedron::b0( void )
  {
    return _b0;
  }

  Eigen::MatrixXf& Tetrahedron::b1( void )
  {
    return _b1;
  }

  Eigen::MatrixXf& Tetrahedron::b2( void )
  {
    return _b2;
  }

  Eigen::MatrixXf& Tetrahedron::b3( void )
  {
    return _b3;
  }

  float& Tetrahedron::volume( void )
  {
    return _volume;
  }


  bool Tetrahedron::face0( void ) const
  {
    return _node0->contour( ) && _node1->contour( ) && _node2->contour( );
  }

  bool Tetrahedron::face1( void ) const
  {
    return _node0->contour( ) && _node2->contour( ) && _node3->contour( );
  }

  bool Tetrahedron::face2( void ) const
  {
    return _node0->contour( ) && _node3->contour( ) && _node1->contour( );
  }

  bool Tetrahedron::face3( void ) const
  {
    return _node1->contour( ) && _node3->contour( ) && _node2->contour( );
  }

 } //end namespace nlphysics
