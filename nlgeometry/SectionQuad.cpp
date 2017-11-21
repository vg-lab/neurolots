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
#include "SectionQuad.h"

#include <nlgeometry/api.h>

namespace nlgeometry
{

  SectionQuad::SectionQuad( OrbitalVertexPtr vertex0_,
                            OrbitalVertexPtr vertex1_,
                            OrbitalVertexPtr vertex2_,
                            OrbitalVertexPtr vertex3_ )
    : _vertex0( vertex0_ )
    , _vertex1( vertex1_ )
    , _vertex2( vertex2_ )
    , _vertex3( vertex3_ )
  {

  }

  SectionQuad::~SectionQuad( void )
  {

  }

  OrbitalVertexPtr& SectionQuad::vertex0( void )
  {
    return _vertex0;
  }

  const OrbitalVertexPtr& SectionQuad::vertex0( void ) const
  {
    return _vertex0;
  }

  OrbitalVertexPtr& SectionQuad::vertex1( void )
  {
    return _vertex1;
  }

  const OrbitalVertexPtr& SectionQuad::vertex1( void ) const
  {
    return _vertex1;
  }

  OrbitalVertexPtr& SectionQuad::vertex2( void )
  {
    return _vertex2;
  }

  const OrbitalVertexPtr& SectionQuad::vertex2( void ) const
  {
    return _vertex2;
  }

  OrbitalVertexPtr& SectionQuad::vertex3( void )
  {
    return _vertex3;
  }

  const OrbitalVertexPtr& SectionQuad::vertex3( void ) const
  {
    return _vertex3;
  }

  void SectionQuad::inverse( void )
  {
    OrbitalVertexPtr vertex = _vertex1;
    _vertex1 = _vertex3;
    _vertex3 = vertex;
  }

  SectionQuadPtr SectionQuad::inversed( void ) const
  {
    return new SectionQuad( _vertex0, _vertex3, _vertex2, _vertex1 );
  }

  void SectionQuad::displace( const Eigen::Vector3f& displacement_ )
  {
    _vertex0->displace( displacement_ );
    _vertex1->displace( displacement_ );
    _vertex2->displace( displacement_ );
    _vertex3->displace( displacement_ );
  }

  void SectionQuad::place( const Eigen::Vector3f& placement_ )
  {
    _vertex0->place( placement_ );
    _vertex1->place( placement_ );
    _vertex2->place( placement_ );
    _vertex3->place( placement_ );
  }

  void SectionQuad::rotate( const Eigen::Matrix3f& rotation_ )
  {
    _vertex0->rotate( rotation_ );
    _vertex1->rotate( rotation_ );
    _vertex2->rotate( rotation_ );
    _vertex3->rotate( rotation_ );
  }

  void SectionQuad::rotate( const Eigen::Quaternion< float >& rotation_ )
  {
    _vertex0->rotate( rotation_ );
    _vertex1->rotate( rotation_ );
    _vertex2->rotate( rotation_ );
    _vertex3->rotate( rotation_ );
  }

  void SectionQuad::orient( const Eigen::Vector3f& orientation_ )
  {
    Eigen::Quaternion< float > q;
    q.setFromTwoVectors( this->normal( ), orientation_.normalized( ));
    this->rotate( q );
  }

  Eigen::Vector3f SectionQuad::axis0( void ) const
  {
    return ( _vertex0->position( ) - _vertex2->position( )).normalized( );
  }

  Eigen::Vector3f SectionQuad::axis1( void ) const
  {
    return ( _vertex1->position( ) - _vertex3->position( )).normalized( );
  }

  void SectionQuad::norm(  float norm_ )
  {
    _vertex0->norm( norm_ );
    _vertex1->norm( norm_ );
    _vertex2->norm( norm_ );
    _vertex3->norm( norm_ );
  }

