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
#include "MeshGenerator.h"

#include "Icosphere.h"

namespace nlgenerator
{

  nlgeometry::MeshPtr MeshGenerator::generateMesh(
    nsol::MorphologyPtr morphology_ )
  {
    nsol::NeuronMorphologyPtr neuronMorphology =
      dynamic_cast< nsol::NeuronMorphologyPtr >( morphology_ );
    if ( neuronMorphology )
      return _generateMophology( neuronMorphology );
    else
      return _generateMorphology( morphology_ );
  }

  nlgeometry::MeshPtr MeshGenerator::generateMesh(
    nsol::NeuronMorphologyPtr morphology_,
    float alphaRadius_,
    const std::vector< float >& alphaNeurites_ )
  {
    auto mesh = new nlgeometry::Mesh( );

    nsol::Sections sections;
    for ( auto neurite: morphology_->neurites(  ))
    {
      auto section = neurite->firstSection( );
      if ( section->nodes( ).size( ) == 1 )
      {
        auto firstSecNode = section->firstNode( );
        Eigen::Vector3f position =
          ( morphology_->soma( )->center( ) - firstSecNode->point( )
            ).normalized( ) * firstSecNode->radius( ) + firstSecNode->point( );
        auto newNode = new nsol::Node( position, firstSecNode->id( ),
                                       firstSecNode->radius( ));
        section->addBackwardNode( newNode );
      }
      sections.push_back( neurite->firstSection( ));
    }

    auto joints = _vectorizeJoints( sections );

    JointNodes firstJoints;
    const float somaRadius = morphology_->soma( )->meanRadius( );
    Eigen::Vector3f somaCenter = morphology_->soma( )->center( );
    for ( unsigned int i = 0; i < morphology_->neurites(  ).size( ); ++i )
    {
      auto neurite = morphology_->neurites( )[i];
      auto joint = joints[neurite->firstSection( )->firstNode( )];
      joint->connectedSoma( ) = true;
      Eigen::Vector3f axis = joint->position( ) - somaCenter;
      float module = axis.norm( ) - somaRadius;
      module = module * alphaNeurites_[i] + somaRadius;
      joint->position( ) = somaCenter + axis.normalized( ) * module;
      firstJoints.push_back( joint );
    }

    for ( auto element: joints )
    {
      auto &joint = element.second;
      joint->computeGeometry( );
    }

    Icosphere icosphere( somaCenter, somaRadius * alphaRadius_, 3 );

    mesh->triangles( ) = icosphere.compute( firstJoints );

    auto facets = _meshSections( sections, joints );

    for ( auto element: joints )
    {
      auto joint = element.second;
      if ( joint->numberNeighbors( ) == 1 && !joint->connectedSoma( ))
      {
        auto sectionQuad = joint->sectionQuad( );
        auto node = joint->neighbour( );
        Eigen::Vector3f center = joint->position( );
        Eigen::Vector3f position = ( center - node->point( )
          ).normalized( ) * joint->radius( ) + center;
        auto vertex = new nlgeometry::OrbitalVertex( position, center );
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex0( ),
                                 sectionQuad->vertex1( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex1( ),
                                 sectionQuad->vertex2( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex2( ),
                                 sectionQuad->vertex3( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex3( ),
                                 sectionQuad->vertex0( ),
                                 vertex, vertex ));
      }
    }

    mesh->quads( ) = facets;
    return mesh;
  }

  nlgeometry::MeshPtr MeshGenerator::generateStructureMesh(
    nsol::MorphologyPtr morphology_, NodeIdToVertices& nodeIdToVertices_,
    Eigen::Vector3f color_, bool generateNodes_, float offset_ )
  {
    auto mesh = new nlgeometry::Mesh(  );
    nodeIdToVertices_.clear( );
    nsol::Sections firstSections;

    nsol::NeuronMorphologyPtr neuronMorphology =
      dynamic_cast< nsol::NeuronMorphologyPtr >( morphology_ );
    if ( neuronMorphology )
    {
      auto somaNodes = neuronMorphology->soma( )->nodes( );
      if ( somaNodes.size( ) > 0 )
      {
        auto somaSection = new nsol::NeuronMorphologySection( );
        nsol::NodePtr firstNode = somaNodes[0];
        somaSection->addNode( firstNode );
        nsol::NodePtr previousNode = nullptr;
        for ( unsigned int i = 1; i < somaNodes.size( ); i++ )
        {
          previousNode = somaNodes[i];
          somaSection->addNode( previousNode );
        }
        if ( previousNode )
          somaSection->addNode( firstNode );
        firstSections.push_back( somaSection );
      }

      for ( auto neurite: neuronMorphology->neurites( ))
        firstSections.push_back( neurite->firstSection( ));
    }
    else
    {
      firstSections = morphology_->sections( );
    }

    std::set< nsol::SectionPtr > uniqueSections;

    for ( auto firstSection: firstSections )
      _vectorizeSections( mesh, firstSection, uniqueSections, nodeIdToVertices_,
                          color_, generateNodes_, offset_ );

    for ( auto section: uniqueSections )
    {
      if ( nodeIdToVertices_.find( section->backwardNode( )->id( )) !=
           nodeIdToVertices_.end( ))
      {
        auto firstVertex =
          nodeIdToVertices_[section->backwardNode( )->id( )][0];
        if ( nodeIdToVertices_.find( section->forwardNode( )->id( )) !=
             nodeIdToVertices_.end( ))
        {
          auto lastVertex =
            nodeIdToVertices_[section->forwardNode( )->id( )][0];
          auto previousVertex = firstVertex;
          auto nodes = section->nodes( );
          for ( unsigned int i = 1; i < nodes.size( ) - 1; i++ )
          {
            auto currentNode = nodes[i];
            if ( currentNode != section->forwardNode( ))
            {
              auto currentVertex =
                new nlgeometry::OrbitalVertex(
                  currentNode->point( ), currentNode->point( ),
                  Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), color_ );
              nodeIdToVertices_[currentNode->id( )].push_back( currentVertex );
              mesh->lines( ).push_back(
                new nlgeometry::Facet( previousVertex, currentVertex ));
              if ( generateNodes_ )
              {
                auto triangles = _generateCube( currentNode, nodeIdToVertices_,
                                                color_, offset_ );
                mesh->triangles().insert(
                  mesh->triangles( ).end( ), triangles.begin( ),
                  triangles.end( ));
              }

              previousVertex = currentVertex;
            }
          }
          mesh->lines( ).push_back(
            new nlgeometry::Facet( previousVertex, lastVertex ));
        }
      }
    }

    return mesh;
  }

