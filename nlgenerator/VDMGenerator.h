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
#ifndef __NLGENERATOR_VDMGENERATOR__
#define __NLGENERATOR_VDMGENERATOR__

#include "../nlgeometry/Parametrizer.h"
#include "../nlgeometry/VDMap.h"

#include <nlgenerator/api.h>


namespace nlgenerator
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
    NLGENERATOR_API
    static VDMGenerator* Instance( void );

    /**
     * Method to compute the vector displacement map for a given mesh
     * @param mesh_ mesh to compute it vector displacement map
     * @return vector displacement map texture 2D
     */
    NLGENERATOR_API
    nlgeometry::VDMapPtr vectorDisplacementMapTexture(
      nlgeometry::MeshPtr mesh_,
      nlgeometry::Parametrizer::TVertexWeightMethod paraMethod0_ =
      nlgeometry::Parametrizer::MEAN_VALUE,
      nlgeometry::Parametrizer::TVertexWeightMethod paraMethod1_ =
      nlgeometry::Parametrizer::UNDEFINED, const float alpha0_ = 1.0f,
      const float alpha1_ = 0.5f,
      const float factor_ = 3.0f );

    /**
     * Method to set the resultant vector displacement map size
     * @param size_ size of the computed vector displacement map
     */
    NLGENERATOR_API
    void vdmapSize( unsigned int size_ );

    /**
     * Method that returns the size of the vector displacement maps
     * @return the size of the vector displacement maps
     */
    NLGENERATOR_API
    unsigned int vdmapSize( void );

    /**
     * Method that returns the cpu tessellated quad used
     * @return pointer to the mesh of the cpu tessellated quad
     */
    NLGENERATOR_API
    nlgeometry::MeshPtr cpuTessellatedQuad( void );

  private:

    VDMGenerator( void );

    void _generateTessQuad( void );

    Eigen::Vector3f _barycentricCoords( const nlgeometry::FacetPtr facet_,
                                        const nlgeometry::VertexPtr vertex_ );

    bool _edgeNearestPoint( const nlgeometry::VertexPtr edge0_,
                            const nlgeometry::VertexPtr edge1_,
                            const nlgeometry::VertexPtr vertex_,
                            Eigen::Vector3f& nearestPoint_,
                            Eigen::Vector3f& normal_,
                            const float factor_ = 5.0f );

    void _matchPoint( const nlgeometry::Facets& triangles_,
                      const nlgeometry::Vertices& vertices_,
                      unsigned int borderVerticesSize_,
                      const nlgeometry::VertexPtr vertex_,
                      Eigen::Vector3f& intersectionPoint_,
                      Eigen::Vector3f& normal_,
                      const float factor_ = 5.0f );

    Eigen::Vector3f _coordRelax( const Eigen::Vector3f coord_,
                                 const float factor_ = 5.0f );

    //! Static instance of VDMGenerator singleton class
    static VDMGenerator* _mpInstance;

    //! Pointer to a mesh containing a cpu tessellated quad
    nlgeometry::MeshPtr _cpuTessQuad;

    //! Vector of the cpu tessellated quad vertices with generation sort
    nlgeometry::Vertices _cpuTessQuadVertices;

    //! Size of the vector displacement maps computed
    unsigned int _vdmSize;

  }; // class VDMGenerator

} // namespace nlgenerator

#endif