  void SectionQuad::normalize( void )
  {
    Eigen::Vector3f axis0, axis1;

    axis0 = this->axis0( );
    axis1 = this->axis1( );

    _vertex0->normalize( axis0 );
    _vertex1->normalize( axis1 );
    _vertex2->normalize( -axis0 );
    _vertex3->normalize( -axis1 );
  }

  Eigen::Vector3f SectionQuad::normal( void ) const
  {
    Eigen::Vector3f axis0, axis1;
    axis0 = this->axis0( );
    axis1 = this->axis1( );
    return axis0.cross( axis1 );
  }

  SectionQuadPtr SectionQuad::clone( void ) const
  {
    OrbitalVertexPtr vertex0 =
      dynamic_cast< OrbitalVertexPtr >( _vertex0->clone( ));
    OrbitalVertexPtr vertex1 =
      dynamic_cast< OrbitalVertexPtr >( _vertex1->clone( ));
    OrbitalVertexPtr vertex2 =
      dynamic_cast< OrbitalVertexPtr >( _vertex2->clone( ));
    OrbitalVertexPtr vertex3 =
      dynamic_cast< OrbitalVertexPtr >( _vertex3->clone( ));

    return new SectionQuad( vertex0, vertex1, vertex2, vertex3 );
  }

  void SectionQuad::deleteVertices( void )
  {
    delete _vertex0;
    delete _vertex1;
    delete _vertex2;
    delete _vertex3;
  }

  Eigen::Quaternion< float > SectionQuad::getZRotation(
    SectionQuadPtr otherQuad_ )
  {
    return getZRotation( this, otherQuad_ );
  }

  float SectionQuad::getZAngle( SectionQuadPtr otherQuad_ )
  {
    return getZAngle( this, otherQuad_ );
  }

  SectionQuadPtr SectionQuad::identity( void )
  {
    OrbitalVertexPtr vertex0 =
      new OrbitalVertex( Eigen::Vector3f( 0.0f, 0.0f, -1.0f ));
    OrbitalVertexPtr vertex1 =
      new OrbitalVertex( Eigen::Vector3f( -1.0f, 0.0f, 0.0f ));
    OrbitalVertexPtr vertex2 =
      new OrbitalVertex( Eigen::Vector3f( 0.0f, 0.0f, 1.0f ));
    OrbitalVertexPtr vertex3 =
      new OrbitalVertex( Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));