  void MeshGenerator::verticesToIndices(
    NodeIdToVertices& nodeIdToVertices_,
    NodeIdToVerticesIds& nodeIdToVerticesIds_ )
  {
    nodeIdToVerticesIds_.clear( );
    for ( auto cell: nodeIdToVertices_ )
      for ( auto vertex: cell.second )
        nodeIdToVerticesIds_[ cell.first ].push_back( vertex->id( ));
  }

  void MeshGenerator::conformBuffer( std::vector< unsigned int >& nodeIds_,
                                     NodeIdToVerticesIds& nodeIdToVerticesIds_,
                                     std::vector< float >& buffer_,
                                     Eigen::Vector3f value_ )
  {
    for ( auto nodeId: nodeIds_ )
    {
      for ( auto vertexId: nodeIdToVerticesIds_[nodeId] )
      {
        buffer_[vertexId*3] = value_.x( );
        buffer_[vertexId*3+1] = value_.y( );
        buffer_[vertexId*3+2] = value_.z( );
      }
    }
  }

  nlgeometry::MeshPtr MeshGenerator::_generateMorphology(
    nsol::MorphologyPtr morphology_ )
  {
    auto mesh = new nlgeometry::Mesh( );

    nsol::Sections sections = morphology_->sections( );

    auto joints = _vectorizeJoints( sections );
    for ( auto element: joints )
    {
      auto &joint = element.second;
      joint->computeGeometry( );
    }
    auto facets = _meshSections( sections, joints );

    for ( auto element: joints )
    {
      auto &joint = element.second;
      if ( joint->numberNeighbors( ) == 1 && !joint->connectedSoma( ))
      {
        auto sectionQuad = joint->sectionQuad( );
        auto node = joint->neighbour( );
        const auto center = joint->position( );
        const auto position = (center - node->point( )).normalized( ) * joint->radius( ) + center;
        auto vertex = new nlgeometry::OrbitalVertex( position, center );

        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex1( ),
                                 sectionQuad->vertex0( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex2( ),
                                 sectionQuad->vertex1( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex3( ),
                                 sectionQuad->vertex2( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex0( ),
                                 sectionQuad->vertex3( ),
                                 vertex, vertex ));
      }
    }

    mesh->quads( ) = facets;

    return mesh;
  }

  nlgeometry::MeshPtr MeshGenerator::_generateMophology(
    nsol::NeuronMorphologyPtr morphology_ )
  {
    auto mesh = new nlgeometry::Mesh( );

    nsol::Sections sections;
    for ( auto neurite: morphology_->neurites(  ))
    {
      auto section = neurite->firstSection( );
      if ( section->nodes( ).size( ) == 1 )
      {
        auto firstSecNode = section->firstNode( );
        Eigen::Vector3f position =
          ( morphology_->soma( )->center( ) - firstSecNode->point( )
            ).normalized( ) * firstSecNode->radius( ) + firstSecNode->point( );
        auto newNode = new nsol::Node( position, firstSecNode->id( ),
                                       firstSecNode->radius( ));
        section->addBackwardNode( newNode );
      }
      sections.push_back( neurite->firstSection( ));
    }

    auto joints = _vectorizeJoints( sections );

    JointNodes firstJoints;
    for ( auto neurite: morphology_->neurites(  ))
    {
      auto &joint = joints[neurite->firstSection( )->firstNode( )];
      joint->connectedSoma( ) = true;
      firstJoints.push_back( joint );
    }

    for ( auto element: joints )
    {
      auto &joint = element.second;
      joint->computeGeometry( );
    }

    Icosphere icosphere( morphology_->soma( )->center( ),
                         morphology_->soma( )->meanRadius( ), 3 );

    mesh->triangles( ) = icosphere.compute( firstJoints );

    auto facets = _meshSections( sections, joints );

    for ( auto element: joints )
    {
      auto joint = element.second;
      if ( joint->numberNeighbors( ) == 1 && !joint->connectedSoma( ))
      {
        auto sectionQuad = joint->sectionQuad( );
        auto node = joint->neighbour( );
        Eigen::Vector3f center = joint->position( );
        Eigen::Vector3f position = ( center - node->point( )
          ).normalized( ) * joint->radius( ) + center;
        auto vertex = new nlgeometry::OrbitalVertex( position, center );
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex1( ),
                                 sectionQuad->vertex0( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex2( ),
                                 sectionQuad->vertex1( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex3( ),
                                 sectionQuad->vertex2( ),
                                 vertex, vertex ));
        facets.push_back(
          new nlgeometry::Facet( sectionQuad->vertex0( ),
                                 sectionQuad->vertex3( ),
                                 vertex, vertex ));
      }
    }

    mesh->quads( ) = facets;
    return mesh;
  }

  std::unordered_map< nsol::NodePtr, JointNodePtr >
  MeshGenerator::_vectorizeJoints( const nsol::Sections& sections_ )
  {
    std::unordered_map< nsol::NodePtr, JointNodePtr > joints;

    std::set< nsol::SectionPtr > uniqueSections;

    for ( auto section: sections_ )
    {
      _vectorizeJoints( uniqueSections, section, joints );
    }

    return joints;
  }

  void MeshGenerator::_vectorizeJoints(
    std::set< nsol::SectionPtr>& uniqueSections_,
    const nsol::SectionPtr& section_,
    std::unordered_map< nsol::NodePtr, JointNodePtr >& joints_ )
  {
    if ( uniqueSections_.find( section_ ) == uniqueSections_.end( ))
    {
      uniqueSections_.insert( section_ );
      unsigned int numNodes = static_cast<unsigned int>(section_->nodes( ).size( ));
      if ( numNodes > 1 )
      {
        nsol::NodePtr nsolJoint;
        nsol::NodePtr nsolNeighbour;
        nsolJoint = section_->nodes( ).front( );
        nsolNeighbour = section_->nodes( )[ 1 ];

        if ( joints_.find( nsolJoint ) == joints_.end( ))
        {
          auto joint = new JointNode( nsolJoint->point( ),
                                      nsolJoint->radius( ));
          joints_[nsolJoint] = joint;
        }
        joints_[nsolJoint]->addNeighbour( nsolNeighbour );

        nsolJoint = section_->nodes( ).back( );
        nsolNeighbour = section_->nodes( )[ numNodes - 2 ];
        if ( joints_.find( nsolJoint ) == joints_.end( ))
        {
          auto joint = new JointNode( nsolJoint->point( ),
                                    nsolJoint->radius( ));
          joints_[nsolJoint] = joint;
        }
        joints_[nsolJoint]->addNeighbour( nsolNeighbour );
      }
      for ( auto nextSection: section_->backwardNeighbors( ))
        _vectorizeJoints( uniqueSections_, nextSection, joints_ );
      for ( auto nextSection: section_->forwardNeighbors( ))
        _vectorizeJoints( uniqueSections_, nextSection, joints_ );
    }
  }

  nlgeometry::Facets MeshGenerator::_meshSections(
    const nsol::Sections& sections_,
    std::unordered_map< nsol::NodePtr, JointNodePtr >& joints_ )
  {
    nlgeometry::Facets facets;

    std::set< nsol::SectionPtr > uniqueSections;

    for ( auto section: sections_ )
      _meshSections( uniqueSections, section, joints_, facets );

    return facets;
  }

  void MeshGenerator::_meshSections(
    std::set< nsol::SectionPtr>& uniqueSections_,
    const nsol::SectionPtr& section_,
    std::unordered_map< nsol::NodePtr, JointNodePtr >& joints_,
    nlgeometry::Facets& facets_ )
  {
    if ( uniqueSections_.find( section_ ) == uniqueSections_.end( ))
    {
      uniqueSections_.insert( section_ );

      auto nodes = section_->nodes( );
      unsigned int numNodes = static_cast<unsigned int>(nodes.size( ));


      auto startJointIt = joints_.find( nodes.front( ));
      auto endJointIt = joints_.find( nodes.back( ));
      JointNodePtr startJoint;
      JointNodePtr endJoint;
      if ( startJointIt != joints_.end( ) && endJointIt != joints_.end( ))
      {
        startJoint = startJointIt->second;
        endJoint = endJointIt->second;
        nlgeometry::SectionQuadPtr prim0;
        nlgeometry::SectionQuadPtr prim1;
        if ( numNodes == 2 )
        {
          prim0 = startJoint->sectionQuad( nodes.back( ));
          prim1 = endJoint->sectionQuad( nodes.front( ));
          if ( prim0 && prim1 )
            nlgeometry::SectionQuad::createPipe(
              prim0, prim1->inversed( ), facets_, true );
        }
        else if ( numNodes > 2 )
        {
          prim0 = startJoint->sectionQuad( nodes.at( 1 ));
          prim1 =  endJoint->sectionQuad( nodes.at( numNodes - 2 ));
          if ( prim0 && prim1 )
          {
            float totalDist = 0.0f;
            nlgeometry::SectionQuadPtr preQuad = prim0->clone( );
            nlgeometry::SectionQuadPtr quad = prim0->clone( );
            quad->normalize( );
            for ( unsigned int nodeId = 1; nodeId < numNodes - 1; nodeId++ )
            {
              totalDist +=
                (nodes[nodeId]->point( ) - nodes[nodeId-1]->point( )).norm( );

              Eigen::Vector3f center = nodes[nodeId]->point( );
              Eigen::Vector3f exe = ( center - nodes[nodeId-1]->point(
                                        )).normalized( );
              Eigen::Vector3f exe1 = ( nodes[nodeId+1]->point( ) -
                                       center ).normalized( );
              Eigen::Quaternion< float > q;
              Eigen::Quaternion< float > qI =
                Eigen::Quaternion<float>::Identity( );
              Eigen::Quaternion< float > qSlerp;
              q.setFromTwoVectors( exe, exe1 );
              qSlerp = q.slerp( 0.5f, qI );
              Eigen::Vector3f tangent = qSlerp * exe;

              exe = quad->normal( );
              q.setFromTwoVectors(exe,tangent);
              quad->rotate( q );
            }

            totalDist += (nodes[numNodes - 1]->point( ) -
                          nodes[numNodes-2]->point( )).norm( );

            float offsetAngle = quad->getZAngle( prim1->inversed( ));

            quad->deleteVertices( );
            delete quad;

            for ( unsigned int nodeId = 1; nodeId < numNodes - 1; nodeId++ )
            {
              Eigen::Vector3f center = nodes[nodeId]->point( );
              float dist = ( center - nodes[nodeId-1]->point( )).norm( );
              float radius = nodes[nodeId]->radius( );
              Eigen::Vector3f exe = ( center - nodes[nodeId-1]->point(
                                        )).normalized( );
              Eigen::Vector3f exe1 = ( nodes[nodeId+1]->point( ) -
                                       center ).normalized( );
              Eigen::Quaternion< float > q;
              Eigen::Quaternion< float > qI =
                Eigen::Quaternion<float>::Identity( );
              Eigen::Quaternion< float > qSlerp;
              q.setFromTwoVectors( exe, exe1 );
              qSlerp = q.slerp( 0.5f, qI );
              Eigen::Vector3f tangent = qSlerp * exe;
              quad = preQuad->clone( );
              quad->normalize( );
              exe = preQuad->normal( );
              quad->place( center );
              q.setFromTwoVectors(exe,tangent);
              quad->rotate( q );

              float zfactor = dist/totalDist;
              q = Eigen::Quaternion< float >(
                Eigen::AngleAxis< float >(
                  offsetAngle * zfactor, quad->normal( ) ));
              quad->rotate( q );
              quad->norm( radius );
              if ( nodeId == 1 )
                nlgeometry::SectionQuad::createPipe( prim0, quad, facets_ );
              else
                nlgeometry::SectionQuad::createPipe( preQuad, quad, facets_ );

              delete preQuad;
              preQuad = quad;
            }
            nlgeometry::SectionQuad::createPipe(
              quad, prim1->inversed( ), facets_, true );
            delete quad;
          }
        }
      }

      for ( auto nextSection: section_->forwardNeighbors( ))
        _meshSections( uniqueSections_, nextSection, joints_, facets_ );
      for ( auto nextSection: section_->backwardNeighbors( ))
        _meshSections( uniqueSections_, nextSection, joints_, facets_ );
    }
  }

  void MeshGenerator::_vectorizeSections(
    nlgeometry::MeshPtr mesh_,
    const nsol::SectionPtr section_,
    std::set< nsol::SectionPtr>& uniqueSections_,
    NodeIdToVertices& nodeIdToVertices_,
    Eigen::Vector3f color_,
    bool generateNodes_,
    float offset_ )
  {
    if ( uniqueSections_.find( section_ ) == uniqueSections_.end( ))
    {
      uniqueSections_.insert( section_ );

      auto firstNode = section_->backwardNode( );
      if ( nodeIdToVertices_.find( firstNode->id()) == nodeIdToVertices_.end( ))
      {
        auto firstVertex = new nlgeometry::OrbitalVertex(
          firstNode->point( ), firstNode->point( ),
          Eigen::Vector3f( 0.0f, 0.0f, 0.0f), color_ );
        nodeIdToVertices_[firstNode->id( )].push_back( firstVertex );
      }
      if ( generateNodes_ )
      {
        auto triangles = _generateCube( firstNode, nodeIdToVertices_,
                                        color_, offset_ );
        mesh_->triangles().insert(
          mesh_->triangles().end( ), triangles.begin( ), triangles.end( ));
      }

      auto lastNode = section_->forwardNode( );
      if ( nodeIdToVertices_.find( lastNode->id( ) ) ==
           nodeIdToVertices_.end( ))
      {
        auto lastVertex = new nlgeometry::OrbitalVertex(
          lastNode->point( ), lastNode->point( ),
          Eigen::Vector3f( 0.0f, 0.0f, 0.0f), color_);
        nodeIdToVertices_[lastNode->id( )].push_back( lastVertex );
      }
      if ( generateNodes_ )
      {
        auto triangles = _generateCube( lastNode, nodeIdToVertices_,
                                        color_, offset_ );
        mesh_->triangles().insert(
          mesh_->triangles().end( ), triangles.begin( ), triangles.end( ));
      }

      for ( auto nextSection: section_->forwardNeighbors( ))
        _vectorizeSections( mesh_, nextSection, uniqueSections_,
                            nodeIdToVertices_, color_, generateNodes_,
                            offset_ );
      for ( auto nextSection: section_->backwardNeighbors( ))
        _vectorizeSections( mesh_, nextSection, uniqueSections_,
                            nodeIdToVertices_, color_, generateNodes_,
                            offset_ );
    }
  }

  nlgeometry::Facets MeshGenerator::_generateCube(
    nsol::NodePtr node_, NodeIdToVertices& nodeIdToVertices_,
    Eigen::Vector3f color_, float offset_ )
  {
    nlgeometry::Facets triangles;
    auto center = node_->point( );
    float radius = node_->radius( ) * offset_;
    auto vertex0 = new nlgeometry::OrbitalVertex(
      center + Eigen::Vector3f( radius, 0.0f, 0.0f ), center,
      Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), color_ );
    nodeIdToVertices_[node_->id( )].push_back( vertex0 );

    auto vertex1 = new nlgeometry::OrbitalVertex(
      center + Eigen::Vector3f( -radius, 0.0f, 0.0f ), center,
      Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), color_ );
    nodeIdToVertices_[node_->id( )].push_back( vertex1 );

