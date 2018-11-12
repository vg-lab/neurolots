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
#include "Parametrizer.h"

#include <iostream>
#include <stdexcept>

#include <Eigen/Sparse>

#define PI 3.1415927f

namespace nlgeometry
{

  Parametrizer* Parametrizer::_mpInstance = nullptr;

  Parametrizer* Parametrizer::Instance( )
  {
    if ( !_mpInstance )
      _mpInstance = new Parametrizer;

    return _mpInstance;
  }

  unsigned int Parametrizer::parametrize( MeshPtr mesh_,
                                          TVertexWeightMethod vwm0_,
                                          TVertexWeightMethod vwm1_,
                                          float alpha_ )
  {
    unsigned int borderVerticesSize;
    NeighborsMap neighborsMap;
    _borderSort( mesh_, borderVerticesSize );
    _borderAlignment( mesh_, borderVerticesSize );
    _uvBorderRepos( mesh_, borderVerticesSize );
    _neighbors( mesh_, neighborsMap );

    std::vector< float > weights;
    _vertexWeightsZero( mesh_, weights );

    auto function = _function( vwm0_ );
    if ( function )
    {
      function( mesh_, neighborsMap, weights );
      _normalizeWeights( weights );
    }

    function = _function( vwm1_ );
    if ( function )
    {
      std::vector< float > weights2;
      _vertexWeightsZero( mesh_, weights2 );
      function( mesh_, neighborsMap, weights2 );
      _normalizeWeights( weights2 );
      _mixWeights( weights, weights2, alpha_ );
    }

    _solve( mesh_, neighborsMap, borderVerticesSize, weights );
    return borderVerticesSize;
  }

  unsigned int Parametrizer::doublePassParametrize( MeshPtr mesh_,
                                                    TVertexWeightMethod vwm0_,
                                                    TVertexWeightMethod vwm1_,
                                                    float alpha0_,
                                                    float alpha1_ )
  {
    unsigned int borderVerticesSize;
    NeighborsMap neighborsMap;
    _borderSort( mesh_, borderVerticesSize );
    _borderAlignment( mesh_, borderVerticesSize );
    _uvBorderRepos( mesh_, borderVerticesSize );
    _neighbors( mesh_, neighborsMap );
    std::vector< float > weights0;
    std::vector< float > weights1;
    std::vector< float > weights2;

    // First pass
    auto function = _function( vwm0_ );
    if ( function )
    {
      _vertexWeightsZero( mesh_, weights0 );
      function( mesh_, neighborsMap, weights0 );
      _normalizeWeights( weights0 );
    }

    function = _function( vwm1_ );
    if ( function )
    {
      _vertexWeightsZero( mesh_, weights1 );
      function( mesh_, neighborsMap, weights1 );
      _normalizeWeights( weights1 );
      _mixWeights( weights0, weights1, alpha0_ );
    }

    //Correction weights compute
    _vertexWeightsZero( mesh_, weights2 );
    _vwmMeanValue( mesh_, neighborsMap, weights2 );
    _normalizeWeights( weights2 );
    _solve( mesh_, neighborsMap, borderVerticesSize, weights2 );

    _vertexWeightsZero( mesh_, weights2 );
    _vwmUVArea( mesh_, neighborsMap, weights2 );
    _normalizeWeights( weights2 );

    //Finall pass
    _mixWeights( weights0, weights2, alpha1_ );
    _solve( mesh_, neighborsMap, borderVerticesSize, weights0 );

    return borderVerticesSize;
  }

  void Parametrizer::alingMesh( MeshPtr mesh_ )
  {
    unsigned int borderVerticesSize;
    _borderSort( mesh_, borderVerticesSize );
    _borderAlignment( mesh_, borderVerticesSize );
  }

  Parametrizer::func Parametrizer::_function( TVertexWeightMethod vwm_ )
  {
    func function;
    switch( vwm_ )
    {
    case MEAN_VALUE:
      function = _vwmMeanValue;
      break;
    case RANDOM:
      function = _vwmRandom;
      break;
    case CURVATURE:
      function = _vwmCurvature;
      break;
    case AREA:
      function = _vwmArea;
      break;
    default:
      function = nullptr;
      break;
    }
    return function;
  }

  void Parametrizer::_vwmMeanValue( const MeshPtr /*mesh_*/,
                                    const NeighborsMap& /*neighborsMap_*/,
                                    std::vector< float >& weights_ )
  {
    for ( unsigned int i = 0; i < weights_.size( ); i++ )
    {
      weights_[i] += 1.0f;
    }
  }