      return new SectionQuad( vertex0, vertex1, vertex2, vertex3 );
  }

  void SectionQuad::createPipe( SectionQuadPtr startQuad_,
                                SectionQuadPtr endQuad_,
                                Facets& facets_, bool checkDirection_ )
  {
    std::vector< OrbitalVertexPtr > ids0( 4 );
    std::vector< OrbitalVertexPtr > ids1( 4 );

    ids0[0] = startQuad_->vertex0( ); ids0[1] = startQuad_->vertex1( );
    ids0[2] = startQuad_->vertex2( ); ids0[3] = startQuad_->vertex3( );

    ids1[0] = endQuad_->vertex0( ); ids1[1] = endQuad_->vertex1( );
    ids1[2] = endQuad_->vertex2( ); ids1[3] = endQuad_->vertex3( );

    unsigned int offset = 0;
    if ( checkDirection_ )
    {
      auto refDir =
        ( ids0[0]->position( ) - ids0[2]->position( )).normalized( );
      auto maxCos = -1.0f;
      for ( unsigned int i = 0; i < 4; i++ )
      {
        auto cos = refDir.dot(( ids1[i]->position( ) -
                                ids1[(i+2)%4]->position( )).normalized( ));
        if ( cos > maxCos )
        {
          maxCos = cos;
          offset = i;
        }
      }
    }

    for ( unsigned int i = 0; i < 4; i++ )
    {
      facets_.push_back(
        new Facet( ids0[i], ids0[(i+1)%4], ids1[(i+offset)%4],
                   ids1[(i+1+offset)%4] ));
    }
  }

  Eigen::Quaternion< float > SectionQuad::getZRotation(
    SectionQuadPtr quad0_, SectionQuadPtr quad1_ )
  {
    Eigen::Quaternion< float > q;
      auto quad0 = quad0_->clone( );
      auto quad1 = quad1_->clone( );

      quad0->normalize( );
      quad1->normalize( );

      std::vector< OrbitalVertexPtr > ids0( 4 );
      std::vector< OrbitalVertexPtr > ids1( 4 );

      ids0[0] = quad0->vertex0( ); ids0[1] = quad0->vertex1( );
      ids0[2] = quad0->vertex2( ); ids0[3] = quad0->vertex3( );

      ids1[0] = quad1->vertex0( ); ids1[1] = quad1->vertex3( );
      ids1[2] = quad1->vertex2( ); ids1[3] = quad1->vertex1( );

      Eigen::Vector3f normal0 = quad0->normal( );
      Eigen::Vector3f normal1 = quad1->normal( );
      q.setFromTwoVectors( normal0, normal1 );
      quad0->rotate( q );

      unsigned int offset = 0;
      Eigen::Vector3f refDir = ids0[0]->position( ) - ids0[0]->center( );
      Eigen::Vector3f newDir;
      refDir.normalize( );
      auto maxCos = -1.0f;
      for ( unsigned int i = 0; i < 4; i++ )
      {
        newDir = ids1[i]->position( ) - ids1[i]->center( );
        newDir.normalize( );
        auto cos =
          refDir.dot( newDir );
        if ( cos > maxCos )
        {
          maxCos = cos;
          offset = i;
        }
      }

      newDir = ids1[offset]->position( ) - ids1[offset]->center( );
      newDir.normalize( );
      q.setFromTwoVectors( refDir, newDir );

      quad0->deleteVertices( );
      quad1->deleteVertices( );
      delete quad0;
      delete quad1;
      return q;
  }

  float SectionQuad::getZAngle(
    SectionQuadPtr quad0_, SectionQuadPtr quad1_ )
  {
    Eigen::Quaternion< float > q;
    auto quad0 = quad0_->clone( );
    auto quad1 = quad1_->clone( );

    quad0->normalize( );
    quad1->normalize( );

    std::vector< OrbitalVertexPtr > ids0( 4 );
    std::vector< OrbitalVertexPtr > ids1( 4 );

    ids0[0] = quad0->vertex0( ); ids0[1] = quad0->vertex1( );
    ids0[2] = quad0->vertex2( ); ids0[3] = quad0->vertex3( );

    ids1[0] = quad1->vertex0( ); ids1[1] = quad1->vertex3( );
    ids1[2] = quad1->vertex2( ); ids1[3] = quad1->vertex1( );

    Eigen::Vector3f normal0 = quad0->normal( );
    Eigen::Vector3f normal1 = quad1->normal( );
    q.setFromTwoVectors( normal0, normal1 );
    quad0->rotate( q );

    unsigned int offset = 0;
    Eigen::Vector3f refDir = ids0[0]->position( ) - ids0[0]->center( );
    Eigen::Vector3f newDir;
    refDir.normalize( );
    auto maxCos = -1.0f;
    for ( unsigned int i = 0; i < 4; i++ )
    {
      newDir = ids1[i]->position( ) - ids1[i]->center( );
      newDir.normalize( );
      auto cos =
        refDir.dot( newDir );
      if ( cos > maxCos )
      {
        maxCos = cos;
        offset = i;
      }
    }

    newDir = ids1[offset]->position( ) - ids1[offset]->center( );
    newDir.normalize( );

    Eigen::Vector3f normalAxis =
      refDir.cross( newDir );
    float sinNormalAxis = normalAxis.norm( );
    if ( quad0->normal( ).dot( normalAxis ) < 0.0f )
    {
      sinNormalAxis *= -1.0f;
    }
    float result = atan2( sinNormalAxis, refDir.dot( newDir ));

    quad0->deleteVertices( );
    quad1->deleteVertices( );
    delete quad0;
    delete quad1;
    return result;
  }

} // namespace nlgeometry
