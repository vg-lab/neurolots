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

#include "SpatialHashTable.h"

namespace nlgeometry
{

  SpatialHashTable::SpatialHashTable(
    unsigned int size_ , float cellSize_, float tolerance_,
    unsigned int primeX_, unsigned int primeY_, unsigned int primeZ_ )
    : _size( size_ )
    , _cellSize( cellSize_ )
    , _tolerance( tolerance_ )
    , _primeX( primeX_ )
    , _primeY( primeY_ )
    , _primeZ( primeZ_ )
  {
    _table.resize( _size );
  }

  SpatialHashTable::~SpatialHashTable( void )
  {
    for ( auto& cell: _table )
      cell.clear( );
    _table.clear( );
  }

  VertexPtr SpatialHashTable::insert( const VertexPtr& vertex_ )
  {
    VertexPtr v = vertex_;
    if ( std::isnan( v->position( ).x( )) | std::isnan( v->position( ).y( )) |
         std::isnan( v->position( ).z( )))
    {
      delete v;
      return nullptr;
    }
    unsigned int i = floor( std::abs( v->position( ).x( )) / _cellSize );
    unsigned int j = floor( std::abs( v->position( ).y( )) / _cellSize );
    unsigned int k = floor( std::abs( v->position( ).z( )) / _cellSize );

    unsigned int tableId = ( i * _primeX + j * _primeY + k * _primeZ ) % _size;

    Vertices& list = _table[ tableId ];

    bool found = false;
    for ( unsigned int r = 0; r < list.size(); r++ )
    {
      VertexPtr vAux = list[r];
      if( _equal( v, vAux ))
      {
        v = vAux;
        found = true;
        break;
      }
    }
    if( !found )
    {
      list.push_back( v );
    }

    return v;
  }

  void SpatialHashTable::vertices( Vertices& vertices_ ) const
  {
    vertices_.clear( );
    for ( const Vertices& list: _table )
      for ( const VertexPtr& vertex: list )
        vertices_.push_back( vertex );
  }

  bool SpatialHashTable::_equal( const VertexPtr v0, const VertexPtr v1 ) const
  {
    Eigen::Vector3f vpos = v1->position( ) +
      Eigen::Vector3f( _tolerance, _tolerance, _tolerance );
    Eigen::Vector3f vneg = v1->position( ) -
      Eigen::Vector3f( _tolerance, _tolerance, _tolerance );
    Eigen::Vector3f pos = v0->position( );
    return ( pos.x( ) <= vpos.x( ) && pos.x( ) >= vneg.x( ) &&
             pos.y( ) <= vpos.y( ) && pos.y( ) >= vneg.y( ) &&
             pos.z( ) <= vpos.z( ) && pos.z( ) >= vneg.z( ) );
  }

} // end namespace nlgeometry