  void Parametrizer::_vwmRandom( const MeshPtr /*mesh_*/,
                                 const NeighborsMap& /*neighborsMap_*/,
                                 std::vector< float >& weights_ )
  {
    for ( unsigned int i = 0; i < weights_.size( ); i++ )
    {
      float weight = rand( );
      weight /= RAND_MAX;
      weights_[i] += weight;
    }
  }

  void Parametrizer::_vwmCurvature( const MeshPtr mesh_,
                                    const NeighborsMap& /*neighborsMap_*/,
                                    std::vector< float >& weights_ )
  {
    EdgeToFacets edgeToTriangles;

    for ( auto triangle: mesh_->triangles( ))
    {
      unsigned int id0 = triangle->vertex0( )->id( );
      unsigned int id1 = triangle->vertex1( )->id( );
      unsigned int id2 = triangle->vertex2( )->id( );

      Edge edge0 = _makeEdge( id0, id1 );
      Edge edge1 = _makeEdge( id1, id2 );
      Edge edge2 = _makeEdge( id2, id0 );

      _addTriangleToEdge( edgeToTriangles, edge0, triangle );
      _addTriangleToEdge( edgeToTriangles, edge1, triangle );
      _addTriangleToEdge( edgeToTriangles, edge2, triangle );
    }

    for ( auto edge: edgeToTriangles )
    {
      unsigned int id0 = edge.first.first;
      unsigned int id1 = edge.first.second;
      float weight = 2.0f;
      if ( edge.second.size( ) != 2 )
      {
        weight = 2.0f;
      }
      else
      {
        float e =( mesh_->vertices( )[id0]->position( )
                   - mesh_->vertices( )[id1]->position( )).norm( );
        weight = 1.0f + _trianglesCos( edge.second[0], edge.second[1] );
        weight *= e;
      }
      weights_[id0] += weight;
      weights_[id1] += weight;
    }
  }

  void Parametrizer::_vwmArea( const MeshPtr mesh_,
                               const NeighborsMap& /*neighborsMap_*/,
                               std::vector< float >& weights_ )
  {
    for ( auto triangle: mesh_->triangles( ))
    {
      auto vertex0 = triangle->vertex0( );
      auto vertex1 = triangle->vertex1( );
      auto vertex2 = triangle->vertex2( );
      unsigned int id0 = vertex0->id( );
      unsigned int id1 = vertex1->id( );
      unsigned int id2 = vertex2->id( );

      Eigen::Vector3f axis0 = vertex1->position( ) - vertex0->position( );
      Eigen::Vector3f axis1 = vertex2->position( ) - vertex0->position( );
      float weight = axis0.cross( axis1 ).norm( ) * 0.5f;
      weight = 1 / weight;
      weights_[id0] += weight;
      weights_[id1] += weight;
      weights_[id2] += weight;
    }
  }

  void Parametrizer::_vwmUVArea( const MeshPtr mesh_,
                                 const NeighborsMap& // neighborsMap_
                                 ,
                                 std::vector< float >& weights_ )
  {
    std::vector< float > triangles( weights_.size( ));
    for ( auto triangle: mesh_->triangles( ))
    {
      auto vertex0 = triangle->vertex0( );
      auto vertex1 = triangle->vertex1( );
      auto vertex2 = triangle->vertex2( );
      unsigned int id0 = vertex0->id( );
      unsigned int id1 = vertex1->id( );
      unsigned int id2 = vertex2->id( );

      Eigen::Vector3f axis0( 0.0f, 0.0f, 0.0f );
      Eigen::Vector3f axis1( 0.0f, 0.0f, 0.0f );
      axis0.block< 2, 1 >( 0, 0 ) = vertex1->uv( ) - vertex0->uv( );
      axis1.block< 2, 1 >( 0, 0 ) = vertex2->uv( ) - vertex0->uv( );
      float paramArea = axis0.cross( axis1 ).norm( ) * 0.5f;

      // axis0 = vertex1->position( ) - vertex0->position( );
      // axis1 = vertex2->position( ) - vertex0->position( );
      // float meshArea = axis0.cross( axis1 ).norm( ) * 0.5f;

      // float weight = paramArea/meshArea;
      float weight = paramArea;
      weights_[id0] += weight;
      triangles[id0] += 1.0f;
      weights_[id1] += weight;
      triangles[id1] += 1.0f;
      weights_[id2] += weight;
      triangles[id2] += 1.0f;
    }
    for ( unsigned int i = 0; i < weights_.size( ); i++ )
    {
      weights_[i] /= triangles[i];
    }
  }

