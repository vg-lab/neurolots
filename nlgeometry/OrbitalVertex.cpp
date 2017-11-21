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
#include "OrbitalVertex.h"

namespace nlgeometry
{

  OrbitalVertex::OrbitalVertex(
    const Eigen::Vector3f& position_,
    const Eigen::Vector3f& center_,
    const Eigen::Vector3f& tangent_,
    const Eigen::Vector3f& color_ )
    : Vertex( position_, Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), color_ )
    , _center( center_ )
    , _tangent( tangent_ )
  {

  }

  OrbitalVertex::OrbitalVertex( const OrbitalVertex& other_  )
    : Vertex( other_ )
    , _center( other_.center( ))
    , _tangent( other_.tangent( ))
  {

  }

  OrbitalVertex::~OrbitalVertex( void )
  {

  }

  Eigen::Vector3f& OrbitalVertex::center( void )
  {
    return _center;
  }

  const Eigen::Vector3f& OrbitalVertex::center( void ) const
  {
    return _center;
  }

  Eigen::Vector3f& OrbitalVertex::tangent( void )
  {
    return _tangent;
  }

  const Eigen::Vector3f& OrbitalVertex::tangent( void ) const
  {
    return _tangent;
  }

  void OrbitalVertex::displace( const Eigen::Vector3f& displacement_ )
  {
    _position += displacement_;
    _center += displacement_;
  }

  void OrbitalVertex::place( const Eigen::Vector3f& placement_ )
  {
    Eigen::Vector3f axis = _position - _center;
    _center = placement_;
    _position = _center + axis;
  }

  void OrbitalVertex::rotate( const Eigen::Matrix3f& rotation_ )
  {
    Eigen::Vector3f axis = _position - _center;
    _position = _center + rotation_ * axis;
  }

  void OrbitalVertex::rotate( const Eigen::Quaternion< float >& rotation_ )
  {
    Eigen::Vector3f axis = _position - _center;
    _position = _center + rotation_ * axis;
  }

  void OrbitalVertex::norm( const float norm_ )
  {
    _position = ( _position - _center ).normalized( ) * norm_ + _center;
  }

  void OrbitalVertex::normalize( const Eigen::Vector3f& axis_ )
  {
    _position = _center + axis_.normalized( );
  }

  void OrbitalVertex::store( std::vector< float >& buffer_,
                        const TAttribType attribType_ )
  {
    switch( attribType_ )
    {
    case CENTER:
      buffer_.push_back( _center.x( ));
      buffer_.push_back( _center.y( ));
      buffer_.push_back( _center.z( ));
      break;
    case TANGENT:
      buffer_.push_back( _tangent.x( ));
      buffer_.push_back( _tangent.y( ));
      buffer_.push_back( _tangent.z( ));
      break;
    default:
      Vertex::store( buffer_, attribType_ );
      break;
    }
  }

  VertexPtr OrbitalVertex::clone( void )
  {
    return new OrbitalVertex( *this );
  }

} // namespace nlgeometry
