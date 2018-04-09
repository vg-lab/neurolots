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

  VDMapPtr VDMGenerator::vectorDisplacementMapTexture(
    MeshPtr mesh_, Parametrizer::TVertexWeightMethod  paraMethod0_,
    Parametrizer::TVertexWeightMethod paraMethod1_, const float alpha0_,
    const float alpha1_, const float factor_ )
  {
    auto vdmap = new VDMap( );

    auto texConfig = reto::TextureConfig( );
    texConfig.internalFormat = GL_RGB32F;
    texConfig.format = GL_RGB;
    texConfig.type = GL_FLOAT;
    texConfig.wrapS = GL_CLAMP_TO_EDGE;
    texConfig.wrapT = GL_CLAMP_TO_EDGE;
    unsigned int borderVerticesSize =
      Parametrizer::Instance( )->doublePassParametrize( mesh_, paraMethod0_,
                                                        paraMethod1_, alpha0_,
                                                        alpha1_ );
    mesh_->computeNormals( );
    std::vector< float > dis;
    std::vector< float > normals;
    for ( auto vertex: _cpuTessQuadVertices )
    {
      Eigen::Vector3f intersectionPoint;
      Eigen::Vector3f normal;
      _matchPoint( mesh_->triangles( ), mesh_->vertices( ),
                   borderVerticesSize, vertex, intersectionPoint,
                   normal, factor_ );
      Eigen::Vector3f displacement = intersectionPoint - vertex->position( );
      // vertex->color( ) = displacement;
      dis.push_back( displacement.x( ));
      dis.push_back( displacement.y( ));
      dis.push_back( displacement.z( ));
      normals.push_back( normal.x( ));
      normals.push_back( normal.y( ));
      normals.push_back( normal.z( ));
    }
    auto vdmTexture =
      new reto::Texture2D( texConfig, dis.data( ), _vdmSize, _vdmSize );
    auto normalTexture =
      new reto::Texture2D( texConfig, normals.data( ), _vdmSize, _vdmSize );

    vdmap->vdmTexture( ) = vdmTexture;
    vdmap->normalTexture( ) = normalTexture;
    vdmap->size( ) = _vdmSize;

    return vdmap;
  }

  void VDMGenerator::vdmapSize( unsigned int size_ )
  {
    _vdmSize = size_;
    _generateTessQuad( );
  }

  unsigned int VDMGenerator::vdmapSize( void )
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
    Parametrizer::Instance( )->doublePassParametrize( _cpuTessQuad,
                                                      Parametrizer::MEAN_VALUE,
                                                      Parametrizer::UNDEFINED,
                                                      1.0f, 0.5f );
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
                                        Eigen::Vector3f& nearestPoint_,
                                        Eigen::Vector3f& normal_,
                                        const float factor_ )
  {
    float invFactor = 1.0f / factor_;
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
      if ( alpha < 0.5f )
      {
        float invAlpha = pow( 1.0f - alpha, invFactor );
        alpha = 1.0 - invAlpha;
      }
      else
      {
        alpha = pow( alpha, invFactor );
      }

      nearestPoint_ = edge1_->position( ) * alpha + edge0_->position( ) * ( 1 - alpha );
//////////////////////////////
      // vertex_->uv( ) = edge1_->uv( ) * alpha + edge0_->uv( ) * ( 1 - alpha );
//////////////////////////////
      normal_ = edge1_->normal( ) * alpha + edge0_->normal( ) * ( 1 - alpha );
      return true;
    }

    return false;
  }

  void VDMGenerator::_matchPoint(
    const Facets& triangles_, const Vertices& vertices_,
    unsigned int borderVerticesSize_, const VertexPtr vertex_,
    Eigen::Vector3f& intersectionPoint_, Eigen::Vector3f& normal_,
    const float factor_ )
  {
    Eigen::Vector3f coord;
    bool innerFound = false;

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
        coord = _coordRelax( coord, factor_ );
        intersectionPoint_ =
          triangle->vertex0( )->position( ) * coord.x( ) +
          triangle->vertex1( )->position( ) * coord.y( ) +
          triangle->vertex2( )->position( ) * coord.z( );
        normal_ =
          triangle->vertex0( )->normal( ) * coord.x( ) +
          triangle->vertex1( )->normal( ) * coord.y( ) +
          triangle->vertex2( )->normal( ) * coord.z( );
//////////////////////////////////////////////
        // vertex_->uv( ) =
        //   triangle->vertex0( )->uv( ) * coord.x( ) +
        //   triangle->vertex1( )->uv( ) * coord.y( ) +
        //   triangle->vertex2( )->uv( ) * coord.z( );
//////////////////////////////////////////////
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
                                intersectionPoint_, normal_, factor_ ))
        {
          break;
        }
      }
    }
  }

  Eigen::Vector3f VDMGenerator::_coordRelax( const Eigen::Vector3f coord_,
                                             const float factor_ )
  {
    float invFactor = 1.0f / factor_;
    Eigen::Vector3f newCoord( 0.0f, 0.0f, 0.0f );

    if ( coord_.x( ) >= coord_.y( ) && coord_.x( ) >= coord_.z( ))
    {
      newCoord.x( ) = pow( coord_.x( ), invFactor );
      float rest = 1.0f - newCoord.x( );
      float factor = coord_.y( ) /(coord_.y( ) + coord_.z( ));
      newCoord.y( ) = rest * factor;
      newCoord.z( ) = rest - newCoord.y( );
    }
    else if ( coord_.y( ) >= coord_.z( ))
    {
      newCoord.y( ) = pow( coord_.y( ), invFactor );
      float rest = 1.0f - newCoord.y( );
      float factor = coord_.x( ) / (coord_.x( ) + coord_.z( ));
      newCoord.x( ) = rest * factor;
      newCoord.z( ) = rest - newCoord.x( );
    }
    else
    {
      newCoord.z( ) = pow( coord_.z( ), invFactor );
      float rest = 1.0f - newCoord.z( );
      float factor = coord_.x( ) / (coord_.x( ) + coord_.y( ));
      newCoord.x( ) = rest * factor;
      newCoord.y( ) = rest - newCoord.x( );
    }

    return newCoord;
  }
} // namespace nlgeometry