  void Parametrizer::_vertexWeightsZero( const MeshPtr mesh_,
                                         std::vector< float >& weights_ )
  {
    weights_.clear( );
    for ( unsigned int i = 0; i < mesh_->vertices( ).size( ); i++ )
    {
      weights_.push_back( 0.0f );
    }
  }

  void Parametrizer::_normalizeWeights( std::vector< float >& weights_  )
  {
    float sumValue = 0.0f;
    for ( unsigned int i = 0; i < weights_.size( ); i++ )
      sumValue += weights_[i];
    float factor = 1.0f / sumValue;
    for ( unsigned int i = 0; i < weights_.size( ); i++ )
      weights_[i] *= factor;
  }

  void Parametrizer::_mixWeights( std::vector< float >& weights0_,
                                  std::vector< float >& weights1_,
                                  float alpha_ )
  {
    for ( unsigned int i = 0; i < weights0_.size( ); i++ )
    {
      weights0_[i] = weights0_[i] * alpha_ + weights1_[i] * ( 1 - alpha_ );
    }
  }

  void Parametrizer::_solve( MeshPtr mesh_, const NeighborsMap& neighborsMap_,
                             unsigned int borderVerticesSize_,
                             const std::vector< float >& weights_ )
  {
    Vertices& vertices = mesh_->vertices( );
    unsigned int coeffSize = 0;
    unsigned int systemDim =
      (unsigned int)vertices.size( ) - borderVerticesSize_;
    for ( auto neighbors: neighborsMap_ )
      coeffSize += (unsigned int)neighbors.second.size( ) + 1;

    Eigen::SparseMatrix< float > system( systemDim, systemDim );
    system.reserve( coeffSize );
    Eigen::VectorXf us = Eigen::VectorXf::Zero( systemDim );
    Eigen::VectorXf vs = Eigen::VectorXf::Zero( systemDim );
    std::vector< Eigen::Triplet< float >> coefficients;

    for ( unsigned int i = borderVerticesSize_; i < vertices.size( ); i++ )
    {
      int systemId = i - borderVerticesSize_;
      auto vertex = vertices[i];
      auto neighborsIt = neighborsMap_.find( vertex );
      if ( neighborsIt != neighborsMap_.end( ))
      {
        auto neighbors = neighborsIt->second;
        float rowWeight = 0.0f;
        for ( auto neighbour: neighbors )
        {
          int neighbourId = neighbour->id( ) - borderVerticesSize_;
          float weight = weights_[neighbour->id( )];
          rowWeight += weight;
          if ( neighbourId >= 0 )
          {
            coefficients.push_back( Eigen::Triplet< float >(
                                      systemId, neighbourId, -weight ));
          }
          else
          {
            auto uv = neighbour->uv( ) * weight;
            us( systemId ) += uv.x( );
            vs( systemId ) += uv.y( );
          }
        }
        coefficients.push_back( Eigen::Triplet< float >(
                                  systemId, systemId, rowWeight ));
      }
      else
      {
        coefficients.push_back( Eigen::Triplet< float >(
                                  systemId, systemId, 1.0f ));
      }
    }

    system.setFromTriplets( coefficients.begin( ), coefficients.end( ));

    Eigen::SparseLU< Eigen::SparseMatrix< float >> solver;
    solver.compute( system );

    Eigen::VectorXf solutionUs = solver.solve( us );
    Eigen::VectorXf solutionVs = solver.solve( vs );

    for ( unsigned int i = 0; i < solutionUs.size( ); i++ )
    {
      vertices[i+borderVerticesSize_]->uv( ) =
        Eigen::Vector2f( solutionUs[i], solutionVs[i] );
    }
  }

