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
#ifndef __NLGEOMETRY_VDMGENERATOR__
#define __NLGEOMETRY_VDMGENERATOR__

#include "Parametrizer.h"

#include <reto/reto.h>
#include <nlgeometry/api.h>


namespace nlgeometry
{
  class VDMap
  {

  public:

    NLGEOMETRY_API
    VDMap( void ) : _vdmTexture( nullptr ), _normalTexture( nullptr ),  _size( 0 ){ }

    NLGEOMETRY_API
    ~VDMap( void ) { delete _vdmTexture; delete _normalTexture; }

    NLGEOMETRY_API
    reto::Texture2D*& vdmTexture( void ) { return _vdmTexture; }

    NLGEOMETRY_API
    reto::Texture2D*& normalTexture( void ) { return _normalTexture; }

    NLGEOMETRY_API
    unsigned int& size( void ) { return _size; }

  private:

    reto::Texture2D* _vdmTexture;

    reto::Texture2D* _normalTexture;

    unsigned int _size;
  };

  typedef VDMap* VDMapPtr;

  /*! \class VectorDisplacementMapGenerator
    \brief A singleton class to generate vector displacement maps from
    geometric meshes
  */
  class VDMGenerator
  {

  public:

    /**
     * Returns the instace of the singleton class
     * @return the VDMGenerator instance
     */
    NLGEOMETRY_API
    static VDMGenerator* Instance( void );

    /**
     * Method to compute the vector displacement map for a given mesh
     * @param mesh_ mesh to compute it vector displacement map
     * @return vector displacement map texture 2D
     */
    VDMapPtr vectorDisplacementMapTexture(
      MeshPtr mesh_, Parametrizer::TVertexWeightMethod paraMethod0_ =
      Parametrizer::MEAN_VALUE, Parametrizer::TVertexWeightMethod paraMethod1_ =
      Parametrizer::UNDEFINED, const float alpha0_ = 1.0f,
      const float alpha1_ = 0.5f,
      const float factor_ = 3.0f );

    /**
     * Method to set the resultant vector displacement map size
     * @param size_ size of the computed vector displacement map
     */
    void vdmapSize( unsigned int size_ );

    /**
     * Method that returns the size of the vector displacement maps
     * @return the size of the vector displacement maps
     */
    unsigned int vdmapSize( void );

    /**
     * Method that returns the cpu tessellated quad used
     * @return pointer to the mesh of the cpu tessellated quad
     */
    MeshPtr cpuTessellatedQuad( void );

  private:

    VDMGenerator( void );

    void _generateTessQuad( void );

    Eigen::Vector3f _barycentricCoords( const FacetPtr facet_,
                                        const VertexPtr vertex_ );

    bool _edgeNearestPoint( const VertexPtr edge0_,
                            const VertexPtr edge1_,
                            const VertexPtr vertex_,
                            Eigen::Vector3f& nearestPoint_,
                            Eigen::Vector3f& normal_,
                            const float factor_ = 5.0f );

    void _matchPoint( const Facets& triangles_,
                      const Vertices& vertices_,
                      unsigned int borderVerticesSize_,
                      const VertexPtr vertex_,
                      Eigen::Vector3f& intersectionPoint_,
                      Eigen::Vector3f& normal_,
                      const float factor_ = 5.0f );

    Eigen::Vector3f _coordRelax( const Eigen::Vector3f coord_,
                                 const float factor_ = 5.0f );

    //! Static instance of VDMGenerator singleton class
    static VDMGenerator* _mpInstance;

    //! Pointer to a mesh containing a cpu tessellated quad
    MeshPtr _cpuTessQuad;

    //! Vector of the cpu tessellated quad vertices with generation sort
    Vertices _cpuTessQuadVertices;

    //! Size of the vector displacement maps computed
    unsigned int _vdmSize;

  }; // class VDMGenerator

} // namespace nlgeometry

#endif
