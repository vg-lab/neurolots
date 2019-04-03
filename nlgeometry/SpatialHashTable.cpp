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

#include <iostream>

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



  unsigned int  TrianglesSpatialHashTable::_primeX = 73856093;
  unsigned int  TrianglesSpatialHashTable::_primeY = 19349663;
  unsigned int  TrianglesSpatialHashTable::_primeZ = 83492791;

  TrianglesSpatialHashTable::TrianglesSpatialHashTable(
    unsigned int size_ , float cellSize_ )
    : _size( size_ )
    , _cellSize( cellSize_ )
  {
    _table.resize( _size );
  }

  TrianglesSpatialHashTable::~TrianglesSpatialHashTable( void )
  {
    clear( );
    _table.clear( );
  }

  void TrianglesSpatialHashTable::clear( void )
  {
    for ( auto& cell: _table )
      cell.clear( );
  }
  void TrianglesSpatialHashTable::insert( const FacetPtr& facet_ )
  {
    auto indices = _tableId( facet_->vertex0( )->position( ));
    Eigen::Array3i minimum( indices );
    Eigen::Array3i maximum( indices );

    indices = _tableId( facet_->vertex1( )->position( ));
    minimum = minimum.min( Eigen::Array3i( indices ));
    maximum = maximum.max( Eigen::Array3i( indices ));

    indices = _tableId( facet_->vertex2( )->position( ));
    minimum = minimum.min( Eigen::Array3i( indices ));
    maximum = maximum.max( Eigen::Array3i( indices ));

    for ( int i = minimum.x( ); i <= maximum.x( ); i++ )
      for ( int j = minimum.y( ); j <= maximum.y( ); j++ )
        for ( int k = minimum.z( ); k <= maximum.z( ); k++ )
        {
          Eigen::Vector3i ids( i, j, k );
          auto& list0 = _table[ _tableId( ids )];
          list0.insert( std::make_pair( ids, facet_ ));
        }
  }

  std::unordered_set< FacetPtr > TrianglesSpatialHashTable::getNeighbors(
    const VertexPtr vertex_ )
  {
    std::unordered_set< FacetPtr > neighbors;

    unsigned int level = 0;
    while( neighbors.empty( ))
    {
      // std::cout << level << std::endl;
      neighbors = _getNeighbors( vertex_->position( ), level );
      level++;
    }

    return neighbors;
  }

  Eigen::Vector3i TrianglesSpatialHashTable::_tableId(
    const Eigen::Vector3f& pos_ )
  {
    int i = floor( pos_.x( ) / _cellSize );
    int j = floor( pos_.y( ) / _cellSize );
    int k = floor( pos_.z( ) / _cellSize );

    return Eigen::Vector3i( i, j, k );
  }

  unsigned int TrianglesSpatialHashTable::_tableId(
    const Eigen::Vector3i& indices_ )
  {
    return ( std::abs( indices_.x( )) * _primeX +
             std::abs( indices_.y( )) * _primeY +
             std::abs( indices_.z( )) * _primeZ ) % _size;
  }

  size_t TrianglesSpatialHashTable::_indicesHash(
    const Eigen::Vector3i& indices_ )
  {
    return ( std::abs( indices_.x( )) * _primeX +
             std::abs( indices_.y( )) * _primeY +
             std::abs( indices_.z( )) * _primeZ );
  }

  std::unordered_set< FacetPtr > TrianglesSpatialHashTable::_getNeighbors(
    const Eigen::Vector3f& pos_ )
  {
    auto indices = _tableId( pos_ );
    unsigned int tableId = _tableId( indices );
    std::unordered_set< FacetPtr > neighbors;
    for ( auto data: _table[tableId] )
    {
      if ( _indicesHash( indices ) == _indicesHash( data.first ))
      {
        neighbors.insert( data.second );
      }
    }
    return neighbors;
  }

  std::unordered_set< FacetPtr > TrianglesSpatialHashTable::_getNeighbors(
    const Eigen::Vector3f& pos_, unsigned int level_ )
  {
    std::unordered_set< FacetPtr > facets;
    unsigned int size = level_ * 2 + 1;
    Eigen::Vector3f pos0(
      pos_ - Eigen::Vector3f( _cellSize, _cellSize, _cellSize ) * level_ );
    for ( unsigned int i = 0;  i < size; i++ )
      for ( unsigned int j = 0; j < size; j++ )
        for ( unsigned int k = 0; k < size; k++ )
          if ( i == 0 || i == size-1 || j == 0 || j == size-1 ||
            k == 0 || k == size-1 )
          {
            Eigen::Vector3f pos = pos0 + Eigen::Vector3f(
              i * _cellSize, j * _cellSize, k * _cellSize );
            auto currentFacets = _getNeighbors( pos );
            facets.insert( currentFacets.begin( ), currentFacets.end( ));
          }

    return facets;
  }
} // end namespace nlgeometry
