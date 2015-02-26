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

  NeuronMorphologyPtr NeuronMeshGenerator::ReadMorphology( const char * file_name )
  {
    SwcReader r;
    return r.readFile(file_name);
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

  void NeuronMeshGenerator::GenerateMesh( NeuronMorphologyPtr morpho,
                                          vector< float > & vertices,
                                          vector< float > & centers,
                                          vector< float > & tangents,
                                          vector< unsigned int > & mesh)
  {
    vertices.clear( );
    centers.clear( );
    tangents.clear( );
    mesh.clear( );
    Eigen::Vector3f v( morpho->soma( )->center( ).x( ),
                       morpho->soma( )->center( ).y( ),
                       morpho->soma( )->center( ).z( ));

    Vec3f c = morpho->soma( )->center( );
    Nodes nodes = morpho->soma( )->nodes();
    float radius = 0.0f;
    for( unsigned int i = 0; i < nodes.size(); i++ )
    {
      radius += (nodes[i]->point() - c).norm();
    }
    radius /= nodes.size( );
    _CreateSphere( v, radius, vertices, centers, tangents, mesh );

    //Dendritas

    Neurites neurites = morpho->neurites( );

    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );
      _GenerateMesh(section, vertices, centers, tangents, mesh);
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
      int id1 = vertices.size( )/3;

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

  void NeuronMeshGenerator::_GenerateMesh( SectionPtr section,
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
      _CreateSphere( v, radius, vertices, centers, tangents, mesh );

     segment = segment->next();
     }

     Sections childs = section->children();
     for( unsigned int i = 0; i < childs.size(); i++ )
     {
       _GenerateMesh( childs[i], vertices, centers, tangents, mesh );
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

  void NeuronMeshGenerator::_CreateSphere( Eigen::Vector3f center,
                                           float radius,
                                           vector< float > & vertices,
                                           vector< float > & centers,
                                           vector< float > & tangents,
                                           vector< unsigned int > & mesh )
  {

    Eigen::Vector3f aux = center + Eigen::Vector3f( -0.5 * radius, 0.5 * radius, 0.5 * radius );
    int id0 = vertices.size( ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));
    aux = center + Eigen::Vector3f( -0.5 * radius, -0.5 * radius, 0.5 * radius);
    int id1 = vertices.size( ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));
    aux = center + Eigen::Vector3f( 0.5 * radius, -0.5 * radius, 0.5 * radius );
    int id2 = vertices.size( ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));
    aux = center + Eigen::Vector3f( 0.5 * radius, 0.5 * radius, 0.5 * radius );
    int id3 = vertices.size( ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    aux = center + Eigen::Vector3f( -0.5 * radius, 0.5 * radius, -0.5 * radius );
    int id4 = vertices.size( ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));
    aux = center + Eigen::Vector3f( -0.5 * radius, -0.5 * radius, -0.5 * radius );
    int id5 = vertices.size( ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));
    aux = center + Eigen::Vector3f( 0.5 * radius, -0.5 * radius, -0.5 * radius );
    int id6 = vertices.size( ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));
    aux = center + Eigen::Vector3f( 0.5 * radius, 0.5 * radius, -0.5 * radius );
    int id7 = vertices.size( ) / 3;
    vertices.push_back( aux.x( )); vertices.push_back( aux.y( )); vertices.push_back( aux.z( ));

    for( int i = 0; i < 8; i++ )
    {
      centers.push_back( center.x( ));
      centers.push_back( center.y( ));
      centers.push_back( center.z( ));

      tangents.push_back( 0.0f );
      tangents.push_back( 0.0f );
      tangents.push_back( 0.0f );
    }

    mesh.push_back(id0); mesh.push_back(id1); mesh.push_back(id3); mesh.push_back(id2);

    mesh.push_back(id4); mesh.push_back(id0); mesh.push_back(id7); mesh.push_back(id3);

    mesh.push_back(id1); mesh.push_back(id5); mesh.push_back(id2); mesh.push_back(id6);

    mesh.push_back(id5); mesh.push_back(id4); mesh.push_back(id6); mesh.push_back(id7);

    mesh.push_back(id3); mesh.push_back(id2); mesh.push_back(id7); mesh.push_back(id6);

    mesh.push_back(id4); mesh.push_back(id5); mesh.push_back(id0); mesh.push_back(id1);


  }

} // end namespace neurolots

// EOF
