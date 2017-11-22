/*
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
#include "Vertex.h"

namespace nlgeometry
{

  Vertex::Vertex(  const Eigen::Vector3f& position_,
                   const Eigen::Vector3f& normal_,
                   const Eigen::Vector3f& color_,
                   const Eigen::Vector2f& uv_ )
    : _id( 0 )
    , _position( position_ )
    , _normal( normal_ )
    , _color( color_ )
    , _uv( uv_ )
    , _stored( false )
  {

  }

  Vertex::Vertex( const Vertex& other_ )
    : _id( 0 )
    , _position( other_.position( ))
    , _normal( other_.normal( ))
    , _color( other_.color( ))
    , _uv( other_.uv( ))
    , _stored( false )
  {

  }

  Vertex::~Vertex( void )
  {

  }

  unsigned int& Vertex::id( void )
  {
    return _id;
  }

  const unsigned int& Vertex::id( void ) const
  {
    return _id;
  }

  Eigen::Vector3f& Vertex::position( void )
  {
    return _position;
  }

  const Eigen::Vector3f& Vertex::position( void ) const
  {
    return _position;
  }

  Eigen::Vector3f& Vertex::normal( void )
  {
    return _normal;
  }

  const Eigen::Vector3f& Vertex::normal( void ) const
  {
    return _normal;
  }

  Eigen::Vector3f& Vertex::color( void )
  {
    return _color;
  }

  const Eigen::Vector3f& Vertex::color( void ) const
  {
    return _color;
  }

  Eigen::Vector2f& Vertex::uv( void )
  {
    return _uv;
  }

  const Eigen::Vector2f& Vertex::uv( void ) const
  {
    return _uv;
  }

  void Vertex::displace( const Eigen::Vector3f& displacement_ )
  {
    _position += displacement_;
  }

  void Vertex::place( const Eigen::Vector3f& placement_ )
  {
    _position = placement_;
  }

  void Vertex::rotate( const Eigen::Matrix3f& rotation_ )
  {
    _position = rotation_ * _position;
    _normal = rotation_ * _normal;
  }

  void Vertex::rotate( const Eigen::Quaternion< float >& rotation_ )
  {
    _position = rotation_ * _position;
    _normal = rotation_ * _normal;
  }

  void Vertex::norm( const float norm_ )
  {
    _position = ( norm_ / _position.norm( )) * _position;
  }

  void Vertex::store( std::vector< float >& buffer_,
                      const TAttribType attribType_ )
  {
    switch( attribType_ )
    {
    case POSITION:
      _id = ( unsigned int )buffer_.size( ) / 3;
      buffer_.push_back( _position.x( ) );
      buffer_.push_back( _position.y( ) );
      buffer_.push_back( _position.z( ) );
      break;
    case NORMAL:
      buffer_.push_back( _normal.x( ) );
      buffer_.push_back( _normal.y( ) );
      buffer_.push_back( _normal.z( ) );
      break;
    case COLOR:
      buffer_.push_back( _color.x( ) );
      buffer_.push_back( _color.y( ) );
      buffer_.push_back( _color.z( ) );
      break;
    case UV:
      buffer_.push_back( _uv.x( ) );
      buffer_.push_back( _uv.y( ) );
      break;
    default:
      break;
    }
  }

  void Vertex::store( Attribs& attribs_, const AttribsFormat format_ )
  {
    if ( attribs_.size( ) != format_.size( ))
      throw std::runtime_error(
        "Vertex attribs format and attribs buffers have differents sizes." );
    if ( !_stored )
    {
      for ( unsigned int i = 0; i < attribs_.size( ); i++ )
      {
          store( attribs_[i], format_[i]);
      }
      _stored = true;
    }
  }

  void Vertex::cleanStoreStatus( void )
  {
    _stored = false;
  }

  VertexPtr Vertex::clone( void )
  {
    return new Vertex( *this );
  }

} // namespace nlgeometry
