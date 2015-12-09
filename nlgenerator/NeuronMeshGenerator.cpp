/**
 * @file    NeuronMeshGenerator.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "NeuronMeshGenerator.h"

using namespace nsol;
using namespace std;

namespace nlgenerator
{

//CONSTRUCTORS AND DESTRUCTORS

  NeuronMeshGenerator::NeuronMeshGenerator( void )
  {

  }

  NeuronMeshGenerator::~NeuronMeshGenerator( void )
  {

  }

// PUBLIC METHODS

  void NeuronMeshGenerator::GenerateMesh(
    const nsol::NeuronMorphologyPtr& morpho,
    vector< float > & vertices,
    vector< float > & centers,
    vector< float > & tangents,
    vector< unsigned int > & mesh,
    unsigned int& somaEnd )
  {
    vertices.clear( );
    centers.clear( );
    tangents.clear( );
    mesh.clear( );

    std::vector< VectorizedNodePtr > vNodes;
    std::vector< VectorizedNodePtr > firstNodes;

    VectorizeMorphology( morpho, vNodes, firstNodes );

    GenerateSoma( morpho->soma( ), firstNodes, vertices, centers,
                           tangents, mesh );

    somaEnd = int( mesh.size( ));

    CalculateTangents( vNodes );
    CalculateGeometry( vNodes, vertices, centers, tangents);

    for ( unsigned int i = 0; i < vNodes.size(); i++ )
    {
      VectorizedNodePtr vNode = vNodes[i];
      if( vNode->Father( ) != nullptr )
      {
        _CreateQuadPipe( vNode->Father( )->Primitive( ),
                             vNode->Primitive(), mesh );
        if( vNode->Childs( ).size( ) == 0 )
        {
          mesh.push_back( vNode->Primitive( )->A( ));
          mesh.push_back( vNode->Primitive( )->B( ));
          mesh.push_back( vNode->Primitive( )->D( ));
          mesh.push_back( vNode->Primitive( )->C( ));
        }
      }
    }
  }

  void NeuronMeshGenerator::GenerateMesh(
    const nsol::NeuronMorphologyPtr& morpho,
    const float& alphaRadius,
    const std::vector< float >& alphaFirstNodes,
    vector< float >& vertices,
    vector< float >& centers,
    vector< float >& tangents,
    vector< unsigned int >& mesh,
    unsigned int& somaEnd )
  {
    vertices.clear( );
    centers.clear( );
    tangents.clear( );
    mesh.clear( );

    std::vector< VectorizedNodePtr > vNodes;
    std::vector< VectorizedNodePtr > firstNodes;

    VectorizeMorphology( morpho, vNodes, firstNodes );



    GenerateSoma( morpho->soma( ), alphaRadius, firstNodes, alphaFirstNodes,
                  vertices, centers, tangents, mesh );

    somaEnd = int( mesh.size( ));

    CalculateTangents( vNodes );
    CalculateGeometry( vNodes, vertices, centers, tangents);

    for ( unsigned int i = 0; i < vNodes.size(); i++ )
    {
      VectorizedNodePtr vNode = vNodes[i];
      if( vNode->Father( ) != nullptr )
      {
        _CreateQuadPipe( vNode->Father( )->Primitive( ),
                             vNode->Primitive(), mesh );
        if( vNode->Childs( ).size( ) == 0 )
        {
          mesh.push_back( vNode->Primitive( )->A( ));
          mesh.push_back( vNode->Primitive( )->B( ));
          mesh.push_back( vNode->Primitive( )->D( ));
          mesh.push_back( vNode->Primitive( )->C( ));
        }
      }
    }
  }

  void NeuronMeshGenerator::GenerateSoma(
    const nsol::SomaPtr& soma,
    std::vector< VectorizedNodePtr >& firstNodes,
    std::vector< float >& vertices,
    std::vector< float >& centers,
    std::vector< float >& tangents,
    std::vector< unsigned int >& mesh )
  {
    Vec3f c = soma->center( );
    Eigen::Vector3f center( c.x( ), c.y( ), c.z( ) );

    Nodes nodes = soma->nodes( );
    float radius = std::numeric_limits< float >::max( );
    for( unsigned int i = 0; i < ( unsigned int ) nodes.size(); i++ )
    {
      float r = (nodes[i]->point() - c).norm();
      if ( r < radius )
        radius = r;
    }

    Icosphere ico( center, radius, 3 );

    ico.CalculateSoma( firstNodes );

    ico.TrianglesToVector( firstNodes, vertices, centers, tangents,
                                      mesh );
  }


  void NeuronMeshGenerator::GenerateSoma(
    const nsol::SomaPtr& soma,
    const float& alphaRadius,
    std::vector< VectorizedNodePtr > & firstNodes,
    const std::vector< float >& alphaFirstNodes,
    std::vector< float >& vertices,
    std::vector< float >& centers,
    std::vector< float >& tangents,
    std::vector< unsigned int >& mesh )
  {
    Vec3f c = soma->center( );
    Eigen::Vector3f center( c.x( ), c.y( ), c.z( ) );

    Nodes nodes = soma->nodes( );
    float radius = FLT_MAX;
    for( unsigned int i = 0; i < nodes.size(); i++ )
    {
      float r = (nodes[i]->point() - c).norm();
      if ( r < radius )
        radius = r;
    }
    radius *= alphaRadius;
    Icosphere ico( center, radius, 3 );

    if ( firstNodes.size( ) == alphaFirstNodes.size( ))
    {
      for ( unsigned int i = 0; i < firstNodes.size( ); i++ )
      {
        Eigen::Vector3f exe = firstNodes[i]->Position( ) - center;
        float module = exe.norm( ) - radius;
        module = module * alphaFirstNodes[i] + radius;
        firstNodes[i]->Position( center + exe.normalized( ) *  module );
      }
    }
    ico.CalculateSoma( firstNodes );

    ico.TrianglesToVector( firstNodes, vertices, centers, tangents,
                                      mesh );
  }

  void NeuronMeshGenerator::VectorizeMorphology(
    const nsol::NeuronMorphologyPtr& morpho,
    vector< VectorizedNodePtr >& vNodes,
    vector< VectorizedNodePtr >& firstNodes )
  {
    vNodes.clear( );
    firstNodes.clear( );
    Neurites neurites = morpho->neurites( );

    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );


      VectorizedNodePtr vNode = new VectorizedNode( );
      NodePtr node = section->firstNode( );
      Vec3f pos = node->point( );
      vNode->Position( Eigen::Vector3f( pos.x(), pos.y(), pos.z( )));
      vNode->Radius( node->radius( ));
      vNode->Id( int( vNodes.size( )));

      vNodes.push_back( vNode );
      firstNodes.push_back( vNode );

      _VectorizeMorphology(section, vNodes, vNode);
    }
  }

  void NeuronMeshGenerator::CalculateTangents(
    std::vector< VectorizedNodePtr >& vNodes )
  {
    for ( unsigned int i = 0; i < vNodes.size(); i++ )
    {
      VectorizedNodePtr vNode = vNodes[i];
      if( !vNode->FirstNode( ))
      {
        VectorizedNodePtr vNodeFather = vNode->Father( );
        vector< VectorizedNodePtr > childNodes = vNode->Childs();

        Eigen::Vector3f tangent( 0.0f, 0.0f, 0.0f );

        if ( childNodes.size( ) == 0 ) // End node
        {
          vNodeFather = vNode->Father() ;
          tangent = ( vNode->Position( )
                      - vNodeFather->Position( )).normalized( );
        }
        else //Bifurcation and normal node
        {

          Eigen::Vector3f localTangent;
          for ( unsigned int j = 0; j < childNodes.size(); j++ )
          {
            localTangent = childNodes[j]->Position( ) - vNode->Position( );
            if( localTangent.norm( ) > 0.000001 )
            {
              localTangent.normalize( );
            }
            tangent += localTangent;
          }
          if( tangent.norm() > 0.000001)
          {
            tangent.normalize( );
          }
          localTangent = vNode->Position( ) - vNodeFather->Position( );
          if( localTangent.norm( ) > 0.000001 )
          {
            localTangent.normalize( );
          }
          tangent  += localTangent;
          if( tangent.norm() > 0.000001)
          {
            tangent.normalize( );
          }
        }

        vNode->Tangent(tangent);
      }
    }
  }

  void NeuronMeshGenerator::CalculateGeometry(
    vector< VectorizedNodePtr >& vNodes,
    vector< float >& vertices,
    vector< float >& centers,
    vector< float >& tangents )
  {
    Eigen::Vector3f exe( 0.0f, 0.0f, 1.0f );

    Eigen::Vector3f va( 0.0f, 1.0f, 0.0f );
    Eigen::Vector3f vb( -1.0f, 0.0f, 0.0f );
    Eigen::Vector3f vc( 0.0f, -1.0f, 0.0f );
    Eigen::Vector3f vd( 1.0f, 0.0f, 0.0f );

    Eigen::Vector3f center;
    Eigen::Vector3f tangent;
    Eigen::Vector3f position;

    Eigen::Quaternion< float > q;

    for ( unsigned int i = 0; i < vNodes.size(); i++ )
    {
      VectorizedNodePtr vNode = vNodes[i];

      if ( !vNode->FirstNode( ))
      {
        center = vNode->Position( );
        tangent = vNode->Tangent();

        VectorizedNodePtr father = vNode->Father( );

        exe = father->Tangent( );

        q.setFromTwoVectors(exe,tangent);

        GeometricPrimitivePtr fatherGeom = father->Primitive();

        int vecId = fatherGeom->A( ) * 3;


        va = ( Eigen::Vector3f( vertices[vecId], vertices[vecId + 1],
             vertices[vecId + 2] ) - father->Position( )).normalized( );
        vecId = fatherGeom->B( ) * 3;
        vb = ( Eigen::Vector3f( vertices[vecId], vertices[vecId + 1],
             vertices[vecId + 2] )- father->Position( )).normalized( ) ;
        vecId = fatherGeom->C( ) * 3;
        vc = ( Eigen::Vector3f( vertices[vecId], vertices[vecId + 1],
             vertices[vecId + 2] ) - father->Position( )).normalized( );
        vecId = fatherGeom->D( ) * 3;
        vd = ( Eigen::Vector3f( vertices[vecId], vertices[vecId + 1],
             vertices[vecId + 2] ) - father->Position( )).normalized( );


        int a = int( vertices.size( )) / 3;
        position = q._transformVector( va ) * vNode->Radius( ) + center;

        vertices.push_back( position.x( ));
        vertices.push_back( position.y( ));
        vertices.push_back( position.z( ));

        int b = int( vertices.size( )) / 3;
        position = q._transformVector( vb ) * vNode->Radius( ) + center;

        vertices.push_back( position.x( ));
        vertices.push_back( position.y( ));
        vertices.push_back( position.z( ));

        int c = int( vertices.size( ) ) / 3;
        position = q._transformVector( vc ) * vNode->Radius( ) + center;

        vertices.push_back( position.x( ));
        vertices.push_back( position.y( ));
        vertices.push_back( position.z( ));

        int d = int( vertices.size( ) ) / 3;
        position = q._transformVector( vd ) * vNode->Radius( ) + center;

        vertices.push_back( position.x( ));
        vertices.push_back( position.y( ));
        vertices.push_back( position.z( ));

        if ( vNode->Childs( ).size( ) == 0 )
        {
          center = center - tangent * 0.1f;
        }

        for (unsigned int j = 0; j < 4; j++ )
        {
          centers.push_back( center.x( ));
          centers.push_back( center.y( ));
          centers.push_back( center.z( ));

          tangents.push_back( tangent.x( ));
          tangents.push_back( tangent.y( ));
          tangents.push_back( tangent.z( ));
        }

        vNode->Primitive( new GeometricPrimitive( a, b, c, d ));
      }
    }
  }



// PRIVATE METHODS

  void NeuronMeshGenerator::_VectorizeMorphology(
    const SectionPtr& section,
    vector< VectorizedNodePtr >& vNodes,
    VectorizedNodePtr vFatherNode )
  {
    VectorizedNodePtr vNode;
    NodePtr node;
    Vec3f pos;

    for( unsigned int i = 0; i < section->middleNodes( ).size( ); i++ )
    {
      vNode = new VectorizedNode( );
      node = section->middleNodes( )[i];
      pos = node->point( );

      vNode->Position( Eigen::Vector3f( pos.x(), pos.y(), pos.z( )));
      vNode->Radius( node->radius( ));
      vNode->Id( int( vNodes.size( )));
      vNode->Father( vFatherNode);
      vNodes.push_back( vNode );

      vFatherNode->AddChild( vNode );
      vFatherNode = vNode;
    }

    if( section->lastNode( ))
    {
      vNode = new VectorizedNode( );
      node = section->lastNode( );
      pos = node->point( );

      vNode->Position( Eigen::Vector3f( pos.x(), pos.y(), pos.z( )));
      vNode->Radius( node->radius( ));
      vNode->Id( int( vNodes.size( )));
      vNode->Father( vFatherNode);
      vNodes.push_back( vNode );

      vFatherNode->AddChild( vNode );
      vFatherNode = vNode;
    }

    Sections childSections = section->children();
    for( unsigned int i = 0; i < childSections.size(); i++ )
    {
      _VectorizeMorphology( childSections[i], vNodes, vFatherNode );
    }
  }

  unsigned int NeuronMeshGenerator::_NumNodes(
    const nsol::NeuronMorphologyPtr& morpho )
  {
    unsigned int numNodes = 0;
    Neurites neurites = morpho->neurites( );
    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );
      numNodes += _NumNodes(section);

    }

    return numNodes;
  }

  unsigned int NeuronMeshGenerator::_NumNodes( const nsol::SectionPtr& section )
  {
    unsigned int numNodes = ( unsigned int )section->middleNodes( ).size( ) + 2;

      Sections childs = section->children();
      for( unsigned int i = 0; i < childs.size(); i++ )
      {
        numNodes += _NumNodes( childs[i] );

      }

      return numNodes;
  }

  unsigned int NeuronMeshGenerator::_MaxId(
    const nsol::NeuronMorphologyPtr& morpho )
  {
    unsigned int maxId = 0;
    Neurites neurites = morpho->neurites( );
    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );

      unsigned int id = _MaxId(section);
      if(id > maxId)
        maxId = id;
    }

    return maxId;
  }

  unsigned int NeuronMeshGenerator::_MaxId( const nsol::SectionPtr& section )
  {
      unsigned int maxId = 0;


      if( section->firstNode()->id( ) > section->lastNode()->id( ))
        maxId = section->firstNode( )->id( );
      else
        maxId = section->lastNode( )->id( );

      for( unsigned int i = 0; i < section->middleNodes( ).size( ); i++ )
      {
        unsigned int id = section->middleNodes( )[i]->id( );
        if ( id > maxId )
          maxId = id;
      }

      Sections childs = section->children();

      for( unsigned int i = 0; i < childs.size(); i++ )
      {
        unsigned int id = _MaxId( childs[i] );
        if( id > maxId )
          maxId = id;
      }

      return maxId;
  }

  void NeuronMeshGenerator::_CreateQuadPipe( const GeometricPrimitivePtr& geom0,
                                             const GeometricPrimitivePtr& geom1,
                                             vector< unsigned int >& mesh )
  {
    //AB
    mesh.push_back( geom0->A() );
    mesh.push_back( geom0->B() );
    mesh.push_back( geom1->A() );
    mesh.push_back( geom1->B() );

    //BC
    mesh.push_back( geom0->B() );
    mesh.push_back( geom0->C() );
    mesh.push_back( geom1->B() );
    mesh.push_back( geom1->C() );

    //CD
    mesh.push_back( geom0->C() );
    mesh.push_back( geom0->D() );
    mesh.push_back( geom1->C() );
    mesh.push_back( geom1->D() );

    //DA
    mesh.push_back( geom0->D() );
    mesh.push_back( geom0->A() );
    mesh.push_back( geom1->D() );
    mesh.push_back( geom1->A() );
  }
} // end namespace nlgenerator

// EOF
