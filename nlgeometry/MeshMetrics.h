/**
 * Copyright (c) 2015-2019 GMRV/URJC.
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
#ifndef __NLGEOMETRY_MESH_METRICS__
#define __NLGEOMETRY_MESH_METRICS__

#include "Mesh.h"
#include "SpatialHashTable.h"

#include <unordered_set>

#include <nlgeometry/api.h>

namespace nlgeometry
{

  class MeshMetrics
  {

  public:

    NLGEOMETRY_API
    static bool hausdorffDistance(
      float& minDistance_, float& maxDistance_, float& meanDistance_,
      float& rmsDistance_, unsigned int& samplesMesh0_,
      unsigned int& samplesMesh1_, MeshPtr mesh0_, MeshPtr mesh1_,
      unsigned int samples_ = 1000 );

    NLGEOMETRY_API
    static float area( MeshPtr mesh_ );

  protected:

    static float _triangleArea( FacetPtr facet_ );

    static void _addSamples( unsigned int numSamples_, Vertices& vertices_,
                             FacetPtr facet_ );

    static void _addSamples( unsigned int numSamples_, MeshPtr mesh_ );

    static Eigen::Vector3f _proyectPointOverTriangle(
      const Eigen::Vector3f& pos_, const Eigen::Vector3f& v0_,
      const Eigen::Vector3f& v1_, const Eigen::Vector3f& v2_ );

    static Eigen::Vector3f _barycentricCoords(
      const Eigen::Vector3f& pos_, const Eigen::Vector3f& v0_,
      const Eigen::Vector3f& v1_, const Eigen::Vector3f& v2_ );

    static float _distToEdge( const Eigen::Vector3f& pos_,
                              const Eigen::Vector3f& v0_,
                              const Eigen::Vector3f& v1_ );

    static float _distToTriangle( const VertexPtr vertex_,
                                  const FacetPtr triangle_ );

    static float _distToNearestTriangle(
      const VertexPtr vertex_,
      const std::unordered_set< FacetPtr >& triangles_ );

    static float _minDistance( const std::vector< float >& distances_ );

    static float _maxDistance( const std::vector< float >& distances_ );

    static float _meanDistance( const std::vector< float >& distances_ );

    static float _rmsDistance( const std::vector< float >& distances_ );

  };

}

#endif
