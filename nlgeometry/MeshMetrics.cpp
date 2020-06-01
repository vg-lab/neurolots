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

#include "MeshMetrics.h"
#include "SpatialHashTable.h"
#include <iostream>

#include <cstdlib>
#include <ctime>

namespace nlgeometry
{

  bool MeshMetrics::hausdorffDistance(
    float& minDistance_, float& maxDistance_, float& meanDistance_,
    float& rmsDistance_, unsigned int& samplesMesh0_,
    unsigned int& samplesMesh1_, MeshPtr mesh0_, MeshPtr mesh1_,
    unsigned int samples_ )
  {

    std::srand( std::time( nullptr ));

    if ( mesh0_->triangles( ).size( ) == 0 ||
         mesh1_->triangles( ).size( ) == 0  )
      return false;

    float min0, min1, max0, max1, mean0, mean1, rms0, rms1;

    mesh0_->computeBoundingBox( );
    mesh1_->computeBoundingBox( );
    auto aabb = mesh0_->aaBoundingBox( );
    aabb.joint( mesh1_->aaBoundingBox( ));

    _addSamples( samples_, mesh0_ );
    samplesMesh0_ = mesh0_->vertices( ).size( );
    _addSamples( samples_, mesh1_ );
    samplesMesh1_ = mesh1_->vertices( ).size( );

    float cellSize =
      (aabb.maximum( ) - aabb.minimum( )).cwiseAbs( ).maxCoeff( ) / 100.0f;
    std::vector< float > distances;
    TrianglesSpatialHashTable tsht( 1000000, cellSize );
    for ( auto triangle: mesh1_->triangles( ))
      tsht.insert( triangle );
    for ( auto vertex: mesh0_->vertices( ))
    {
      auto triangles = tsht.getNeighbors( vertex );
      distances.push_back( _distToNearestTriangle( vertex, triangles ));
    }

    min0 = _minDistance( distances );
    max0 = _maxDistance( distances );
    mean0 = _meanDistance( distances );
    rms0 = _rmsDistance( distances );

    distances.clear( );
    tsht.clear( );
    for ( auto triangle: mesh0_->triangles( ))
      tsht.insert( triangle );
    for ( auto vertex: mesh1_->vertices( ))
    {
      auto triangles = tsht.getNeighbors( vertex );
      distances.push_back( _distToNearestTriangle( vertex, triangles ));
    }

    min1 = _minDistance( distances );
    max1 = _maxDistance( distances );
    mean1 = _meanDistance( distances );
    rms1 = _rmsDistance( distances );

    minDistance_ = std::max( min0, min1 );
    maxDistance_ = std::max( max0, max1 );
    meanDistance_ = std::max( mean0, mean1 );
    rmsDistance_ = std::max( rms0, rms1 );

    return true;
  }

  float MeshMetrics::area( MeshPtr mesh_ )
  {
    float area = 0.0f;
    for ( auto facet: mesh_->triangles( ))
      area += _triangleArea( facet );
    return area;
  }

  float MeshMetrics::_triangleArea( FacetPtr facet_ )
  {
    Eigen::Vector3f pos0 = facet_->vertex0( )->position( );
    Eigen::Vector3f pos1 = facet_->vertex1( )->position( );
    Eigen::Vector3f pos2 = facet_->vertex2( )->position( );
    float area = (( pos1 - pos0 ).cross( pos2 - pos0 )).norm( ) * 0.5f;
    return area;
  }

  void MeshMetrics::_addSamples( unsigned int numSamples_, Vertices& vertices_,
                                 FacetPtr facet_ )
  {
    for ( unsigned int i = 0; i < numSamples_; i++ )
    {
      Eigen::Vector3f pos0 = facet_->vertex0( )->position( );
      Eigen::Vector3f pos1 = facet_->vertex1( )->position( );
      Eigen::Vector3f pos2 = facet_->vertex2( )->position( );

      float r1 = (( float )std::rand( ))/RAND_MAX;
      float r2 = (( float )std::rand( ))/RAND_MAX;
      float alpha = 1 - sqrt( r1 );
      float beta = sqrt( r1 ) * ( 1 - r2 );
      float gamma = 1 - alpha - beta;
      vertices_.push_back(
        new Vertex( pos0 * alpha + pos1 * beta + pos2 * gamma ));
    }
  }

  void MeshMetrics::_addSamples( unsigned int numSamples_, MeshPtr mesh_ )
  {
    float area = MeshMetrics::area( mesh_ );
    float totalSamples = (( float )numSamples_ - mesh_->vertices( ).size( ));
    if ( totalSamples < 0.0f )
      totalSamples = 0.0f;

    float samplesPerUnit = totalSamples / area;
    for ( auto facet: mesh_->triangles( ))
    {
      float tArea = _triangleArea( facet );
      unsigned int numSamples = round( tArea * samplesPerUnit );
      _addSamples( numSamples, mesh_->vertices( ), facet );
    }
  }

