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
#include "Mesh.h"

//OpenGL
#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
#include <GL/glew.h>
#endif
#ifdef Darwin
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace nlgeometry
{

  Mesh::Mesh( void )
    : _vao( GL_INVALID_VALUE )
    , _trianglesSize( 0 )
    , _quadsSize( 0 )
  {
    _modelMatrix = Eigen::Matrix4f::Identity( );
  }

  Mesh::~Mesh( void )
  {
    clearCPUData( );
    clearGPUData( );
  }

  void Mesh::init( void )
  {

  }

  Vertices& Mesh::vertices( void )
  {
    return _vertices;
  }

  Facets& Mesh::triangles( void )
  {
    return _triangles;
  }

  Facets& Mesh::quads( void )
  {
    return _quads;
  }

  AxisAlignedBoundingBox& Mesh::aaBoundingBox( void )
  {
    return _aaBoundingBox;
  }

  Eigen::Matrix4f& Mesh::modelMatrix( void )
  {
    return _modelMatrix;
  }

  float* Mesh::modelMatrixVectorized( void )
  {
   return _modelMatrix.transpose( ).data( );
  }

  void Mesh::clearCPUData( void )
  {
    for ( auto vertex: _vertices )
      delete vertex;
    _vertices.clear( );
    _triangles.clear( );
    _quads.clear( );
  }

  void Mesh::clearGPUData( void )
  {
    if ( _vao != GL_INVALID_VALUE )
      glDeleteVertexArrays( 1, &_vao );
    if ( _vbos.size( ) > 0 )
      glDeleteBuffers( (GLsizei)_vbos.size( ), _vbos.data( ));
  }

  void Mesh::uploadGPU( AttribsFormat format_, Facet::TFacetType facetType_ )
  {
    clearGPUData( );

    Attribs attribs;
    attribs.resize( format_.size( ));
    std::vector< unsigned int > indices;

    for ( auto vertex: _vertices )
      vertex->store( attribs, format_ );

    for ( auto triangle: _triangles )
      triangle->addIndicesAs( facetType_, indices );
    _trianglesSize = (unsigned int)_triangles.size( ) * 3;

    for ( auto quad: _quads )
      quad->addIndicesAs( facetType_, indices );

    switch( facetType_ )
    {
    case Facet::TRIANGLES:
      _quadsSize = (unsigned int)_quads.size( ) * 6;
      break;
    case Facet::PATCHES:
      _quadsSize = (unsigned int)_quads.size( ) * 4;
      break;
    }

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );

    _vbos.resize( attribs.size(  ) + 1 );
    glGenBuffers( ( unsigned int )attribs.size( ) + 1, _vbos.data( ));

    for ( unsigned int i = 0; i < attribs.size( ); i++ )
    {
      _uploadBuffer( attribs[i], format_[i], i );
      attribs[i].clear( );
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbos[attribs.size( )] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int) *
                  indices.size( ), indices.data( ), GL_STATIC_DRAW );
    indices.clear( );
    glBindVertexArray( 0 );
  }

  void Mesh::computeBoundingBox( void )
  {
    Eigen::Vector4f pos;
    for ( auto vertex: _vertices )
    {
      pos = _modelMatrix * Eigen::Vector4f( vertex->position( ).x( ),
                                            vertex->position( ).y( ),
                                            vertex->position( ).z( ), 1.0f );
      _aaBoundingBox.minimum( ).x( ) =
        ( pos.x( ) < _aaBoundingBox.minimum( ).x( )) ?
        pos.x( ): _aaBoundingBox.minimum( ).x( );
      _aaBoundingBox.minimum( ).y( ) =
        ( pos.y( ) < _aaBoundingBox.minimum( ).y( )) ?
        pos.y( ): _aaBoundingBox.minimum( ).y( );
      _aaBoundingBox.minimum( ).z( ) =
        ( pos.z( ) < _aaBoundingBox.minimum( ).z( )) ?
        pos.z( ): _aaBoundingBox.minimum( ).z( );
      _aaBoundingBox.maximum( ).x( ) =
        ( pos.x( ) > _aaBoundingBox.maximum( ).x( )) ?
        pos.x( ): _aaBoundingBox.maximum( ).x( );
      _aaBoundingBox.maximum( ).y( ) =
        ( pos.y( ) > _aaBoundingBox.maximum( ).y( )) ?
        pos.y( ): _aaBoundingBox.maximum( ).y( );
      _aaBoundingBox.maximum( ).z( ) =
        ( pos.z( ) > _aaBoundingBox.maximum( ).z( )) ?
        pos.z( ): _aaBoundingBox.maximum( ).z( );
    }
  }

  void Mesh::computeNormals( void )
  {
    for ( auto vertex: _vertices )
    {
      vertex->normal( ) = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    }
    for ( auto facet: _triangles )
    {
      Eigen::Vector3f exe0 = ( facet->vertex1( )->position( ) -
                              facet->vertex0( )->position( )).normalized( );
      Eigen::Vector3f exe1 = ( facet->vertex2( )->position( ) -
                              facet->vertex0( )->position( )).normalized( );
      Eigen::Vector3f normal = ( exe0.cross( exe1 )).normalized( );

      facet->vertex0( )->normal( ) += normal;
      facet->vertex1( )->normal( ) += normal;
      facet->vertex2( )->normal( ) += normal;
    }
    for ( auto facet: _quads )
    {
      Eigen::Vector3f exe0 = facet->vertex1( )->position( ) -
        facet->vertex0( )->position( );
      Eigen::Vector3f exe1 = facet->vertex2( )->position( ) -
        facet->vertex0( )->position( );
      Eigen::Vector3f normal = (exe0.cross( exe1 )).normalized( );

      facet->vertex0( )->normal( ) += normal;
      facet->vertex1( )->normal( ) += normal;
      facet->vertex2( )->normal( ) += normal;
      facet->vertex3( )->normal( ) += normal;
    }
    for ( auto vertex: _vertices )
    {
      vertex->normal( ).normalize( );
    }
  }

  void Mesh::renderTriangles( Facet::TFacetType facetType_ )
  {
    switch( facetType_ )
    {
    case Facet::TRIANGLES:
      glBindVertexArray( _vao );
      glDrawElements( GL_TRIANGLES, _trianglesSize, GL_UNSIGNED_INT,
                      (void*) 0 );
      break;
    case Facet::PATCHES:
      glBindVertexArray( _vao );
      glPatchParameteri( GL_PATCH_VERTICES, 3 );
      glDrawElements( GL_PATCHES, _trianglesSize, GL_UNSIGNED_INT,
                      (void*) 0 );
      break;
    }
  }

  void Mesh::renderQuads( Facet::TFacetType facetType_ )
  {
    switch( facetType_ )
    {
    case Facet::TRIANGLES:
      glBindVertexArray( _vao );
      glDrawElements( GL_TRIANGLES, _quadsSize, GL_UNSIGNED_INT,
                      (void*) ( _trianglesSize * sizeof( unsigned int )));
      break;
    case Facet::PATCHES:
      glBindVertexArray( _vao );
      glPatchParameteri( GL_PATCH_VERTICES, 4 );
      glDrawElements( GL_PATCHES, _quadsSize, GL_UNSIGNED_INT,
                      (void*) ( _trianglesSize * sizeof( unsigned int )));
      break;
    }
  }

  void Mesh::render( void )
  {
    renderTriangles( );
    renderQuads( );
  }

  void Mesh::_uploadBuffer( std::vector< float >& buffer_, TAttribType type_,
                            unsigned int vaoPosition_ )
  {
    unsigned int numComponents;
    switch( type_ )
    {
    case UV:
      numComponents = 2;
    default:
      numComponents = 3;
      break;
    }
    glBindBuffer( GL_ARRAY_BUFFER, _vbos[vaoPosition_]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * buffer_.size( ),
                  buffer_.data( ), GL_STATIC_DRAW );
    glVertexAttribPointer( vaoPosition_, numComponents,
                           GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( vaoPosition_ );
  }

} // namespace nlgeometry
