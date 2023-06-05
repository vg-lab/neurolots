/**
 * Copyright (c) 2015-2018 GMRV/URJC.
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
#include "VDMap.h"

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

#include <cmath>

#define MAX_TESS_LEVEL 65.0f

namespace nlgeometry
{

  VDMap::VDMap( void )
    : _vdmTexture( nullptr )
    , _normalTexture( nullptr )
    , _size( 0 )
  {

  }

  VDMap::~VDMap( void )
  {
    if ( _vdmTexture )
      delete _vdmTexture;
    if ( _normalTexture )
      delete _normalTexture;
  }

  reto::Texture2D* VDMap::vdmTexture( void )
  {
    return _vdmTexture;
  }

  reto::Texture2D* VDMap::normalTexture( void )
  {
    return _normalTexture;
  }

  void VDMap::vdmTexture( reto::Texture2D* vdmTexture_ )
  {
    _vdmTexture = vdmTexture_;
  }

  void VDMap::normalTexture( reto::Texture2D* normalTexture_ )
  {
    _normalTexture = normalTexture_;
  }

  unsigned int& VDMap::size( void )
  {
    return _size;
  }

    PCAComponentMap::PCAComponentMap( void )
    : _textureComponents( nullptr )
    , _size( 0 )
  {

  }

  PCAComponentMap::~PCAComponentMap( void )
  {
    if ( _textureComponents )
      delete _textureComponents;
  }

  reto::Texture2DArray* PCAComponentMap::textureComponents( void )
  {
    return _textureComponents;
  }


  void PCAComponentMap::textureComponents( reto::Texture2DArray* textureComponents_ )
  {
    _textureComponents = textureComponents_;
  }

  unsigned int& PCAComponentMap::size( void )
  {
    return _size;
  }


  VDMapCollection::VDMapCollection( void )
    : _vdmapsSize( 0 )
    , _macroMap( nullptr )
    , _macroMapSpines( 0 )
  {

  }

  VDMapCollection::~VDMapCollection( void )
  {
    for ( auto vdmap: _vdmaps )
    {
      if ( vdmap )
        delete vdmap;
    }
    _vdmaps.clear( );
    _models.clear( );
    _macroModels.clear( );

    delete( _macroMap );
    if ( _vao != GL_INVALID_VALUE )
      glDeleteVertexArrays( 1, &_vao );
    if ( _vbos.size( ) > 0 )
      glDeleteBuffers( (GLsizei)_vbos.size( ), _vbos.data( ));
  }

  std::vector< VDMapPtr > VDMapCollection::vdmaps( void )
  {
    return _vdmaps;
  }

  std::vector< Eigen::Matrix4f >& VDMapCollection::models( void )
  {
    return _models;
  }

  std::vector< Eigen::Matrix4f >& VDMapCollection::macroModels( void )
  {
    return _macroModels;
  }

  unsigned int VDMapCollection::vdmapsSize( void )
  {
    return _vdmapsSize;
  }

  void VDMapCollection::vdmapsSize( unsigned int vdmapsSize_ )
  {
    _vdmapsSize = vdmapsSize_;
  }

  bool VDMapCollection::addVDMap( VDMapPtr vdmap_, Eigen::Matrix4f model_ )
  {
    if ( _vdmapsSize == vdmap_->size( ) || _vdmaps.size( ) == 0 )
    {
      if ( _vdmaps.size( ) == 0 )
      {
        _vdmapsSize = vdmap_->size( );
      }
      _vdmaps.push_back( vdmap_ );
      _models.push_back( model_ );
      return true;
    }
    return false;
  }

  void VDMapCollection::computeMacroMap( void )
  {
    _macroMapSpines = _vdmaps.size( );
    unsigned int spinesPerSide = std::ceil( std::sqrt( _macroMapSpines ));
    unsigned int macroMapSize = spinesPerSide * _vdmapsSize;

    std::vector< float > dis( macroMapSize * macroMapSize * 3 );
    std::vector< float > normals( macroMapSize * macroMapSize * 3 );
    unsigned int pixelsSize = _vdmapsSize *_vdmapsSize * 3;
    std::vector< float > localDis( pixelsSize );
    std::vector< float > localNormals( pixelsSize );

    for ( unsigned int spine = 0; spine < _macroMapSpines; spine++ )
    {
      _vdmaps[spine]->vdmTexture( )->bind( );
      glGetTexImage( _vdmaps[spine]->vdmTexture( )->target( ), 0, GL_RGB,
                     GL_FLOAT, localDis.data( ));
      _vdmaps[spine]->normalTexture( )->bind( );
      glGetTexImage( _vdmaps[spine]->normalTexture( )->target( ), 0, GL_RGB,
                     GL_FLOAT, localNormals.data( ));

      unsigned int spineCoordX = spine % spinesPerSide;
      unsigned int spineCoordY = spine / spinesPerSide;
      for ( unsigned int localX = 0; localX < _vdmapsSize; localX++ )
      {
        for ( unsigned int localY = 0; localY < _vdmapsSize; localY++ )
        {
          unsigned int globalX = localX + spineCoordX * _vdmapsSize;
          unsigned int globalY = localY + spineCoordY * _vdmapsSize;

          unsigned int localSerial = ( localX + localY * _vdmapsSize ) * 3;
          unsigned int globalSerial = ( globalX + globalY * macroMapSize ) * 3;

          dis[globalSerial] = localDis[localSerial];
          dis[globalSerial+1] = localDis[localSerial+1];
          dis[globalSerial+2] = localDis[localSerial+2];

          normals[globalSerial] = localNormals[localSerial];
          normals[globalSerial+1] = localNormals[localSerial+1];
          normals[globalSerial+2] = localNormals[localSerial+2];
        }
      }
    }

    auto texConfig = reto::TextureConfig( );
    texConfig.internalFormat = GL_RGB32F;
    texConfig.format = GL_RGB;
    texConfig.type = GL_FLOAT;
    texConfig.wrapS = GL_CLAMP_TO_EDGE;
    texConfig.wrapT = GL_CLAMP_TO_EDGE;

    auto vdmTexture =
      new reto::Texture2D( texConfig, dis.data( ), macroMapSize,
                           macroMapSize );
    auto normalTexture =
      new reto::Texture2D( texConfig, normals.data( ), macroMapSize,
                           macroMapSize );
    if ( _macroMap )
      delete _macroMap;
    _macroMap = new nlgeometry::VDMap( );
    _macroMap->vdmTexture( vdmTexture );
    _macroMap->normalTexture( normalTexture );
    _macroMap->size( ) = macroMapSize;

    _macroModels.clear( );
    for ( auto model: _models )
      _macroModels.push_back( model );

  }

  VDMapPtr VDMapCollection::macroMap( void )
  {
    return _macroMap;
  }

  void VDMapCollection::macroMap( VDMapPtr macroMap_ )
  {
    _macroMap = macroMap_;
  }

  unsigned int VDMapCollection::vdmapsNum( void )
  {
    return _macroMapSpines;
  }

  void VDMapCollection::vdmapsNum( unsigned int vdmapsNum_ )
  {
    _macroMapSpines = vdmapsNum_;
  }

  void VDMapCollection::uploadGPU( void )
  {
    if ( _macroMap && _macroModels.size( ) > 0 )
    {
      unsigned int numSegments = ceil( _vdmapsSize / MAX_TESS_LEVEL );

      _vbos.resize( 4 );
      glGenVertexArrays( 1, &_vao );
      glBindVertexArray( _vao );
      glGenBuffers( 4, _vbos.data( ));

      std::vector< float > positions;
      float increment = 2.0f / numSegments;
      unsigned int numVertices = numSegments + 1;
      for ( unsigned int i = 0; i < numVertices; i++ )
      {
        for ( unsigned int j = 0; j < numVertices; j++ )
        {
          positions.push_back( j * increment - 1.0f );
          positions.push_back( i * increment - 1.0f );
          positions.push_back( 0.0f );
        }
      }

      std::vector< float > spineCoords;
      int spinesPerSide =
        _macroMap->size( ) / _vdmapsSize;
      for ( unsigned int i = 0; i < _macroMapSpines; i++ )
      {
        spineCoords.push_back( i % spinesPerSide );
        spineCoords.push_back( i / spinesPerSide );
      //   spineCoords.push_back( 1.0f );
      //   spineCoords.push_back( 1.0f );
      }

      std::vector< float > models;

      for ( auto model: _macroModels )
      {
        auto tran = model.transpose( );
        std::vector< float >  tranVec{ tran.data( ), tran.data( ) + 16 };
        models.insert( models.end( ), tranVec.begin( ), tranVec.end( ));
      }

      std::vector< unsigned int > indices;
      for ( unsigned int i = 0; i < numSegments; i++ )
      {
        for ( unsigned int j = 0; j < numSegments; j++ )
        {
          unsigned int id0 = j + i * numVertices;
          unsigned int id1 = j+1 + i * numVertices;
          unsigned int id2 = j + (i+1) * numVertices;
          unsigned int id3 = j+1 + (i+1) * numVertices;
          indices.push_back( id0 );
          indices.push_back( id2 );
          indices.push_back( id1 );
          indices.push_back( id3 );
        }
      }

      glBindBuffer( GL_ARRAY_BUFFER, _vbos[0]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * positions.size( ),
                    positions.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( 0 );
      glVertexAttribDivisor( 0, 0 );

      glBindBuffer( GL_ARRAY_BUFFER, _vbos[1]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( int ) * spineCoords.size( ),
                    spineCoords.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( 1 );
      glVertexAttribDivisor( 1, 1 );

      glBindBuffer( GL_ARRAY_BUFFER, _vbos[2]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * models.size( ),
                    models.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof(float)*4*4,
                             (void*)(0));
      glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof(float)*4*4,
                             (void*)(sizeof(float)*4));
      glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof(float)*4*4,
                             (void*)(sizeof(float)*8));
      glVertexAttribPointer( 5, 4, GL_FLOAT, GL_FALSE, sizeof(float)*4*4,
                             (void*)(sizeof(float)*12));
      glEnableVertexAttribArray( 2 );
      glEnableVertexAttribArray( 3 );
      glEnableVertexAttribArray( 4 );
      glEnableVertexAttribArray( 5 );
      glVertexAttribDivisor( 2, 1 );
      glVertexAttribDivisor( 3, 1 );
      glVertexAttribDivisor( 4, 1 );
      glVertexAttribDivisor( 5, 1 );

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbos[3] );
      glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(unsigned int)*indices.size( ), indices.data( ),
                    GL_STATIC_DRAW );

      _indicesSize = indices.size( );
      positions.clear( );
      spineCoords.clear( );
      models.clear( );
      indices.clear( );
      glBindVertexArray( 0 );


    }
  }

  void VDMapCollection::render( void )
  {
    _macroMap->vdmTexture( )->bind( 0 );
    _macroMap->normalTexture( )->bind( 1 );
    glBindVertexArray( _vao );
    glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawElementsInstanced( GL_PATCHES, _indicesSize, GL_UNSIGNED_INT, 0,
                             _macroMapSpines );
  }
}
