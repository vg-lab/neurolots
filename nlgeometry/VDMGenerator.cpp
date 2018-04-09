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
#include "VDMGenerator.h"

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

#include <iostream>

namespace nlgeometry
{

  VDMGenerator* VDMGenerator::_mpInstance = nullptr;

  VDMGenerator::VDMGenerator( void )
    : _cpuTessQuad( nullptr )
    , _vdmSize( 33 )
  {
    _generateTessQuad( );
  }

  VDMGenerator* VDMGenerator::Instance( )
  {
    if ( !_mpInstance )
    {
      _mpInstance = new VDMGenerator;
    }
    return _mpInstance;
  }

  std::vector< float > VDMGenerator::vectorDisplacementMapRaw(
    MeshPtr mesh_, Parametrizer::TVertexWeightMethod  paraMethod0_,
    Parametrizer::TVertexWeightMethod paraMethod1_, const float alpha0_,
    const float alpha1_ )
  {
    std::vector< float > raw;
    unsigned int borderVerticesSize =
      Parametrizer::Instance( )->doublePassParametrize( mesh_, paraMethod0_,
                                                        paraMethod1_, alpha0_,
                                                        alpha1_ );

    for ( auto vertex: _cpuTessQuadVertices )
    {
      Eigen::Vector3f intersectionPoint =
        _matchPoint( mesh_->triangles( ), mesh_->vertices( ),
                     borderVerticesSize, vertex );
      Eigen::Vector3f displacement = intersectionPoint - vertex->position( );

      raw.push_back( displacement.x( ));
      raw.push_back( displacement.y( ));
      raw.push_back( displacement.z( ));
    }

    return raw;
  }

