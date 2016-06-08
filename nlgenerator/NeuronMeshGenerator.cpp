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

#define EPSILON 0.00001f

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
      if ( vNode->Father( ) != nullptr )
      {
        if ( vNode->Father( )->Bifurcation( ))
        {
          if ( vNode->Father( )->ChildPrimitive( vNode->Id( )))
            _CreateQuadPipe( vNode->Father( )->ChildPrimitive( vNode->Id( )),
                             vNode->Primitive( ), mesh );
        }
        else
        {
          _CreateQuadPipe( vNode->Father( )->Primitive( ),
                           vNode->Primitive(), mesh );
        }
        if( vNode->Childs( ).size( ) == 0 )
        {
          mesh.push_back( vNode->Primitive( )->a( ));
          mesh.push_back( vNode->Primitive( )->b( ));
          mesh.push_back( vNode->Primitive( )->e( ));
          mesh.push_back( vNode->Primitive( )->e( ));

          mesh.push_back( vNode->Primitive( )->b( ));
          mesh.push_back( vNode->Primitive( )->c( ));
          mesh.push_back( vNode->Primitive( )->e( ));
          mesh.push_back( vNode->Primitive( )->e( ));

          mesh.push_back( vNode->Primitive( )->c( ));
          mesh.push_back( vNode->Primitive( )->d( ));
          mesh.push_back( vNode->Primitive( )->e( ));
          mesh.push_back( vNode->Primitive( )->e( ));

          mesh.push_back( vNode->Primitive( )->d( ));
          mesh.push_back( vNode->Primitive( )->a( ));
          mesh.push_back( vNode->Primitive( )->e( ));
          mesh.push_back( vNode->Primitive( )->e( ));

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
        if ( vNode->Father( )->Bifurcation( ))
        {
          if ( vNode->Father( )->ChildPrimitive( vNode->Id( )))
            _CreateQuadPipe( vNode->Father( )->ChildPrimitive( vNode->Id( )),
                             vNode->Primitive( ), mesh );
        }
        else
        {
          _CreateQuadPipe( vNode->Father( )->Primitive( ),
                           vNode->Primitive(), mesh );
        }
        if( vNode->Childs( ).size( ) == 0 )
        {
          mesh.push_back( vNode->Primitive( )->a( ));
          mesh.push_back( vNode->Primitive( )->b( ));
          mesh.push_back( vNode->Primitive( )->e( ));
          mesh.push_back( vNode->Primitive( )->e( ));

          mesh.push_back( vNode->Primitive( )->b( ));
          mesh.push_back( vNode->Primitive( )->c( ));
          mesh.push_back( vNode->Primitive( )->e( ));
          mesh.push_back( vNode->Primitive( )->e( ));

          mesh.push_back( vNode->Primitive( )->c( ));
          mesh.push_back( vNode->Primitive( )->d( ));
          mesh.push_back( vNode->Primitive( )->e( ));
          mesh.push_back( vNode->Primitive( )->e( ));

          mesh.push_back( vNode->Primitive( )->d( ));
          mesh.push_back( vNode->Primitive( )->a( ));
          mesh.push_back( vNode->Primitive( )->e( ));
          mesh.push_back( vNode->Primitive( )->e( ));

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

    if ( radius < EPSILON )
    {
      std::cerr << "Warning: using soma max radius for the icosphere"
                << std::endl;
      radius = soma->maxRadius( );
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
    if ( radius < EPSILON )
    {
      std::cerr << "Warning: using soma max radius for the icosphere"
                << std::endl;
      radius = soma->maxRadius( );
    }
    #ifdef DEBUG
    assert( alphaRadius != 0 );
    #endif

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
            if( localTangent.norm( ) > EPSILON )
            {
              localTangent.normalize( );
            }
            tangent += localTangent;
          }
          if( tangent.norm() > EPSILON )
          {
            tangent.normalize( );
          }
          localTangent = vNode->Position( ) - vNodeFather->Position( );
          if( localTangent.norm( ) > EPSILON )
          {
            localTangent.normalize( );
          }
          tangent  += localTangent;
          if( tangent.norm() > EPSILON )
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
    Eigen::Vector3f ve;

    Eigen::Vector3f center;
    Eigen::Vector3f tangent;
    Eigen::Vector3f positionA;
    Eigen::Vector3f positionB;
    Eigen::Vector3f positionC;
    Eigen::Vector3f positionD;
    Eigen::Vector3f positionE;

    Eigen::Quaternion< float > q;

    for ( unsigned int i = 0; i < vNodes.size(); i++ )
    {
      VectorizedNodePtr vNode = vNodes[i];
      unsigned int numVertex = 4;
      if ( !vNode->FirstNode( ))
      {
        center = vNode->Position( );
        tangent = vNode->Tangent();

        VectorizedNodePtr father = vNode->Father( );

        exe = father->Tangent( );

        q.setFromTwoVectors(exe,tangent);

        GeometricPrimitivePtr fatherGeom = father->Primitive();



        int vecId = fatherGeom->a( ) * 3;
        va = ( Eigen::Vector3f( vertices[vecId], vertices[vecId + 1],
             vertices[vecId + 2] ) - father->Position( )).normalized( );
        vecId = fatherGeom->b( ) * 3;
        vb = ( Eigen::Vector3f( vertices[vecId], vertices[vecId + 1],
             vertices[vecId + 2] )- father->Position( )).normalized( ) ;
        vecId = fatherGeom->c( ) * 3;
        vc = ( Eigen::Vector3f( vertices[vecId], vertices[vecId + 1],
             vertices[vecId + 2] ) - father->Position( )).normalized( );
        vecId = fatherGeom->d( ) * 3;
        vd = ( Eigen::Vector3f( vertices[vecId], vertices[vecId + 1],
             vertices[vecId + 2] ) - father->Position( )).normalized( );


        int a = int( vertices.size( )) / 3;
        positionA = q._transformVector( va ) * vNode->Radius( ) + center;

        vertices.push_back( positionA.x( ));
        vertices.push_back( positionA.y( ));
        vertices.push_back( positionA.z( ));

        int b = int( vertices.size( )) / 3;
        positionB = q._transformVector( vb ) * vNode->Radius( ) + center;

        vertices.push_back( positionB.x( ));
        vertices.push_back( positionB.y( ));
        vertices.push_back( positionB.z( ));

        int c = int( vertices.size( ) ) / 3;
        positionC = q._transformVector( vc ) * vNode->Radius( ) + center;

        vertices.push_back( positionC.x( ));
        vertices.push_back( positionC.y( ));
        vertices.push_back( positionC.z( ));

        int d = int( vertices.size( ) ) / 3;
        positionD = q._transformVector( vd ) * vNode->Radius( ) + center;

        vertices.push_back( positionD.x( ));
        vertices.push_back( positionD.y( ));
        vertices.push_back( positionD.z( ));

        int e = -1;

        if ( vNode->Childs( ).size( ) == 0 )
        {
          // center = center - tangent * 0.1f;
          numVertex++;
          e = int( vertices.size( )) / 3;
          positionE = tangent * vNode->Radius( )*0.3 + center;

          vertices.push_back( positionE.x( ));
          vertices.push_back( positionE.y( ));
          vertices.push_back( positionE.z( ));
        }

        if ( vNode->Bifurcation( ))
        {
          numVertex++;
          e = int( vertices.size( )) / 3;
          positionE = tangent * vNode->Radius( ) + center;

          vertices.push_back( positionE.x( ));
          vertices.push_back( positionE.y( ));
          vertices.push_back( positionE.z( ));


          VectorizedNodePtr child0 = vNode->Childs( )[0];
          VectorizedNodePtr child1 = vNode->Childs( )[1];

          Eigen::Vector3f childCenter = child0->Position( );

          unsigned int minor = 0;
          float distance = ( childCenter - positionA ).norm( );
          float minorDistance = distance;

          distance = ( childCenter - positionB ).norm( );
          if ( distance < minorDistance )
          {
            minorDistance = distance;
            minor = 1;
          }

          distance = ( childCenter - positionC ).norm( );
          if ( distance < minorDistance )
          {
            minorDistance = distance;
            minor = 2;
          }

          distance = ( childCenter - positionD ).norm( );
          if ( distance < minorDistance )
          {
            minorDistance = distance;
            minor = 3;
          }

          switch( minor )
          {
          case 0:
            vNode->AddChildPrimitive( child0->Id( ),
                                    new GeometricPrimitive( a, b, e, d ));
            vNode->AddChildPrimitive( child1->Id( ),
                                    new GeometricPrimitive( e, b, c, d ));
            break;
          case 1:
            vNode->AddChildPrimitive( child0->Id( ),
                                    new GeometricPrimitive( a, b, c, e ));
            vNode->AddChildPrimitive( child1->Id( ),
                                    new GeometricPrimitive( a, e, c, d ));
            break;
          case 2:
            vNode->AddChildPrimitive( child0->Id( ),
                                    new GeometricPrimitive( e, b, c, d ));
            vNode->AddChildPrimitive( child1->Id( ),
                                    new GeometricPrimitive( a, b, e, d ));
            break;
          case 3:
            vNode->AddChildPrimitive( child0->Id( ),
                                    new GeometricPrimitive( a, e, c, d ));
            vNode->AddChildPrimitive( child1->Id( ),
                                    new GeometricPrimitive( a, b, c, e ));
            break;
          }
        }


        for (unsigned int j = 0; j < numVertex; j++ )
        {
          centers.push_back( center.x( ));
          centers.push_back( center.y( ));
          centers.push_back( center.z( ));

          tangents.push_back( tangent.x( ));
          tangents.push_back( tangent.y( ));
          tangents.push_back( tangent.z( ));
        }

        vNode->Primitive( new GeometricPrimitive( a, b, c, d, e ));
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
    mesh.push_back( geom0->a() );
    mesh.push_back( geom0->b() );
    mesh.push_back( geom1->a() );
    mesh.push_back( geom1->b() );

    //BC
    mesh.push_back( geom0->b() );
    mesh.push_back( geom0->c() );
    mesh.push_back( geom1->b() );
    mesh.push_back( geom1->c() );

    //CD
    mesh.push_back( geom0->c() );
    mesh.push_back( geom0->d() );
    mesh.push_back( geom1->c() );
    mesh.push_back( geom1->d() );

    //DA
    mesh.push_back( geom0->d() );
    mesh.push_back( geom0->a() );
    mesh.push_back( geom1->d() );
    mesh.push_back( geom1->a() );
  }
} // end namespace nlgenerator

// EOF