    auto vertex2 = new nlgeometry::OrbitalVertex(
      center + Eigen::Vector3f( 0.0f, radius, 0.0f ), center,
      Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), color_ );
    nodeIdToVertices_[node_->id( )].push_back( vertex2 );

    auto vertex3 = new nlgeometry::OrbitalVertex(
      center + Eigen::Vector3f( 0.0f, -radius, 0.0f ), center,
      Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), color_ );
    nodeIdToVertices_[node_->id( )].push_back( vertex3 );

    auto vertex4 = new nlgeometry::OrbitalVertex(
      center + Eigen::Vector3f( 0.0f, 0.0f, radius ), center,
      Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), color_ );
    nodeIdToVertices_[node_->id( )].push_back( vertex4 );

    auto vertex5 = new nlgeometry::OrbitalVertex(
      center + Eigen::Vector3f( 0.0f, 0.0f, -radius ), center,
      Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), color_ );
    nodeIdToVertices_[node_->id( )].push_back( vertex5 );

    triangles.push_back( new nlgeometry::Facet( vertex2, vertex4, vertex0 ));
    triangles.push_back( new nlgeometry::Facet( vertex2, vertex1, vertex4 ));
    triangles.push_back( new nlgeometry::Facet( vertex2, vertex5, vertex1 ));
    triangles.push_back( new nlgeometry::Facet( vertex2, vertex0, vertex5 ));

    triangles.push_back( new nlgeometry::Facet( vertex3, vertex0, vertex4 ));
    triangles.push_back( new nlgeometry::Facet( vertex3, vertex4, vertex1 ));
    triangles.push_back( new nlgeometry::Facet( vertex3, vertex1, vertex5 ));
    triangles.push_back( new nlgeometry::Facet( vertex3, vertex5, vertex0 ));

    return triangles;
  }

} // namespace nlgenerator