  void Parametrizer::_neighbors( const MeshPtr mesh_,
                                 NeighborsMap& neighborsMap_ )
  {
    for ( auto triangle: mesh_->triangles( ))
    {
      auto vertex0 = triangle->vertex0( );
      auto vertex1 = triangle->vertex1( );
      auto vertex2 = triangle->vertex2( );

      auto mapIt = neighborsMap_.find( vertex0 );
      if ( mapIt != neighborsMap_.end( ))
      {
        mapIt->second.insert( vertex1 );
        mapIt->second.insert( vertex2 );
      }
      else
      {
        auto vertexNeighbors = new VertexNeighbors( );
        vertexNeighbors->insert( vertex1 );
        vertexNeighbors->insert( vertex2 );
        neighborsMap_[ vertex0 ] = *vertexNeighbors;
      }

      mapIt = neighborsMap_.find( vertex1 );
      if ( mapIt != neighborsMap_.end( ))
      {
        mapIt->second.insert( vertex0 );
        mapIt->second.insert( vertex2 );
      }
      else
      {
        auto vertexNeighbors = new VertexNeighbors( );
        vertexNeighbors->insert( vertex0 );
        vertexNeighbors->insert( vertex2 );
        neighborsMap_[ vertex1 ] = *vertexNeighbors;
      }

      mapIt = neighborsMap_.find( vertex2 );
      if ( mapIt != neighborsMap_.end( ))
      {
        mapIt->second.insert( vertex0 );
        mapIt->second.insert( vertex1 );
      }
      else
      {
        auto vertexNeighbors = new VertexNeighbors( );
        vertexNeighbors->insert( vertex0 );
        vertexNeighbors->insert( vertex1 );
        neighborsMap_[ vertex2 ] = *vertexNeighbors;
      }
    }
  }

  void Parametrizer::_borderSort( MeshPtr mesh_,
                                 unsigned int& borderVerticesSize_ )
  {
    Vertices& vertices = mesh_->vertices( );
    Facets& triangles = mesh_->triangles( );
    for ( unsigned int i = 0; i < vertices.size( ); i++ )
    {
      vertices[i]->id( ) = i;
    }

    Edges edges;
    for ( auto triangle: triangles )
    {
      unsigned int index0 = triangle->vertex0( )->id( );
      unsigned int index1 = triangle->vertex1( )->id( );
      unsigned int index2 = triangle->vertex2( )->id( );

      _addOrDeleteEdge( index0, index1, edges );
      _addOrDeleteEdge( index1, index2, edges );
      _addOrDeleteEdge( index2, index0, edges );
    }

    if ( edges.size( ) == 0 )
      throw std::runtime_error(
        "Error: Parametrization can not be applied over non hole mesh." );

    Vertices sortedVertices;
    Edges::iterator previousEdge = ( edges.begin( ));
    edges.erase( edges.begin( ));
    sortedVertices.push_back( vertices[previousEdge->second] );

    while( !edges.empty( ))
    {
      bool nextFound = false;
      for ( Edges::iterator it = edges.begin( ); it != edges.end( ); it++ )
      {
        if (( nextFound = ( it->first == previousEdge->second )))
        {
          previousEdge = it;
          sortedVertices.push_back( vertices[previousEdge->second] );
          edges.erase( previousEdge );
          break;
        }
      }
      if ( !nextFound )
        throw std::runtime_error( "Error: Parametrization can not be applied"
                                  " over mesh with more than 1 hole." );
    }
    borderVerticesSize_ = (unsigned int)sortedVertices.size( );

    for ( auto vertex: vertices )
    {
      if ( !_vertexInVertices( vertex, sortedVertices ))
        sortedVertices.push_back( vertex );
    }
    for ( unsigned int i = 0; i < sortedVertices.size( ); i++ )
    {
      sortedVertices[i]->id( ) = i;
    }

    vertices.clear( );
    vertices = sortedVertices;
  }

  void Parametrizer::_borderAlignment( MeshPtr mesh_,
                                      const unsigned int borderVerticesSize_ )
  {
    Vertices& vertices = mesh_->vertices( );

    Eigen::Vector3f borderCenter = Eigen::Vector3f( 0.0f, 0.0f, 0.0f );
    Eigen::Vector3f alignAxis( 0.0f, 0.0f, 0.0f );

    for ( unsigned int i = 0; i < borderVerticesSize_; i++ )
    {
      borderCenter += vertices[i]->position( );
    }
    borderCenter /= borderVerticesSize_;

    for ( unsigned int i = 0; i < borderVerticesSize_; i++ )
    {
      Eigen::Vector3f v0 = vertices[i]->position( ) - borderCenter;
      Eigen::Vector3f v1 =
        vertices[(i+1)%borderVerticesSize_]->position( ) - borderCenter;
      Eigen::Vector3f axis = v0.cross( v1 );
      alignAxis += axis;
    }
    alignAxis.normalize( );

    Eigen::Quaternion< float > q;
    q.setFromTwoVectors( Eigen::Vector3f( 0.0f, 0.0f, 1.0f ), alignAxis );
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity( );
    model.block< 3, 3 >( 0, 0 ) = q.toRotationMatrix( );
    model.block< 3, 1 >( 0, 3 ) = borderCenter;
    Eigen::Matrix4f inverseModel = model.inverse( );

    for( unsigned int i = 0; i < vertices.size( ); i++ )
    {
      Eigen::Vector4f position;
      position.block<3,1>( 0, 0 ) = vertices[i]->position( );
      position.w( ) = 1.0f;
      position = inverseModel * position;
      vertices[i]->position( ) = position.block<3,1>( 0, 0 );
    }

    mesh_->modelMatrix( ) = model;
  }

