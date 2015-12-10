/**
 * @file    SpatialHashtable.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "SpatialHashTable.h"

#include <iostream>

namespace nlgeometry
{
  SpatialHashTable::SpatialHashTable( unsigned int size_ , float cellSize_,
                                      float tolerance_,
                                      unsigned int prime1_,
                                      unsigned int prime2_,
                                      unsigned int prime3_ )
    : _size( size_ )
    , _cellSize( cellSize_ )
    , _tolerance( tolerance_ )
    , _prime1( prime1_ )
    , _prime2( prime2_ )
    , _prime3( prime3_ )
  {
    _table.resize( _size );
  }

  SpatialHashTable::~SpatialHashTable( void )
  {

  }

  VertexPtr SpatialHashTable::insert( const VertexPtr& vertex_ )
  {
    VertexPtr v = vertex_;
    unsigned int i = floor( v->position( ).x( ) / _cellSize );
    unsigned int j = floor( v->position( ).y( ) / _cellSize );
    unsigned int k = floor( v->position( ).z( ) / _cellSize );

    unsigned int tableId = ( i * _prime1 + j * _prime2 + k * _prime3 ) % _size;

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
    unsigned int id = 1;
    for ( const Vertices& list: _table )
    {
      for ( const VertexPtr& vertex: list )
      {
        vertex->id( ) = id;
        id ++;
        vertices_.push_back( vertex );
      }
    }
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

}// end namespace nlgeometry
