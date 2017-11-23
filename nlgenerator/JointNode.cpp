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
#include "JointNode.h"

namespace nlgenerator
{

  JointNode::JointNode( const Eigen::Vector3f& position_, float radius_ )
    : _position( position_ )
    , _radius( radius_ )
    , _connectedSoma( false )
  {
  }

  JointNode::~JointNode( void )
  {
    _neighbors.clear( );
  }

  Eigen::Vector3f& JointNode::position( void )
  {
    return _position;
  }

  float& JointNode::radius( void )
  {
    return _radius;
  }

  bool& JointNode::connectedSoma( void )
  {
    return _connectedSoma;
  }

  nlgeometry::SectionQuadPtr JointNode::sectionQuad( nsol::NodePtr neighbour_ )
  {
    auto neigh = _neighbors.find( neighbour_ );
    if ( neigh != _neighbors.end( ))
      return neigh->second;
    else
      return nullptr;
  }

  nlgeometry::SectionQuadPtr JointNode::sectionQuad( void )
  {
    if ( _neighbors.empty( ))
      return nullptr;
    return _neighbors.begin( )->second;
  }

  nsol::NodePtr JointNode::neighbour( void )
  {
    if ( !_neighbors.empty( ) )
      return _neighbors.begin( )->first;
    else
      return nullptr;
  }

  unsigned int JointNode::numberNeighbors( void )
  {
    return ( unsigned int )_neighbors.size( );
  }

  void JointNode::addNeighbour( nsol::NodePtr neighbour_ )
  {
    _neighbors[ neighbour_ ] = nullptr;
  }

