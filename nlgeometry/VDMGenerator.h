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

#include <nlgeometry/api.h>

namespace nlgeometry
{

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
     * @return raw vector with the vector displacement map
     */
    std::vector< float > vectorDisplacementMapRaw(
      MeshPtr mesh_, Parametrizer::TVertexWeightMethod paraMethod0_ =
      Parametrizer::MEAN_VALUE, Parametrizer::TVertexWeightMethod paraMethod1_ =
      Parametrizer::UNDEFINED, const float alpha0_ = 1.0f,
      const float alpha1_ = 1.0f );

    /**
     * Method to compute the vector displacement map for a given mesh
     * @param mesh_ mesh to compute it vector displacement map
     * @return index to vector displacement map texture in the GPU
     */
    unsigned int vectorDisplacementMapTexture(
      MeshPtr mesh_, Parametrizer::TVertexWeightMethod paraMethod0_ =
      Parametrizer::MEAN_VALUE, Parametrizer::TVertexWeightMethod paraMethod1_ =
      Parametrizer::UNDEFINED, const float alpha0_ = 1.0f,
      const float alpha1_ = 1.0f );

    /**
     * Method to set the resultant vector displacement map size
     * @param size_ size of the computed vector displacement map
     */
    void vdmSize( unsigned int size_ );

    /**
     * Method that returns the size of the vector displacement maps
     * @return the size of the vector displacement maps
     */
    unsigned int vdmSize( void );

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
                           Eigen::Vector3f& nearestPoint_ );

    Eigen::Vector3f _matchPoint( const Facets& triangles_,
                                 const Vertices& vertices_,
                                 unsigned int borderVerticesSize_,
                                 const VertexPtr vertex_ );

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