  Eigen::Vector3f MeshMetrics::_proyectPointOverTriangle(
      const Eigen::Vector3f& pos_, const Eigen::Vector3f& v0_,
      const Eigen::Vector3f& v1_, const Eigen::Vector3f& v2_ )
  {
    Eigen::Vector3f n = ( v1_ - v0_ ).cross( v2_ - v0_ );
    n.normalize( );
    float t = n.dot( v0_ ) - n.dot( pos_ );
    Eigen::Vector3f ppos = pos_ + t * n;
    return ppos;
  }

  Eigen::Vector3f MeshMetrics::_barycentricCoords(
    const Eigen::Vector3f& pos_, const Eigen::Vector3f& v0_,
    const Eigen::Vector3f& v1_, const Eigen::Vector3f& v2_ )
  {
    Eigen::Matrix3f A;

    A <<
      v0_.x( ), v1_.x( ), v2_.x( ),
      v0_.y( ), v1_.y( ), v2_.y( ),
      v0_.z( ), v1_.z( ), v2_.z( );

    return A.colPivHouseholderQr( ).solve( pos_ );
  }

  float MeshMetrics::_distToEdge( const Eigen::Vector3f& pos_,
                                  const Eigen::Vector3f& v0_,
                                  const Eigen::Vector3f& v1_ )
  {
    float result = (( pos_ - v0_ ).cross( pos_ - v1_ )).norm( );
    result /= ( v1_ - v0_ ).norm( );
    return result;
  }

  float MeshMetrics::_distToTriangle( const VertexPtr vertex_,
                                      const FacetPtr triangle_ )
  {
    Eigen::Vector3f pos = vertex_->position( );
    Eigen::Vector3f v0 = triangle_->vertex0( )->position( );
    Eigen::Vector3f v1 = triangle_->vertex1( )->position( );
    Eigen::Vector3f v2 = triangle_->vertex2( )->position( );

    Eigen::Vector3f ppos = _proyectPointOverTriangle( pos, v0, v1, v2 );

    Eigen::Vector3f coord = _barycentricCoords( ppos, v0, v1, v2 );

    float dist;

    if ( coord.x( ) >= 0.0f && coord.x( ) < 1.0f &&
         coord.x( ) >= 0.0f && coord.x( ) < 1.0f &&
         coord.x( ) >= 0.0f && coord.x( ) < 1.0f )
      dist = ( ppos - pos ).norm( );
    else if ( coord.x( ) >= 0.0f && coord.y( ) >= 0.0f && coord.z( ) < 0.0 )
      dist = _distToEdge( pos, v0, v1 );
    else if ( coord.x( ) < 0.0f && coord.y( ) >= 0.0f && coord.z( ) < 0.0f )
      if ( coord.z( ) <= coord.x( ))
        dist = _distToEdge( pos, v0, v1 );
      else
        dist = _distToEdge( pos, v1, v2 );
    else if ( coord.x( ) < 0.0f && coord.y( ) >= 0.0f && coord.z( ) >= 0.0f )
      dist = _distToEdge( pos, v1, v2 );
    else if ( coord.x( ) < 0.0f && coord.y( ) < 0.0f && coord.z( ) >= 0.0f )
      if ( coord.x( ) <= coord.y( ))
        dist = _distToEdge( pos, v1, v2 );
      else
        dist = _distToEdge( pos, v0, v2 );
    else if ( coord.x( ) >= 0.0f && coord.y( ) < 0.0f && coord.z( ) >= 0.0f )
      dist = _distToEdge( pos, v0, v2 );
    else if ( coord.x( ) >= 0.0f && coord.y( ) < 0.0f && coord.z( ) < 0.0f )
      if ( coord.y( ) <= coord.z( ))
        dist = _distToEdge( pos, v0, v2 );
      else
        dist = _distToEdge( pos, v0, v1 );
    else
      dist = 0.0f;

    return dist;
  }

  float MeshMetrics::_distToNearestTriangle(
    const VertexPtr vertex_, const std::unordered_set< FacetPtr >& triangles_ )
  {
    float dist = std::numeric_limits< float >::max( );

    for ( auto triangle: triangles_ )
    {
      float currentDist = _distToTriangle( vertex_, triangle );
      if ( currentDist < dist )
        dist = currentDist;
     }
    return dist;
  }

  float MeshMetrics::_minDistance( const std::vector< float >& distances_ )
  {
    float min = std::numeric_limits< float >::max( );
    for ( auto dist: distances_ )
      if ( dist < min )
        min = dist;
    return min;
  }

  float MeshMetrics::_maxDistance( const std::vector< float >& distances_ )
  {
    float max = std::numeric_limits< float >::min( );
    for ( auto dist: distances_ )
      if ( dist > max )
        max = dist;
    return max;
  }

  float MeshMetrics::_meanDistance( const std::vector< float >& distances_ )
  {
    float mean = 0.0f;
    for ( auto dist: distances_ )
      mean += dist;
    if ( distances_.size( ) > 0 )
      return mean / distances_.size( );
    else
      return mean;
  }

  float MeshMetrics::_rmsDistance( const std::vector< float >& distances_ )
  {
    float rms = 0.0f;
    for ( auto dist: distances_ )
      rms += std::pow( dist, 2 );
    return rms;
  }

}
