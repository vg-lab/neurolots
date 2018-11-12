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
#ifndef __NLGEOMETRY_VDMAP__
#define __NLGEOMETRY_VDMAP__

#include <reto/reto.h>
#include <nlgeometry/api.h>

namespace nlgeometry
{
  class VDMap
  {

  public:

    NLGEOMETRY_API
    VDMap( void );

    NLGEOMETRY_API
    ~VDMap( void );

    NLGEOMETRY_API
    reto::Texture2D* vdmTexture( void );

    NLGEOMETRY_API
    reto::Texture2D* normalTexture( void );

    NLGEOMETRY_API
    void vdmTexture( reto::Texture2D* vdmTexture_ );

    NLGEOMETRY_API
    void normalTexture( reto::Texture2D* normalTexture_ );

    NLGEOMETRY_API
    unsigned int& size( void );

  private:

    reto::Texture2D* _vdmTexture;

    reto::Texture2D* _normalTexture;

    unsigned int _size;
  };

  typedef VDMap* VDMapPtr;

  class VDMapCollection
  {

  public:

    NLGEOMETRY_API
    VDMapCollection( void );

    NLGEOMETRY_API
    ~VDMapCollection( void );

    NLGEOMETRY_API
    std::vector< VDMapPtr > vdmaps( void );

    NLGEOMETRY_API
    std::vector< Eigen::Matrix4f >& models( void );

    NLGEOMETRY_API
    std::vector< Eigen::Matrix4f >& macroModels( void );

    NLGEOMETRY_API
    unsigned int vdmapsSize( void );

    NLGEOMETRY_API
    void vdmapsSize( unsigned int vdmapsSize_ );

    NLGEOMETRY_API
    bool addVDMap( VDMapPtr vdmap_,
                   Eigen::Matrix4f model_ = Eigen::Matrix4f::Identity( ) );

    NLGEOMETRY_API
    void computeMacroMap( void );

    NLGEOMETRY_API
    VDMapPtr macroMap( void );

    NLGEOMETRY_API
    void macroMap( VDMapPtr macroMap_ );

    NLGEOMETRY_API
    unsigned int vdmapsNum( void );

    NLGEOMETRY_API
    void vdmapsNum( unsigned int vdmapsNum_ );

    NLGEOMETRY_API
    void uploadGPU( void );

    NLGEOMETRY_API
    void render( void );

  protected:

    std::vector< VDMapPtr > _vdmaps;

    std::vector< Eigen::Matrix4f > _models;

    unsigned int _vdmapsSize;

    VDMapPtr _macroMap;

    std::vector< Eigen::Matrix4f > _macroModels;

    unsigned int _macroMapSpines;

    std::vector< unsigned int > _vbos;

    unsigned int _vao;

    unsigned int _indicesSize;
  };

  typedef VDMapCollection* VDMapCollectionPtr;

}

#endif