  void JointNode::computeGeometry( void )
  {
    Eigen::Vector3f exe;
    Eigen::Vector3f exe1;
    Eigen::Vector3f tangent;
    Eigen::Quaternion< float > q;
    Eigen::Quaternion< float > qI = Eigen::Quaternion< float >::Identity( );
    Eigen::Quaternion< float > qSlerp;

    if ( _neighbors.size( ) == 1 )
    {
      auto neigh = _neighbors.begin( );
      tangent = ( neigh->first->point( ) - _position).normalized( );
      exe = Eigen::Vector3f( 0.0f, 1.0f, 0.0f );
      q.setFromTwoVectors(exe,tangent);

      auto quad = nlgeometry::SectionQuad::identity( );
      quad->rotate( q );
      quad->place( _position );
      quad->norm( _radius );
      _neighbors[ neigh->first ] = quad;
    }
    else if ( _neighbors.size( ) == 2 )
    {
      auto neigh = _neighbors.begin( );
      exe = ( _position - neigh->first->point( )  ).normalized( );
      neigh ++;
      exe1 = ( neigh->first->point( ) - _position ).normalized( );
      q.setFromTwoVectors( exe, exe1 );
      qSlerp = q.slerp( 0.5f, qI );
      tangent = qSlerp * exe ;
      exe = Eigen::Vector3f( 0.0f, 1.0f, 0.0f );
      q.setFromTwoVectors(exe,tangent);

      auto quad = nlgeometry::SectionQuad::identity( );
      quad->rotate( q );
      quad->place( _position );
      quad->norm( _radius );
      _neighbors[ _neighbors.begin( )->first ] = quad->inversed( );
      _neighbors[ neigh->first ] = quad;
    }
    else if ( _neighbors.size( ) > 2 )
    {
      Eigen::MatrixXf A( _neighbors.size( ), 3 );

      auto neigh = _neighbors.begin( );
      for ( unsigned int i = 0; i < _neighbors.size( ); i++ )
      {
        Eigen::Vector3f pos = neigh->first->point( );
        neigh ++;
        A( i, 0 ) = pos.x( );
        A( i, 1 ) = pos.y( );
        A( i, 2 ) = pos.z( );
      }

      Eigen::MatrixXf centered = A.rowwise() - A.colwise().mean();
      Eigen::MatrixXf covA = ( centered.adjoint() * centered ) /
        float( A.rows() - 1 );
      Eigen::SelfAdjointEigenSolver< Eigen::Matrix3f > eigenSolver( covA );
      Eigen::Matrix3f result = eigenSolver.eigenvectors( ).real( );
      Eigen::Vector3f normal( result( 0, 0 ), result( 1, 0 ), result( 2, 0 ));

      std::vector< std::tuple< float, Eigen::Vector3f, nsol::NodePtr >>
        orderedNodes;
      for ( neigh = _neighbors.begin( ); neigh != _neighbors.end( );
            neigh++ )
      {
        auto point = neigh->first->point( );
        auto v = point - _position;
        auto dist = v.dot( normal );
        auto projectDirection =
          ( point - dist * normal - _position ).normalized( );
        Eigen::Vector3f referenceDirection;
        if ( neigh == _neighbors.begin( ))
        {
          referenceDirection = projectDirection;
          auto tuple =
            std::make_tuple( 0.0f, projectDirection, neigh->first );
          orderedNodes.push_back( tuple );
        }
        else
        {
          Eigen::Vector3f normalAxis =
            referenceDirection.cross( projectDirection );
          float sinNormalAxis = normalAxis.norm( );
          if ( normal.dot( normalAxis ) < 0.0f )
          {
            sinNormalAxis *= -1.0f;
          }
          float angle = atan2( sinNormalAxis,
                               referenceDirection.dot( projectDirection ));
          if ( angle < 0.0f )
            angle += 2.0f * ( float )M_PI;

          auto tuple =
            std::make_tuple( angle, projectDirection, neigh->first );

          bool found = false;
          for ( auto orderedNode = orderedNodes.begin( );
                orderedNode != orderedNodes.end( );
                orderedNode++  )
          {
            if (( found = angle < std::get<0>( *orderedNode )))
            {
              orderedNodes.insert( orderedNode, tuple );
              break;
            }
          }
          if ( !found )
            orderedNodes.push_back( tuple );
        }
      }

      auto position0 = normal * _radius + _position;
      auto position2 = normal * -1.0f * _radius + _position;

      auto vertex0 = new nlgeometry::OrbitalVertex( position0, _position );
      auto vertex2 = new nlgeometry::OrbitalVertex( position2, _position );


      std::vector< nlgeometry::OrbitalVertexPtr > vertices;
      unsigned int size = ( unsigned int )orderedNodes.size( );
      for ( unsigned int id = 0; id < size; id++ )
      {
        unsigned int idPost = ( id + 1 ) % size;


        auto dir = std::get<1>( orderedNodes[id] );
        auto postDir = std::get<1>( orderedNodes[idPost] );

        Eigen::Vector3f normalAxis =
          dir.cross( postDir );
        float sinNormalAxis = normalAxis.norm( );
        if ( normal.dot( normalAxis ) < 0.0f )
          sinNormalAxis *= -1.0f;

        float angle = atan2( sinNormalAxis,
                             dir.dot( postDir ));

        if ( angle < 0.0f )
          angle = 2 * M_PI + angle;

        q = Eigen::Quaternion<float>(
          Eigen::AngleAxis<float>( angle*0.5f, normal ));
        auto halfDir = q * dir;
        auto vertex = new nlgeometry::OrbitalVertex( halfDir * _radius +
                                                     _position, _position );
        vertices.push_back( vertex );
      }

      for( unsigned int id = 0; id < orderedNodes.size( ); id++ )
      {
        int idPre = id - 1;
        if ( idPre < 0)
          idPre += size;

        auto vertex1 = vertices[idPre];
        auto vertex3 = vertices[id];
        auto quad = new nlgeometry::SectionQuad(
          vertex0, vertex1, vertex2, vertex3 );
        _neighbors[ std::get<2>( orderedNodes[id] )] = quad;
      }
    }
  }
}
