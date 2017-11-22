/**
 * @file    NeuronMeshGenerator.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "MeshGenerator.h"

#include "Icosphere.h"

#define EPSILON 0.00001f

#define END_NODE_DISPLACE 0.3f

namespace nlgenerator
{

  nlgeometry::MeshPtr MeshGenerator::generateMesh(
    nsol::NeuronMorphologyPtr& morphology_ )
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
      auto joint = joints[neurite->firstSection( )->firstNode( )];
      joint->connectedSoma( ) = true;
      firstJoints.push_back( joint );
    }
    for ( auto element: joints )
    {
      auto joint = element.second;
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

  nlgeometry::MeshPtr MeshGenerator::generateMesh(
    nsol::NeuronMorphologyPtr& morphology_,
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
    float somaRadius = morphology_->soma( )->meanRadius( );
    Eigen::Vector3f somaCenter = morphology_->soma( )->center( );
    for ( unsigned int i = 0; i < morphology_->neurites(  ).size( ); i++ )
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
      auto joint = element.second;
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
      unsigned int numNodes = section_->nodes( ).size( );
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
      unsigned int numNodes = nodes.size( );


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


} // namespace nlgenerator
