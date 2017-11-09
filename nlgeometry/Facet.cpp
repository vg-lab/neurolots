/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of nsol <https://github.com/gmrvvis/neurolots>
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
#include "Facet.h"

namespace nlgeometry
{

  Facet::Facet( VertexPtr vertex0_, VertexPtr vertex1_, VertexPtr vertex2_,
                VertexPtr vertex3_ )
    : _vertex0( vertex0_ )
    , _vertex1( vertex1_ )
    , _vertex2( vertex2_ )
    , _vertex3( vertex3_ )
  {

  }

  Facet::~Facet( void )
  {

  }

  VertexPtr Facet::vertex0( void )
  {
    return _vertex0;
  }

  VertexPtr Facet::vertex0( void ) const
  {
    return _vertex0;
  }

  VertexPtr Facet::vertex1( void )
  {
    return _vertex1;
  }

  VertexPtr Facet::vertex1( void ) const
  {
    return _vertex1;
  }

  VertexPtr Facet::vertex2( void )
  {
    return _vertex2;
  }

  VertexPtr Facet::vertex2( void ) const
  {
    return _vertex2;
  }

  VertexPtr Facet::vertex3( void )
  {
    return _vertex3;
  }

  VertexPtr Facet::vertex3( void ) const
  {
    return _vertex3;
  }

  void Facet::store( Attribs& attribs_, const AttribsFormat format_ )
  {
    if ( _vertex0 )
      _vertex0->store( attribs_, format_ );
    if ( _vertex1 )
      _vertex1->store( attribs_, format_ );
    if ( _vertex2 )
      _vertex2->store( attribs_, format_ );
    if ( _vertex3 )
      _vertex3->store( attribs_, format_ );
  }

  void Facet::cleanStoreStatus( void )
  {
    if ( _vertex0 )
      _vertex0->cleanStoreStatus( );
    if ( _vertex1 )
      _vertex1->cleanStoreStatus( );
    if ( _vertex2 )
      _vertex2->cleanStoreStatus( );
    if ( _vertex3 )
      _vertex3->cleanStoreStatus( );
  }

  std::vector< unsigned int > Facet::getIndicesAs( TFacetType facetType_ ) const
  {
    std::vector< unsigned int > indices;

    if ( !_vertex0 | !_vertex1 | !_vertex2 )
      return indices;

    if ( _vertex3 )
    {
      switch( facetType_ )
      {
      case TRIANGLES:
        indices.push_back( _vertex0->id( ));
        indices.push_back( _vertex1->id( ));
        indices.push_back( _vertex2->id( ));
        indices.push_back( _vertex1->id( ));
        indices.push_back( _vertex3->id( ));
        indices.push_back( _vertex2->id( ));
        break;
      case PATCHES:
        indices.push_back( _vertex0->id( ));
        indices.push_back( _vertex1->id( ));
        indices.push_back( _vertex2->id( ));
        indices.push_back( _vertex3->id( ));
        break;
      }
    }
    else
    {
      indices.push_back( _vertex0->id( ));
      indices.push_back( _vertex1->id( ));
      indices.push_back( _vertex2->id( ));
    }

    return indices;
  }

  void Facet::addIndicesAs( TFacetType facetType_,
                            std::vector< unsigned int >& indices_ ) const
  {
    auto indices = getIndicesAs( facetType_);
    indices_.insert( indices_.end( ), indices.begin(), indices.end( ));
  }

} // end namespace nlgeometry