  void Parametrizer::_uvBorderRepos( MeshPtr mesh_,
                                    const unsigned int borderVerticesSize_ )
  {
    Vertices& vertices = mesh_->vertices( );
    float borderLength = 0.0f;
    for ( unsigned int i = 0; i < borderVerticesSize_; i++ )
    {
      borderLength +=
        ( vertices[i]->position( ) -
          vertices[(i+1)%borderVerticesSize_]->position( )).norm( );
    }
    float factor = 2 * PI / borderLength;
    float currentLength = 0.0f;
    for ( unsigned int i = 0; i < borderVerticesSize_; i++ )
    {
      float currentAngle = currentLength * factor;
      Eigen::Vector2f uv( cos( currentAngle ), sin( currentAngle ));
      vertices[i]->uv( ) = uv;
      currentLength +=
        ( vertices[i]->position( ) -
          vertices[(i+1)%borderVerticesSize_]->position( )).norm( );
    }
    for ( unsigned int i = borderVerticesSize_; i < vertices.size( ); i++ )
    {
      vertices[i]->uv( ) = Eigen::Vector2f( 0.0f, 0.0f );
    }
  }

  void Parametrizer::_addOrDeleteEdge( unsigned int edgeStart_,
                                      unsigned int edgeEnd_, Edges& edges_ )
  {
    auto inverseEdge = std::make_pair( edgeEnd_, edgeStart_ );
    auto edgesIt = edges_.find( inverseEdge );

    if ( edgesIt != edges_.end( ))
    {
      edges_.erase( edgesIt );
    }
    else
    {
      auto edge = std::make_pair( edgeStart_, edgeEnd_ );
      edges_.insert( edge );
    }
  }

  bool Parametrizer::_vertexInVertices( const VertexPtr vertex_,
                                       const Vertices& vertices_ )
  {
    for ( auto vertex: vertices_ )
    {
      if ( vertex_ == vertex )
        return true;
    }
    return false;
  }

  Edge Parametrizer::_makeEdge( unsigned int id0_, unsigned int id1_ )
  {
    if ( id0_ > id1_ )
    {
      unsigned int id = id0_;
      id0_ = id1_;
      id1_ = id;
    }
    return Edge( id0_, id1_ );
  }

  void Parametrizer::_addTriangleToEdge( EdgeToFacets& edgeToTriangles_,
                                         const Edge& edge_,
                                         const FacetPtr triangle_ )
  {
    auto it = edgeToTriangles_.find( edge_ );
    if ( it != edgeToTriangles_.end( ))
    {
      it->second.push_back( triangle_ );
    }
    else
    {
      auto triangles = new Facets( );
      triangles->push_back( triangle_ );
      edgeToTriangles_[edge_] = *triangles;
    }
  }

  float Parametrizer::_trianglesCos( FacetPtr triangle0_, FacetPtr triangle1_ )
  {
    Eigen::Vector3f axis00 =
      ( triangle0_->vertex1( )->position( ) -
        triangle0_->vertex0( )->position( )).normalized( );
    Eigen::Vector3f axis01 =
      ( triangle0_->vertex2( )->position( ) -
        triangle0_->vertex0( )->position( )).normalized( );
    Eigen::Vector3f axis0 = axis00.cross( axis01 );

    Eigen::Vector3f axis10 =
      ( triangle1_->vertex1( )->position( ) -
        triangle1_->vertex0( )->position( )).normalized( );
    Eigen::Vector3f axis11 =
      ( triangle1_->vertex2( )->position( ) -
        triangle1_->vertex0( )->position( )).normalized( );
    Eigen::Vector3f axis1 = axis10.cross( axis11 );

    return axis0.dot( axis1 );

  }
} // end nlgeometry