  unsigned int VDMGenerator::vectorDisplacementMapTexture(
    MeshPtr mesh_, Parametrizer::TVertexWeightMethod paraMethod0_,
    Parametrizer::TVertexWeightMethod paraMethod1_, const float alpha0_,
    const float alpha1_ )
  {
    auto raw = vectorDisplacementMapRaw( mesh_, paraMethod0_, paraMethod1_,
                                         alpha0_, alpha1_ );
    unsigned int texture;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, _vdmSize, _vdmSize, 0,
                  GL_RGB, GL_FLOAT, raw.data( ));
    glGenerateMipmap( GL_TEXTURE_2D );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
    return texture;
  }

  void VDMGenerator::vdmSize( unsigned int size_ )
  {
    _vdmSize = size_;
    _generateTessQuad( );
  }

  unsigned int VDMGenerator::vdmSize( void )
  {
    return _vdmSize;
  }

  MeshPtr VDMGenerator::cpuTessellatedQuad( void )
  {
    return _cpuTessQuad;
  }

  void VDMGenerator::_generateTessQuad( void )
  {
    if ( _cpuTessQuad )
      delete _cpuTessQuad;
    _cpuTessQuad = new Mesh( );
    Vertices& vertices = _cpuTessQuad->vertices( );
    Facets& triangles = _cpuTessQuad->triangles( );

    unsigned int dim = _vdmSize;
    unsigned int subdivisions = dim - 1;
    float increment = 2.0f / subdivisions;

    for ( unsigned int i = 0; i < dim; i++ )
    {
      for ( unsigned int j = 0; j < dim; j++ )
      {
        float xCoord = -1.0f + j * increment;
        float yCoord = -1.0f + i * increment;
          vertices.push_back( new Vertex(
                                Eigen::Vector3f( xCoord, yCoord, 0.0f  )));
      }
    }

    for ( unsigned int i = 0; i < dim - 1 ; i++ )
    {
      for ( unsigned int j = 0; j < dim - 1; j++ )
      {
        unsigned int id0 = i * dim + j;
        unsigned int id1 = i * dim + ( j + 1 );
        unsigned int id2 = ( i + 1 ) * dim + ( j + 1 );
        unsigned int id3 = ( i + 1 ) * dim + j ;

        if (( j < dim / 2 && i >= dim / 2 ) || ( j >= dim / 2 && i < dim / 2 ))
        {
          unsigned int idAux0 = id0;
          unsigned int idAux1 = id1;
          unsigned int idAux2 = id2;
          unsigned int idAux3 = id3;
          id0 = idAux1; id1 = idAux2; id2 = idAux3; id3 = idAux0;
        }
        triangles.push_back( new Facet( vertices[id0], vertices[id1],
                                        vertices[id2] ));
        triangles.push_back( new Facet( vertices[id0], vertices[id2],
                                        vertices[id3] ));
      }
    }
    _cpuTessQuadVertices = _cpuTessQuad->vertices( );
    Parametrizer::Instance( )->parametrize( _cpuTessQuad );
  }

  Eigen::Vector3f VDMGenerator::_barycentricCoords( const FacetPtr facet_,
                                                    const VertexPtr vertex_ )
  {
    Eigen::Matrix3f A;
    Eigen::Vector3f b( vertex_->uv( ).x( ), vertex_->uv( ).y( ), 1.0f );

    Eigen::Vector2f uv0 = facet_->vertex0( )->uv( );
    Eigen::Vector2f uv1 = facet_->vertex1( )->uv( );
    Eigen::Vector2f uv2 = facet_->vertex2( )->uv( );

    A <<
      uv0.x( ), uv1.x( ), uv2.x( ),
      uv0.y( ), uv1.y( ), uv2.y( ),
      1.0f, 1.0f, 1.0f;

    return A.colPivHouseholderQr( ).solve( b );
  }

  bool VDMGenerator::_edgeNearestPoint( const VertexPtr edge0_,
                                        const VertexPtr edge1_,
                                        const VertexPtr vertex_,
                                        Eigen::Vector3f& nearestPoint_ )
  {
    float angleEdge0 = atan2( edge0_->uv( ).y( ), edge0_->uv( ).x( ));
    if ( angleEdge0 < 0.0f )
      angleEdge0 += 2.0f * M_PI;
    float angleEdge1 = atan2( edge1_->uv( ).y( ), edge1_->uv( ).x( ));
    if ( angleEdge1 < 0.0f )
      angleEdge1 += 2.0f * M_PI;
    float angleVertex = atan2( vertex_->uv( ).y( ), vertex_->uv( ).x( ));
    if ( angleVertex < 0.0f )
      angleVertex += 2.0f * M_PI;

    if ( angleEdge0 > angleEdge1 )
      angleEdge1 += 2.0f * M_PI;

    if ( angleVertex >= angleEdge0 && angleVertex <= angleEdge1)
    {
      float alpha = ( angleVertex - angleEdge0 ) / ( angleEdge1 - angleEdge0 );
      nearestPoint_ = edge0_->position( ) +
        alpha * ( edge1_->position( ) - edge0_->position( ));
      return true;
    }
    return false;
  }

  Eigen::Vector3f VDMGenerator::_matchPoint( const Facets& triangles_,
                                            const Vertices& vertices_,
                                            unsigned int borderVerticesSize_,
                                            const VertexPtr vertex_ )
  {
    Eigen::Vector3f coord;
    bool innerFound = false;
    Eigen::Vector3f intersectionPoint;

    for ( auto triangle: triangles_ )
    {
      coord = _barycentricCoords( triangle, vertex_ );
      float coordSum = coord.x( ) + coord.y( ) + coord.z( );
      innerFound = ( coord.x( ) > 0.0f && coord.x( ) < 1.0f ) &&
        ( coord.y( ) > 0.0f && coord.y( ) < 1.0f ) &&
        ( coord.z( ) > 0.0f && coord.z( ) < 1.0f ) &&
        ( coordSum > 0.99999f && coordSum < 1.000001f );
      if ( innerFound )
      {
        intersectionPoint =
          triangle->vertex0( )->position( ) * coord.x( ) +
          triangle->vertex1( )->position( ) * coord.y( ) +
          triangle->vertex2( )->position( ) * coord.z( );
        break;
      }
    }

    if ( !innerFound )
    {
      for ( unsigned int i = 0; i < borderVerticesSize_; i++ )
      {
        unsigned int id0 = i;
        unsigned int id1 = ( i + 1 ) % borderVerticesSize_;
        if ( _edgeNearestPoint( vertices_[id0], vertices_[id1], vertex_,
                                intersectionPoint ))
        {
          break;
        }
      }
    }

    return intersectionPoint;

  }

} // namespace nlgeometry
