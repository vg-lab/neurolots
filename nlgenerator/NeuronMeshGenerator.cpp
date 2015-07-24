#include "NeuronMeshGenerator.h"

using namespace nsol;
using namespace std;

namespace neurolots
{

//CONSTRUCTORS AND DESTRUCTORS

  NeuronMeshGenerator::NeuronMeshGenerator( void )
  {

  }

  NeuronMeshGenerator::~NeuronMeshGenerator( void )
  {

  }

// PUBLIC METHODS

  NeuronMorphologyPtr NeuronMeshGenerator::ReadMorphology( const char *
                                                           file_name )
  {
    SwcReader r;
    return r.readMorphology(file_name);
  }

  void NeuronMeshGenerator::GenerateStructure( NeuronMorphologyPtr morpho,
                                               vector< float > & vertices,
                                               vector< unsigned int > & mesh )
  {
    vector< unsigned int >ids;
    ids.resize( _MaxId( morpho ));
    vertices.clear( );
    mesh.clear( );

    Neurites neurites = morpho->neurites( );
    NodePtr node = morpho->soma( )->nodes( )[0];

    ids[ node->id()-1 ] = 0;

    Vec3f pos = node->point( );
    vertices.push_back( pos.x( ));
    vertices.push_back( pos.y( ));
    vertices.push_back( pos.z( ));

    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );
      _GenerateStructure(section, vertices, mesh, ids);
    }

  }

  void NeuronMeshGenerator::GenerateStructure( NeuronMorphologyPtr morpho,
                                      std::vector< VectorizedNodePtr > & vNodes,
                                      vector< float > & vertices,
                                      vector< unsigned int > & mesh )
  {
    vertices.clear( );
    mesh.clear( );

    Neurites neurites = morpho->neurites( );
    NodePtr node = morpho->soma( )->nodes( )[0];

    VectorizedNodePtr vNode = vNodes[ node->id() - 1 ];

    Eigen::Vector3f pos = vNode->Position( );

    vNode->Primitive( new GeometricPrimitive( int( vertices.size( )) / 3 ));

    vertices.push_back( pos.x( ));
    vertices.push_back( pos.y( ));
    vertices.push_back( pos.z( ));



    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );
      _GenerateStructure(section, vNodes,vertices, mesh);
    }
  }

  void NeuronMeshGenerator::GenerateStructure(
                                        vector< VectorizedNodePtr > & vNodes,
                                        vector< float > & vertices,
                                        vector< unsigned int > & mesh )
    {
      vertices.clear( );
      mesh.clear( );

      for( unsigned int i=0; i < vNodes.size( ); i++ )
      {

        VectorizedNodePtr vNode = vNodes[i];

        int id1 = int( vertices.size( )) / 3;

        vNode->Primitive( new GeometricPrimitive( id1 ));
        Eigen::Vector3f pos = vNode->Position( );
        vertices.push_back( pos.x( ));
        vertices.push_back( pos.y( ));
        vertices.push_back( pos.z( ));


        if( vNode->Father() != nullptr )
        {
          int id0 = vNode->Father()->Primitive()->A() ;
          mesh.push_back( id0 );
          mesh.push_back( id1 );
        }

      }
    }

  void NeuronMeshGenerator::GenerateMeshQuads( NeuronMorphologyPtr morpho,
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

    VectorizeMorpho( morpho, vNodes, firstNodes );

    GenerateSomaTriangles( morpho->soma( ), firstNodes, vertices, centers,
                           tangents, mesh  );

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

  void NeuronMeshGenerator::GenerateMeshQuads(
                                        vector< VectorizedNodePtr > & vNodes,
                                        vector< float > & vertices,
                                        vector< float > & centers,
                                        vector< float > & tangents,
                                        vector< unsigned int > & mesh )
  {
    vertices.clear( );
    centers.clear( );
    tangents.clear( );
    mesh.clear( );

    CalculateTangents( vNodes );
    CalculateGeometry( vNodes, vertices, centers, tangents);

    for ( unsigned int i = 0; i < vNodes.size(); i++ )
    {
      VectorizedNodePtr vNode = vNodes[i];
      if( vNode->Father( ) != nullptr )
      {
        _CreateQuadPipe( vNode->Father( )->Primitive( ), vNode->Primitive(), mesh );
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

  void NeuronMeshGenerator::GenerateMeshTriangles( NeuronMorphologyPtr morpho,
                                           vector< float > & vertices,
                                           vector< float > & centers,
                                           std::vector< float >& tangents,
                                           vector< unsigned int > & mesh)
   {
     vertices.clear( );
     centers.clear( );
     tangents.clear( );
     mesh.clear( );

     std::vector< VectorizedNodePtr > vNodes;
     std::vector< VectorizedNodePtr > firstNodes;

     VectorizeMorpho( morpho, vNodes, firstNodes );

     GenerateSomaTriangles( morpho->soma( ), firstNodes, vertices, centers,
                            tangents, mesh  );

     CalculateTangents( vNodes );
     CalculateGeometry( vNodes, vertices, centers, tangents);

     for ( unsigned int i = 0; i < vNodes.size(); i++ )
     {
       VectorizedNodePtr vNode = vNodes[i];
       if( vNode->Father( ) != nullptr )
       {
         _CreateTrianglePipe( vNode->Father( )->Primitive( ),
                              vNode->Primitive(), mesh );
       }
     }
   }

  void NeuronMeshGenerator::GenerateMeshTriangles(
                                          vector< VectorizedNodePtr > & vNodes,
                                          vector< float > & vertices,
                                          vector< float > & centers,
                                          std::vector< float >& tangents,
                                          vector< unsigned int > & mesh)
    {
      CalculateTangents( vNodes );
      CalculateGeometry( vNodes, vertices, centers, tangents);

      for ( unsigned int i = 0; i < vNodes.size(); i++ )
      {
        VectorizedNodePtr vNode = vNodes[i];
        if( vNode->Father( ) != nullptr )
        {
          _CreateTrianglePipe( vNode->Father( )->Primitive( ),
                               vNode->Primitive(), mesh );
        }
//        _CreateSphereTriangles(pos,radius,vertices,centers,mesh);//BORRAR
      }

  }

  void NeuronMeshGenerator::GenerateSomaTriangles( nsol::SomaPtr soma,
                                 std::vector< VectorizedNodePtr > & firstNodes,
                                 std::vector< float >& vertices,
                                 std::vector< float >& centers,
                                 std::vector< float >& tangents,
                                 std::vector< unsigned int >& mesh )
  {
    Vec3f c = soma->center( );
    Eigen::Vector3f center( c.x( ), c.y( ), c.z( ) );

    Nodes nodes = soma->nodes( );
    float radius = 0.0f;
    for( unsigned int i = 0; i < nodes.size(); i++ )
    {
      radius += (nodes[i]->point() - c).norm();
    }
    radius /= nodes.size( );

    if( radius == 0.0f )
    {
      radius = soma->maxRadius( );
    }

    Icosphere ico( center, radius, 3 );

    ico.CalculateSoma( firstNodes );

    ico.PassContornTrianglesToVector( firstNodes, vertices, centers, tangents,
                                      mesh );
  }

  void NeuronMeshGenerator::GenerateSomaQuads( nsol::SomaPtr soma,
   		                 std::vector< VectorizedNodePtr > & /* firstNodes */,
                                 std::vector< float >& vertices,
                                 std::vector< float >& centers,
                                 std::vector< float >& tangents,
                                 std::vector< unsigned int >& mesh )
    {
      Vec3f c = soma->center( );
      Eigen::Vector3f center( c.x( ), c.y( ), c.z( ) );

      Nodes nodes = soma->nodes( );
      float radius = 0.0f;
      for( unsigned int i = 0; i < nodes.size(); i++ )
      {
        radius += (nodes[i]->point() - c).norm();
      }
      radius /= nodes.size( );

      if( radius == 0.0f )
      {
        radius = soma->maxRadius( );
      }

      Icosphere ico( center, radius, 3 );
      ico.PassQuadsToVector( vertices, centers, tangents, mesh );
    }

  void NeuronMeshGenerator::VectorizeMorpho( NeuronMorphologyPtr morpho,
                                           vector< VectorizedNodePtr > & vNodes)
  {
    vNodes.clear( );
    Neurites neurites = morpho->neurites( );

    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );


      //////////////////////////////////////////////////////
      ///Hack Delete when nsol loaders are rigth
      section->firstSegment( section->firstSegment( )->next( ) );
      //////////////////////////////////////////////////////

      VectorizedNodePtr vNode = new VectorizedNode( );
      NodePtr node = section->firstSegment( )->begin( );
      Vec3f pos = node->point( );
      vNode->Position( Eigen::Vector3f( pos.x(), pos.y(), pos.z( )));
      vNode->Radius( node->radius( ));
      vNode->Id( int( vNodes.size( )));

//      std::cout << "Nodo " << node->id() << "\nPosicion: " << node->point().x()
//                << " " << node->point().y() << " " << node->point().z()
//                << std::endl;



      vNodes.push_back( vNode );

      _VectorizeMorpho(section, vNodes, vNode);
    }
  }

  void NeuronMeshGenerator::VectorizeMorpho( NeuronMorphologyPtr morpho,
                                       vector< VectorizedNodePtr >& vNodes,
                                       vector< VectorizedNodePtr >& firstNodes )
  {
    vNodes.clear( );
    firstNodes.clear( );
    Neurites neurites = morpho->neurites( );

    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );

      //////////////////////////////////////////////////////
      ///Hack Delete when nsol loaders are rigth
      section->firstSegment( section->firstSegment( )->next( ) );
      //////////////////////////////////////////////////////

      VectorizedNodePtr vNode = new VectorizedNode( );
      NodePtr node = section->firstSegment( )->begin( );
      Vec3f pos = node->point( );
      vNode->Position( Eigen::Vector3f( pos.x(), pos.y(), pos.z( )));
      vNode->Radius( node->radius( ));
      vNode->Id( int( vNodes.size( )));

//      std::cout << "Nodo " << node->id() << "\nPosicion: " << node->point().x()
//                << " " << node->point().y() << " " << node->point().z()
//                << std::endl;



      vNodes.push_back( vNode );
      firstNodes.push_back( vNode );

      _VectorizeMorpho(section, vNodes, vNode);
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
          tangent = ( vNode->Position( ) - vNodeFather->Position( )).normalized( );
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
//            tangent += ( childNodes[j]->Position( ) - vNode->Position( ) ).normalized( );
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
//        std::cout << " indice de vertice  " << vecId << std::endl;
//        std::cout << " tamaño de vertices  " << vertices.size() << std::endl;
//        std::cout << " componente x del vertice  " << vertices[vecId] << std::endl;

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

//        std::cout << "va: " << va.x() << " " << va.y() << " " << va.z() << std::endl;
//        std::cout << "vb: " << vb.x() << " " << vb.y() << " " << vb.z() << std::endl;
//        std::cout << "vc: " << vc.x() << " " << vc.y() << " " << vc.z() << std::endl;
//        std::cout << "vd: " << vd.x() << " " << vd.y() << " " << vd.z() << std::endl;

        int a = int( vertices.size( )) / 3;
        position = q._transformVector( va ) * vNode->Radius( ) + center;
//        std::cout << "position: " << position.x() << " " << position.y() << " " << position.z() << std::endl;
        vertices.push_back( position.x( ));
        vertices.push_back( position.y( ));
        vertices.push_back( position.z( ));

        int b = int( vertices.size( )) / 3;
        position = q._transformVector( vb ) * vNode->Radius( ) + center;
//        std::cout << "position: " << position.x() << " " << position.y() << " " << position.z() << std::endl;
        vertices.push_back( position.x( ));
        vertices.push_back( position.y( ));
        vertices.push_back( position.z( ));

        int c = int( vertices.size( ) ) / 3;
        position = q._transformVector( vc ) * vNode->Radius( ) + center;
//        std::cout << "position: " << position.x() << " " << position.y() << " " << position.z() << std::endl;
        vertices.push_back( position.x( ));
        vertices.push_back( position.y( ));
        vertices.push_back( position.z( ));

        int d = int( vertices.size( ) ) / 3;
        position = q._transformVector( vd ) * vNode->Radius( ) + center;
//        std::cout << "position: " << position.x() << " " << position.y() << " " << position.z() << std::endl;
        vertices.push_back( position.x( ));
        vertices.push_back( position.y( ));
        vertices.push_back( position.z( ));

//        std::getchar();

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

  void NeuronMeshGenerator::_GenerateStructure( nsol::SectionPtr section,
                                                vector< float > & vertices,
                                                vector< unsigned int > & mesh,
                                                vector< unsigned int > & ids )
  {
    SegmentPtr segment = section->firstSegment( );

    while( segment != nullptr)
    {
      int id0 = ids[ segment->begin( )->id( )-1 ];
      int id1 = int( vertices.size( ) ) / 3;

      NodePtr node = segment->end( );

      ids[ node->id( )-1 ] = id1;

      Vec3f pos = node->point( );
      vertices.push_back( pos.x( ));
      vertices.push_back( pos.y( ));
      vertices.push_back( pos.z( ));

      mesh.push_back( id0 );
      mesh.push_back( id1 );

      segment = segment->next();
    }

    Sections childs = section->children();
    for( unsigned int i = 0; i < childs.size(); i++ )
    {
      _GenerateStructure( childs[i], vertices, mesh, ids );

    }

  }

  void NeuronMeshGenerator::_GenerateStructure( nsol::SectionPtr section,
                                           vector< VectorizedNodePtr > & vNodes,
                                           vector< float > & vertices,
                                           vector< unsigned int > & mesh )
    {
      SegmentPtr segment = section->firstSegment( );

      while( segment != nullptr)
      {
        int id0 = vNodes[ segment->begin()->id( ) - 1 ]->Primitive( )->A();

        int id1 = int( vertices.size( ) ) / 3;

        NodePtr node = segment->end( );

        VectorizedNodePtr vNode = vNodes[ node->id() - 1 ];

        Eigen::Vector3f pos = vNode->Position( );

        vNode->Primitive( new GeometricPrimitive( id1 ));

        vertices.push_back( pos.x( ));
        vertices.push_back( pos.y( ));
        vertices.push_back( pos.z( ));

        mesh.push_back( id0 );
        mesh.push_back( id1 );

        segment = segment->next();
      }

      Sections childs = section->children();
      for( unsigned int i = 0; i < childs.size(); i++ )
      {
        _GenerateStructure( childs[i], vNodes, vertices, mesh );

      }

    }

  void NeuronMeshGenerator::_GenerateMeshQuads( SectionPtr section,
                                           vector< float > & vertices,
                                           vector< float > & centers,
                                           vector< float > & tangents,
                                           vector< unsigned int > & mesh )
  {
    SegmentPtr segment = section->firstSegment( );

    while( segment != nullptr)
    {
      NodePtr node = segment->end( );

      Eigen::Vector3f v( node->point( ).x( ),
                         node->point( ).y( ),
                         node->point( ).z( ));
      float radius = node->radius( );
      _CreateSphereQuads( v, radius, vertices, centers, tangents, mesh );

     segment = segment->next();
     }

     Sections childs = section->children();
     for( unsigned int i = 0; i < childs.size(); i++ )
     {
       _GenerateMeshQuads( childs[i], vertices, centers, tangents, mesh );
     }
  }

  void NeuronMeshGenerator::_GenerateMeshTriangles( SectionPtr section,
                                             vector< float > & vertices,
                                             vector< float > & centers,
                                             vector< float >& tangents,
                                             vector< unsigned int > & mesh )
    {
      SegmentPtr segment = section->firstSegment( );

      while( segment != nullptr)
      {
        NodePtr node = segment->end( );

        Eigen::Vector3f v( node->point( ).x( ),
                           node->point( ).y( ),
                           node->point( ).z( ));
        float radius = node->radius( );
        _CreateSphereTriangles( v, radius, vertices, centers, tangents, mesh );

       segment = segment->next();
       }

       Sections childs = section->children();
       for( unsigned int i = 0; i < childs.size(); i++ )
       {
         _GenerateMeshTriangles( childs[i], vertices, centers, tangents, mesh );
       }
    }

  void NeuronMeshGenerator::_VectorizeMorpho( SectionPtr section,
                                          vector< VectorizedNodePtr > & vNodes,
                                          VectorizedNodePtr vFatherNode )
  {
    SegmentPtr segment = section->firstSegment( );

    while( segment != nullptr)
    {
      VectorizedNodePtr vNode = new VectorizedNode( );

      NodePtr node = segment->end( );
      Vec3f pos = node->point( );

//      std::cout << "Nodo " << node->id() << "\nPosicion: " << node->point().x()
//                      << " " << node->point().y() << " " << node->point().z()
//                      << std::endl;

      vNode->Position( Eigen::Vector3f( pos.x(), pos.y(), pos.z( )));
      vNode->Radius( node->radius( ));
      vNode->Id( int( vNodes.size( )));
      vNode->Father( vFatherNode);
      vNodes.push_back( vNode );

      vFatherNode->AddChild( vNode );
      vFatherNode = vNode;
      segment = segment->next();
    }

    Sections childSections = section->children();
    for( unsigned int i = 0; i < childSections.size(); i++ )
    {

      _VectorizeMorpho( childSections[i], vNodes, vFatherNode );

    }
  }

  unsigned int NeuronMeshGenerator::_NumNodes( NeuronMorphologyPtr morpho )
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

  unsigned int NeuronMeshGenerator::_NumNodes( nsol::SectionPtr section )
  {
      unsigned int numNodes = 0;
      SegmentPtr segment = section->firstSegment( );

      while( segment != nullptr)
      {
        numNodes++;
        segment = segment->next();
      }

      Sections childs = section->children();

      for( unsigned int i = 0; i < childs.size(); i++ )
      {
        numNodes += _NumNodes( childs[i] );

      }

      return numNodes;
  }

  unsigned int NeuronMeshGenerator::_MaxId( nsol::NeuronMorphologyPtr morpho )
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

  unsigned int NeuronMeshGenerator::_MaxId( nsol::SectionPtr section )
  {
      unsigned int maxId = 0;
      SegmentPtr segment = section->firstSegment( );

      while( segment != nullptr)
      {
        unsigned int id = segment->end( )->id( );
        if( id > maxId )
          maxId = id;
        segment = segment->next();
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

  void NeuronMeshGenerator::_CreateSphereQuads( Eigen::Vector3f center,
                                           float radius,
                                           vector< float > & vertices,
                                           vector< float > & centers,
                                           vector< float > & tangents,
                                           vector< unsigned int > & mesh )
  {

    Eigen::Vector3f aux = center + Eigen::Vector3f( 0.0, 0.0, radius );
    int id0 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( 0.0, 0.0, -radius );
    int id1 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( radius, 0.0, 0.0 );
    int id2 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( -radius, 0.0, 0.0 );
    int id3 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( 0.0, radius, 0.0 );
    int id4 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( 0.0, -radius, 0.0 );
    int id5 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    for( int i = 0; i < 6; i++ )
    {
      centers.push_back( center.x( ));
      centers.push_back( center.y( ));
      centers.push_back( center.z( ));

      tangents.push_back( 0.0f );
      tangents.push_back( 0.0f );
      tangents.push_back( 0.0f );
    }

    mesh.push_back(id4); mesh.push_back(id0); mesh.push_back(id2);
    mesh.push_back(id5);

    mesh.push_back(id4); mesh.push_back(id3); mesh.push_back(id0);
    mesh.push_back(id5);

    mesh.push_back(id4); mesh.push_back(id1); mesh.push_back(id3);
    mesh.push_back(id5);

    mesh.push_back(id4); mesh.push_back(id2); mesh.push_back(id1);
    mesh.push_back(id5);


  }

  void NeuronMeshGenerator::_CreateSphereTriangles( Eigen::Vector3f center,
                                                float radius,
                                                vector< float > & vertices,
                                                vector< float > & centers,
                                                vector< float >& tangents,
                                                vector< unsigned int > & mesh )
  {

    Eigen::Vector3f aux = center + Eigen::Vector3f( 0.0, 0.0, radius );
    int id0 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( 0.0, 0.0, -radius );
    int id1 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( radius, 0.0, 0.0 );
    int id2 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( -radius, 0.0, 0.0 );
    int id3 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( 0.0, radius, 0.0 );
    int id4 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( 0.0, -radius, 0.0 );
    int id5 = int( vertices.size( ) ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    for( int i = 0; i < 6; i++ )
    {
      centers.push_back( center.x( ));
      centers.push_back( center.y( ));
      centers.push_back( center.z( ));

      tangents.push_back( 0.0f );
      tangents.push_back( 0.0f );
      tangents.push_back( 0.0f );
    }

    mesh.push_back(id4); mesh.push_back(id0); mesh.push_back(id2);
    mesh.push_back(id5); mesh.push_back(id2); mesh.push_back(id0);

    mesh.push_back(id4); mesh.push_back(id3); mesh.push_back(id0);
    mesh.push_back(id5); mesh.push_back(id0); mesh.push_back(id3);

    mesh.push_back(id4); mesh.push_back(id1); mesh.push_back(id3);
    mesh.push_back(id5); mesh.push_back(id3); mesh.push_back(id1);

    mesh.push_back(id4); mesh.push_back(id2); mesh.push_back(id1);
    mesh.push_back(id5); mesh.push_back(id1); mesh.push_back(id2);

  }

  void NeuronMeshGenerator::_CreateTrianglePipe( GeometricPrimitivePtr geom0,
                           GeometricPrimitivePtr geom1,
                           vector< unsigned int > & mesh )
  {
    //AB
    mesh.push_back( geom0->A() );
    mesh.push_back( geom0->B() );
    mesh.push_back( geom1->A() );

    mesh.push_back( geom0->B() );
    mesh.push_back( geom1->B() );
    mesh.push_back( geom1->A() );

    //BC
    mesh.push_back( geom0->B() );
    mesh.push_back( geom0->C() );
    mesh.push_back( geom1->B() );

    mesh.push_back( geom0->C() );
    mesh.push_back( geom1->C() );
    mesh.push_back( geom1->B() );

    //CD
    mesh.push_back( geom0->C() );
    mesh.push_back( geom0->D() );
    mesh.push_back( geom1->C() );

    mesh.push_back( geom0->D() );
    mesh.push_back( geom1->D() );
    mesh.push_back( geom1->C() );

    //DA
    mesh.push_back( geom0->D() );
    mesh.push_back( geom0->A() );
    mesh.push_back( geom1->D() );

    mesh.push_back( geom0->A() );
    mesh.push_back( geom1->A() );
    mesh.push_back( geom1->D() );
  }

  void NeuronMeshGenerator::_CreateQuadPipe( GeometricPrimitivePtr geom0,
                             GeometricPrimitivePtr geom1,
                             vector< unsigned int > & mesh )
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
} // end namespace neurolots

// EOF
