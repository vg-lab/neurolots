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
#include "Node.h"

namespace nlphysics
{

  Node::Node( Eigen::Vector3f initialPosition_, unsigned int id_,
              bool contour_ )
    : _id( id_ )
    , _initialPosition( initialPosition_ )
    , _displacement( Eigen::Vector3f::Zero( ))
    , _position( initialPosition_ )
    , _center( Eigen::Vector3f::Zero( ))
    , _tangent( Eigen::Vector3f::Zero( ))
    , _force( Eigen::Vector3f::Zero( ))
    , _fixed( false )
    , _contour( contour_)
  {

  }

  Node::~Node( void )
  {

  }

  unsigned int& Node::id( void )
  {
    return _id;
  }

  const unsigned int& Node::id( void ) const
  {
    return _id;
  }

  Eigen::Vector3f& Node::initialPosition( void )
  {
    return _initialPosition;
  }

  const Eigen::Vector3f& Node::initialPosition( void ) const
  {
    return _initialPosition;
  }

  Eigen::Vector3f& Node::displacement( void )
  {
    return _displacement;
  }

  const Eigen::Vector3f& Node::displacement( void ) const
  {
    return _displacement;
  }

  Eigen::Vector3f& Node::position( void )
  {
    return _position;
  }

  const Eigen::Vector3f& Node::position( void ) const
  {
    return _position;
  }

  Eigen::Vector3f& Node::center( void )
  {
    return _center;
  }

  const Eigen::Vector3f& Node::center( void ) const
  {
    return _center;
  }

  Eigen::Vector3f& Node::tangent( void )
  {
    return _tangent;
  }

  const Eigen::Vector3f& Node::tangent( void ) const
  {
    return _tangent;
  }

  Eigen::Vector3f& Node::force( void )
  {
    return _force;
  }

  const Eigen::Vector3f& Node::force( void ) const
  {
    return _force;
  }

  bool& Node::fixed( void )
  {
    return _fixed;
  }

  const bool& Node::fixed( void ) const
  {
    return _fixed;
  }

  bool& Node::contour( void )
  {
    return _contour;
  }

  const bool& Node::contour( void ) const
  {
    return _contour;
  }

} // namespace nlphysics
