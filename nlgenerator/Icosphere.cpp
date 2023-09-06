/**
 * Copyright (c) 2015-2017 VG-Lab/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/vg-lab/neurolots>
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
#include "Icosphere.h"

bool operator==( const MyPair& lhs, const MyPair& rhs )
{
  return lhs.first_int == rhs.first_int && lhs.second_int == rhs.second_int;
}

namespace nlgenerator
{

  Icosphere::Icosphere(  const Eigen::Vector3f& center_, float radius_,
                         unsigned int subdivisionlevel_ )
    : _center( center_ )
    , _radius( radius_ )
  {

    Eigen::Vector3f position = Eigen::Vector3f( _center );
    nlphysics::NodePtr n0 = new nlphysics::Node( position, 0 );

    position = _center  + Eigen::Vector3f( 0.0f, _radius, 0.0f );
    nlphysics::NodePtr n1 = new nlphysics::Node( position , 1, true );

    position = _center + Eigen::Vector3f( 0.0f, -_radius, 0.0f );
    nlphysics::NodePtr n2 = new nlphysics::Node( position, 2, true );

    position = _center + Eigen::Vector3f( _radius, 0.0f, 0.0f );
    nlphysics::NodePtr n3 = new nlphysics::Node( position, 3, true );

    position = _center + Eigen::Vector3f( -_radius, 0.0f, 0.0f );
    nlphysics::NodePtr n4 = new nlphysics::Node( position, 4, true );

    position = _center + Eigen::Vector3f( 0.0f, 0.0f, _radius );
    nlphysics::NodePtr n5 = new nlphysics::Node( position, 5, true );

    position = _center + Eigen::Vector3f( 0.0f, 0.0f, -_radius );
    nlphysics::NodePtr n6 = new nlphysics::Node( position, 6, true );

    _nodes.push_back( n0 );
    _nodes.push_back( n1 );
    _nodes.push_back( n2 );
    _nodes.push_back( n3 );
    _nodes.push_back( n4 );
    _nodes.push_back( n5 );
    _nodes.push_back( n6 );

    _surfaceNodes.push_back( n1 );
    _surfaceNodes.push_back( n2 );
    _surfaceNodes.push_back( n3 );
    _surfaceNodes.push_back( n4 );
    _surfaceNodes.push_back( n5 );
    _surfaceNodes.push_back( n6 );

    _tetrahedra.push_back( new nlphysics::Tetrahedron( n0, n1, n3, n5 ));
    _tetrahedra.push_back( new nlphysics::Tetrahedron( n0, n1, n5, n4 ));
    _tetrahedra.push_back( new nlphysics::Tetrahedron( n0, n5, n3, n2 ));
    _tetrahedra.push_back( new nlphysics::Tetrahedron( n0, n5, n2, n4 ));

    _tetrahedra.push_back( new nlphysics::Tetrahedron( n0, n1, n4, n6 ));
    _tetrahedra.push_back( new nlphysics::Tetrahedron( n0, n1, n6, n3 ));
    _tetrahedra.push_back( new nlphysics::Tetrahedron( n0, n6, n4, n2 ));
    _tetrahedra.push_back( new nlphysics::Tetrahedron( n0, n6, n2, n3 ));

    _surfaceQuads.push_back( new Quad( n4, n5, n3, n1 ));
    _surfaceQuads.push_back( new Quad( n4, n2, n3, n5 ));
    _surfaceQuads.push_back( new Quad( n3, n6, n4, n1 ));
    _surfaceQuads.push_back( new Quad( n3, n2, n4, n6 ));

    _subdivideSphere( subdivisionlevel_ );
  }

  Icosphere::~Icosphere( void )
  {

  }

  nlgeometry::Facets Icosphere::compute(
    const std::vector< JointNodePtr >& joints_  )
  {
    nlgeometry::Facets facets;
    std::unordered_map< unsigned int, nlgeometry::OrbitalVertexPtr > vertices;

    std::vector< std::vector< nlphysics::NodePtr >> jointsNodes;
    for( auto joint: joints_ )
    {
      Eigen::Vector3f surfacePoint =
        ( joint->position( ) - _center ).normalized( ) * _radius + _center;
      auto quad = _nearestSurfaceQuad( surfacePoint );
      Eigen::Vector3f quadCenter =
        ( quad->node0( )->initialPosition( ) +
          quad->node1( )->initialPosition( ) +
          quad->node2( )->initialPosition( ) +
          quad->node3( )->initialPosition( )) * 0.25f;

      auto sectionQuad = joint->sectionQuad( );
      auto node = quad->node0( );
      node->position( ) = ( node->initialPosition( ) - quadCenter
        ).normalized( ) * joint->radius( ) + joint->position( );
      node->center( ) = joint->position( );
      node->fixed( ) = true;
      sectionQuad->vertex0( ) = _nodeToVertex( node, vertices );

      node = quad->node1( );
      node->position( ) = ( node->initialPosition( ) - quadCenter
        ).normalized( ) * joint->radius( ) + joint->position( );
      node->center( ) = joint->position( );
      node->fixed( ) = true;
      sectionQuad->vertex1( ) = _nodeToVertex( node, vertices );

      node = quad->node2( );
      node->position( ) = ( node->initialPosition( ) - quadCenter
        ).normalized( ) * joint->radius( ) + joint->position( );
      node->center( ) = joint->position( );
      node->fixed( ) = true;
      sectionQuad->vertex2( ) = _nodeToVertex( node, vertices );

      node = quad->node3( );
      node->position( ) = ( node->initialPosition( ) - quadCenter
        ).normalized( ) * joint->radius( ) + joint->position( );
      node->center( ) = joint->position( );
      node->fixed( ) = true;
      sectionQuad->vertex3( ) = _nodeToVertex( node, vertices );
    }

    _femSystem = new nlphysics::Fem( _nodes, _tetrahedra, 0.3f, 1.0f );
    _femSystem->solve( );
    _computeCenters( );
    _surface( facets, vertices );
    return facets;
  }

  nlgeometry::Facets Icosphere::surface( void )
  {
    nlgeometry::Facets facets;
    std::unordered_map< unsigned int, nlgeometry::OrbitalVertexPtr > vertices;
    _surface( facets, vertices );
    return facets;
  }

  nlgeometry::Facets Icosphere::structure( void )
  {
    nlgeometry::Facets facets;
    std::unordered_map< unsigned int, nlgeometry::OrbitalVertexPtr > vertices;
    for ( auto tetrahedron: _tetrahedra )
    {
      auto vertex0 = _nodeToVertex( tetrahedron->node0( ), vertices );
      auto vertex1 = _nodeToVertex( tetrahedron->node2( ), vertices );
      auto vertex2 = _nodeToVertex( tetrahedron->node1( ), vertices );
      facets.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));

      vertex0 = _nodeToVertex( tetrahedron->node0( ), vertices );
      vertex1 = _nodeToVertex( tetrahedron->node3( ), vertices );
      vertex2 = _nodeToVertex( tetrahedron->node2( ), vertices );
      facets.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));

      vertex0 = _nodeToVertex( tetrahedron->node0( ), vertices );
      vertex1 = _nodeToVertex( tetrahedron->node1( ), vertices );
      vertex2 = _nodeToVertex( tetrahedron->node3( ), vertices );
      facets.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));

      vertex0 = _nodeToVertex( tetrahedron->node1( ), vertices );
      vertex1 = _nodeToVertex( tetrahedron->node2( ), vertices );
      vertex2 = _nodeToVertex( tetrahedron->node3( ), vertices );
      facets.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));
    }
    return facets;
  }

  void Icosphere::_surface( nlgeometry::Facets& facets_,
      std::unordered_map< unsigned int, nlgeometry::OrbitalVertexPtr >&
      vertices_ )
  {
    for ( auto tetrahedron: _surfaceTetrahedra )
    {
      if ( tetrahedron->face0( ) && !( tetrahedron->node0( )->fixed( ) &&
                                       tetrahedron->node1( )->fixed( ) &&
                                       tetrahedron->node2( )->fixed( )))
      {
        auto vertex0 = _nodeToVertex( tetrahedron->node0( ), vertices_ );
        auto vertex1 = _nodeToVertex( tetrahedron->node1( ), vertices_ );
        auto vertex2 = _nodeToVertex( tetrahedron->node2( ), vertices_ );
        facets_.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));
      }
      if ( tetrahedron->face1( ) && !( tetrahedron->node0( )->fixed( ) &&
                                       tetrahedron->node2( )->fixed( ) &&
                                       tetrahedron->node3( )->fixed( )))
      {
        auto vertex0 = _nodeToVertex( tetrahedron->node0( ), vertices_ );
        auto vertex1 = _nodeToVertex( tetrahedron->node2( ), vertices_ );
        auto vertex2 = _nodeToVertex( tetrahedron->node3( ), vertices_ );
        facets_.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));
      }
      if ( tetrahedron->face2( ) && !( tetrahedron->node0( )->fixed( ) &&
                                       tetrahedron->node3( )->fixed( ) &&
                                       tetrahedron->node1( )->fixed( )))
      {
        auto vertex0 = _nodeToVertex( tetrahedron->node0( ), vertices_ );
        auto vertex1 = _nodeToVertex( tetrahedron->node3( ), vertices_ );
        auto vertex2 = _nodeToVertex( tetrahedron->node1( ), vertices_ );
        facets_.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));
      }
      if ( tetrahedron->face3( ) && !( tetrahedron->node1( )->fixed( ) &&
                                       tetrahedron->node3( )->fixed( ) &&
                                       tetrahedron->node2( )->fixed( )))
      {
        auto vertex0 = _nodeToVertex( tetrahedron->node1( ), vertices_ );
        auto vertex1 = _nodeToVertex( tetrahedron->node3( ), vertices_ );
        auto vertex2 = _nodeToVertex( tetrahedron->node2( ), vertices_ );
        facets_.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));
      }
    }
  }

  nlphysics::NodePtr Icosphere::_nearestSurfaceNode(
    const Eigen::Vector3f& point_ ) const
  {
    nlphysics::NodePtr nearestNode = nullptr;
    float minimumDistance = std::numeric_limits< float >::max( );

    for( auto node: _surfaceNodes )
    {
      if ( node->fixed( ))
        continue;
      float distance = ( node->initialPosition( ) - point_ ).norm( );
      if ( distance < minimumDistance )
      {
        minimumDistance = distance;
        nearestNode = node;
      }
    }
    return nearestNode;
  }

  QuadPtr Icosphere::_nearestSurfaceQuad( const Eigen::Vector3f& point_ ) const
  {
    QuadPtr nearestQuad = nullptr;
    float minimumDistance = std::numeric_limits< float >::max( );

    for( auto quad: _surfaceQuads )
    {
      if ( quad->node0( )->fixed( ) || quad->node1( )->fixed( ) ||
           quad->node2( )->fixed( ) || quad->node3( )->fixed( ) )
        continue;
      float distance = (( quad->node0( )->initialPosition( ) +
                         quad->node1( )->initialPosition( ) +
                         quad->node2( )->initialPosition( ) +
                          quad->node3( )->initialPosition( )) * 0.25f -
                        point_ ).norm( );
      if ( distance < minimumDistance )
      {
        minimumDistance = distance;
        nearestQuad = quad;
      }
    }
    return nearestQuad;
  }

  void Icosphere::_subdivideSphere( unsigned int subdivisionlevel_ )
  {

    std::unordered_map< MyPair, nlphysics::NodePtr,
                        std::hash<MyPair> > newNodes;


    for ( unsigned int level = 0; level < subdivisionlevel_; level++ )
    {
      newNodes.clear( );
      nlphysics::Tetrahedra newTetrahedra;
      for ( auto tet: _tetrahedra )
      {
        nlphysics::NodePtr nodeA = tet->node0( );
        nlphysics::NodePtr nodeB = tet->node1( );
        nlphysics::NodePtr nodeC = tet->node2( );
        nlphysics::NodePtr nodeD = tet->node3( );

        nlphysics::NodePtr nodeE = _middleNode( nodeA, nodeB, newNodes );
        nlphysics::NodePtr nodeF = _middleNode( nodeA, nodeC, newNodes );
        nlphysics::NodePtr nodeG = _middleNode( nodeA, nodeD, newNodes );
        nlphysics::NodePtr nodeH = _middleNode( nodeB, nodeC, newNodes );
        nlphysics::NodePtr nodeI = _middleNode( nodeC, nodeD, newNodes );
        nlphysics::NodePtr nodeJ = _middleNode( nodeD, nodeB, newNodes );

        newTetrahedra.push_back(
          new nlphysics::Tetrahedron( nodeA, nodeE, nodeF, nodeG ));
        newTetrahedra.push_back(
          new nlphysics::Tetrahedron( nodeB, nodeH, nodeE, nodeJ ));
        newTetrahedra.push_back(
          new nlphysics::Tetrahedron( nodeC, nodeI, nodeF, nodeH ));
        newTetrahedra.push_back(
          new nlphysics::Tetrahedron( nodeD, nodeJ, nodeG, nodeI ));

        newTetrahedra.push_back(
          new nlphysics::Tetrahedron( nodeE, nodeF, nodeJ, nodeH ));
        newTetrahedra.push_back(
          new nlphysics::Tetrahedron( nodeI, nodeF, nodeH, nodeJ ));
        newTetrahedra.push_back(
          new nlphysics::Tetrahedron( nodeE, nodeF, nodeG, nodeJ ));
        newTetrahedra.push_back(
          new nlphysics::Tetrahedron( nodeI, nodeF, nodeJ, nodeG ));
        delete tet;
      }
      _tetrahedra.clear(  );
      _tetrahedra = newTetrahedra;
      Quads newSurfaceQuads;
      for ( auto quad: _surfaceQuads )
      {
        auto nodeA = quad->node0( );
        auto nodeB = quad->node1( );
        auto nodeC = quad->node2( );
        auto nodeD = quad->node3( );

        auto nodeE = _middleNode( nodeA, nodeB, newNodes );
        auto nodeF = _middleNode( nodeB, nodeC, newNodes );
        auto nodeG = _middleNode( nodeC, nodeD, newNodes );
        auto nodeH = _middleNode( nodeD, nodeA, newNodes );
        auto nodeI = _middleNode( nodeB, nodeD, newNodes );
        newSurfaceQuads.push_back( new Quad( nodeA, nodeE, nodeI, nodeH ));
        newSurfaceQuads.push_back( new Quad( nodeE, nodeB, nodeF, nodeI ));
        newSurfaceQuads.push_back( new Quad( nodeI, nodeF, nodeC, nodeG ));
        newSurfaceQuads.push_back( new Quad( nodeH, nodeI, nodeG, nodeD ));
        delete quad;
      }
      _surfaceQuads = newSurfaceQuads;
    }
    for( auto tet: _tetrahedra )
    {
      if ( tet->face0( ) || tet->face1( ) || tet->face2( ) || tet->face3( ))
      {
        _surfaceTetrahedra.push_back( tet );
      }
    }
  }

  void Icosphere::_computeCenters( void )
  {
    for ( auto surfaceNode: _surfaceNodes )
    {
      if ( !surfaceNode->fixed( ))
        surfaceNode->center( ) = Eigen::Vector3f( 0.0f, 0.0f, 0.0f );
    }
    for ( auto tet: _surfaceTetrahedra )
    {
      Eigen::Vector3f normal;
      Eigen::Vector3f axis0;
      Eigen::Vector3f axis1;
      auto node0 = tet->node0( );
      auto node1 = tet->node1( );
      auto node2 = tet->node2( );
      auto node3 = tet->node3( );
      if ( tet->face0( ))
      {
        axis0 = ( node1->position( ) - node0->position( )).normalized( );
        axis1 = ( node2->position( ) - node0->position( )).normalized( );
        normal = axis0.cross( axis1 );
        if ( !node0->fixed( ))
          node0->center( ) -= normal;
        if ( !node1->fixed( ))
          node1->center( ) -= normal;
        if ( !node2->fixed( ))
          node2->center( ) -= normal;
      }
      if ( tet->face1( ))
      {
        axis0 = ( node2->position( ) - node0->position( )).normalized( );
        axis1 = ( node3->position( ) - node0->position( )).normalized( );
        normal = axis0.cross( axis1 );
        if ( !node0->fixed( ))
          node0->center( ) -= normal;
        if ( !node2->fixed( ))
          node2->center( ) -= normal;
        if ( !node3->fixed( ))
          node3->center( ) -= normal;
      }
      if ( tet->face2( ))
      {
        axis0 = ( node3->position( ) - node0->position( )).normalized( );
        axis1 = ( node1->position( ) - node0->position( )).normalized( );
        normal = axis0.cross( axis1 );
        if ( !node0->fixed( ))
          node0->center( ) -= normal;
        if ( !node1->fixed( ))
          node1->center( ) -= normal;
        if ( !node3->fixed( ))
          node3->center( ) -= normal;
      }
      if ( tet->face3( ))
      {
        axis0 = ( node3->position( ) - node1->position( )).normalized( );
        axis1 = ( node2->position( ) - node1->position( )).normalized( );
        normal = axis0.cross( axis1 );
        if ( !node1->fixed( ))
          node1->center( ) -= normal;
        if ( !node2->fixed( ))
          node2->center( ) -= normal;
        if ( !node3->fixed( ))
          node3->center( ) -= normal;
      }
    }
    for ( auto surfaceNode: _surfaceNodes )
    {
      if ( !surfaceNode->fixed( ))
      {
        if ( surfaceNode->center( ).norm( ) < 0.000001f )
        {
          surfaceNode->center( ) = surfaceNode->position( ) - _center;
          std::cout << "norm < 0 " << std::endl;
        }
        surfaceNode->center( ).normalize( );
        surfaceNode->center( ) *= 0.01f;
        surfaceNode->center( ) += surfaceNode->position( );
      }
    }
  }

  nlphysics::NodePtr Icosphere::_middleNode(
      nlphysics::NodePtr node0_, nlphysics::NodePtr node1_,
      std::unordered_map< MyPair, nlphysics::NodePtr,
      std::hash<MyPair> >& newNodes_
    )
  {
    unsigned int minId, maxId;
    if ( node0_->id( ) <= node1_->id( ))
    {
      minId = node0_->id( );
      maxId = node1_->id( );
    }
    else
    {
      minId = node1_->id( );
      maxId = node0_->id( );
    }
    MyPair pair;
    pair.first_int = minId;
    pair.second_int = maxId;

    auto newNodeIt = newNodes_.find( pair );
    nlphysics::NodePtr newNode;
    if ( newNodeIt != newNodes_.end( ))
      newNode = newNodeIt->second;
    else
    {
      Eigen::Vector3f position =
        ( node0_->position( ) + node1_->position( )) * 0.5f;
      if ( node0_->contour( ) && node1_->contour( ))
      {
        position = ( position - _center ).normalized( ) * _radius + _center;
        newNode =
          new nlphysics::Node( position, ( unsigned int )_nodes.size( ), true );
        _surfaceNodes.push_back( newNode );
      }
      else
      {
        newNode =
          new nlphysics::Node( position, ( unsigned int )_nodes.size( ));
      }
        newNodes_[pair] = newNode;
      _nodes.push_back( newNode );
    }
    return newNode;
  }

  nlgeometry::OrbitalVertexPtr Icosphere::_nodeToVertex(
    nlphysics::NodePtr node_,
    std::unordered_map< unsigned int, nlgeometry::OrbitalVertexPtr >&
    vertices_ )
  {
    auto iterator = vertices_.find( node_->id( ));
    nlgeometry::OrbitalVertexPtr vertex;
    if ( iterator == vertices_.end( ))
    {
      vertex = new nlgeometry::OrbitalVertex( node_->position( ),
                                              node_->center( ));
      vertices_[ node_->id( ) ] = vertex;
    }
    else
    {
      vertex = iterator->second;
    }
    return vertex;
  }

} // namespace nlgenerator
