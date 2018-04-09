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
#ifndef __NLGEOMETRY_PARAMETRIZER__
#define __NLGEOMETRY_PARAMETRIZER__

#include "Mesh.h"

#include <unordered_set>
#include <unordered_map>

#include <nlgeometry/api.h>

namespace nlgeometry
{

  typedef std::pair< unsigned int, unsigned int > Edge;
  struct edge_hash {
    inline std::size_t operator( )(const Edge& edge) const {
      unsigned int k1 = edge.first;
      unsigned int k2 = edge.second;
      return ( k1 + k2 ) * ( k1 + k2 + 1 ) * 0.5f + k2;
    }
  };

  typedef std::unordered_set< Edge, edge_hash > Edges;
  typedef std::unordered_map< Edge, Facets, edge_hash >
    EdgeToFacets;
  typedef std::unordered_set< VertexPtr > VertexNeighbors;
  typedef std::unordered_map< VertexPtr, VertexNeighbors > NeighborsMap;

  /*! \class Parametrizer
    \brief A singleton class to parametrize meshes through their uv coordinates
  */
  class Parametrizer
  {

  public:

    //! Methods to add weights to the vertex
    typedef enum
    {
      UNDEFINED = 0,
      MEAN_VALUE,
      RANDOM,
      CURVATURE,
      AREA
    } TVertexWeightMethod;

    /**
     * Returns the instance of the singleton class
     * @return the Parametrizer instance
     */
    NLGEOMETRY_API
    static Parametrizer* Instance( void );

    /**
     * Method to parametrize a mesh
     * @param mesh_ mesh to parametrize
     * @param vwm0_ first method to compute the vertex weight
     * @param vwm1_ second method to compute the vertex weight
     * @param alpha_ coefficient to weigh between the to previous methods
     * @return number of mesh border vertices
     */
    NLGEOMETRY_API
    unsigned int  parametrize( MeshPtr mesh_,
                               TVertexWeightMethod vwm0_ = MEAN_VALUE,
                               TVertexWeightMethod vwm1_ = UNDEFINED,
                               const float alpha_ = 1.0f );

    NLGEOMETRY_API
    unsigned int  doublePassParametrize( MeshPtr mesh_,
                                         TVertexWeightMethod vwm0_ = MEAN_VALUE,
                                         TVertexWeightMethod vwm1_ = UNDEFINED,
                                         const float alpha0_ = 1.0f,
                                         const float alpha1_ = 1.0f );


    /**
     * Method to aling a mesh
     * @param mesh_ mesh to aling
     */
    NLGEOMETRY_API
    void alingMesh( MeshPtr mesh_ );

  private:

    Parametrizer( void ){ };

    typedef void (*func)( const MeshPtr, const NeighborsMap&,
                          std::vector< float >& );

    func _function( Parametrizer::TVertexWeightMethod vwm_ );

    static void _vwmMeanValue( const MeshPtr mesh_,
                               const NeighborsMap& neighborsMap_,
                               std::vector< float >& weights_ );

    static void _vwmRandom( const MeshPtr mesh_,
                            const NeighborsMap& neighborsMap_,
                            std::vector< float >& weights_ );

    static void _vwmCurvature( const MeshPtr mesh_,
                               const NeighborsMap& neighborsMap_,
                               std::vector< float >& weights_ );

    static void _vwmArea( const MeshPtr mesh_,
                          const NeighborsMap& neighborsMap_,
                          std::vector< float >& weights_ );

    static void _vwmUVArea( const MeshPtr mesh_,
                            const NeighborsMap& neighborsMap_,
                            std::vector< float >& weights_ );

    void _vertexWeightsZero( const MeshPtr mesh_,
                             std::vector< float >& weights_ );

    void _normalizeWeights( std::vector< float >& weights_ );

    void _mixWeights( std::vector< float >& weights0_,
                      std::vector< float >& weoghts1_,
                      const float alpha_ );

    void _solve( MeshPtr mesh_, const NeighborsMap& neighborsMap_,
                 unsigned int borderVerticesSize,
                 const std::vector< float >& weights_ );

    void _neighbors( const MeshPtr mesh_, NeighborsMap& neighborsMap_ );

    void _borderSort( MeshPtr mesh_, unsigned int& borderVerticesSize_ );

    void _borderAlignment( MeshPtr mesh_,
                          const unsigned int borderVerticesSize_ );

    void _uvBorderRepos( MeshPtr mesh,
                        const unsigned int borderVerticesSize_ );

    void _addOrDeleteEdge( unsigned int edgeStart_, unsigned int edgeEnd_,
                          Edges& edges_ );

    bool _vertexInVertices( const VertexPtr vertex_,
                            const Vertices& vertices_ );

    static Edge _makeEdge( unsigned int id0_, unsigned int id1_ );

    static void _addTriangleToEdge( EdgeToFacets& edgeToTriangles_,
                             const Edge& edge_, const FacetPtr triangle_ );

    static float _trianglesCos( FacetPtr triangle0_, FacetPtr triangle1_ );

    //! Static instance of Parametrizer singleton class
    static Parametrizer* _mpInstance;

  }; // class Parametrizer

} // namespace nlgeometry

#endif
