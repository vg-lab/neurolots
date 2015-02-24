#include "NeuronMeshGenerator.h"

using namespace nsol;
using namespace std;

namespace neurolots
{

//CONSTRUCTORS AND DESTRUCTORS

  NeuronMeshGenerator::NeuronMeshGenerator( const char * file_name )
  {
    SwcReader r;
    _morfo = r.readFile(file_name);
    _numIds = _MaxId();
    _ids.resize(_numIds);


//    Vec3f center = morfo_->soma()->center();
//    cout << "**********Centro del soma x: " << center.x( ) << " y: "
//         << center.y( ) << " z: " << center.z( ) << "\n" << "Radio "
//         << morfo_->soma()->maxRadius() << endl;


//    cout << "Numero de nodos : " << _NumNodes( ) <<  endl;
//    cout << "Mayor Id de nodos: " << _numIds <<endl;

  }

  NeuronMeshGenerator::~NeuronMeshGenerator( void )
  {

  }

// PUBLIC METHODS

  void NeuronMeshGenerator::GenerateStructure( void )
  {
    _vertices.clear();
    _mesh.clear();

    Neurites neurites = _morfo->neurites( );
    NodePtr node = _morfo->soma( )->nodes( )[0];

    _ids[ node->id()-1 ] = 0;

    Vec3f pos = node->point( );
    _vertices.push_back( pos.x( ));
    _vertices.push_back( pos.y( ));
    _vertices.push_back( pos.z( ));

    for( unsigned int i = 0; i < neurites.size( ); i++ )
    {
      SectionPtr section = neurites[i]->firstSection( );
      _GenerateStructure(section);
    }

  }

  void NeuronMeshGenerator::GenerateMesh( void )
  {

  }

  vector< float > NeuronMeshGenerator::Vertices( void )
  {
    return _vertices;
  }

  vector< unsigned int > NeuronMeshGenerator::Mesh( void )
  {
    return _mesh;
  }

// PRIVATE METHODS

  void NeuronMeshGenerator::_GenerateStructure( nsol::SectionPtr section )
  {
    SegmentPtr segment = section->firstSegment( );

    while( segment != nullptr)
    {
      int id0 = _ids[ segment->begin( )->id( )-1 ];
      int id1 = _vertices.size( )/3;

      NodePtr node = segment->end( );

      _ids[ node->id( )-1 ] = id1;

      Vec3f pos = node->point( );
      _vertices.push_back( pos.x( ));
      _vertices.push_back( pos.y( ));
      _vertices.push_back( pos.z( ));

      _mesh.push_back( id0 );
      _mesh.push_back( id1 );

      segment = segment->next();
    }

    Sections childs = section->children();
    for( unsigned int i = 0; i < childs.size(); i++ )
    {
      _GenerateStructure( childs[i] );

    }

  }

  void NeuronMeshGenerator::_GenerateMesh( nsol::SectionPtr section )
  {

  }

  unsigned int NeuronMeshGenerator::_NumNodes( void )
  {
    unsigned int numNodes = 0;
    Neurites neurites = _morfo->neurites( );
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

  unsigned int NeuronMeshGenerator::_MaxId( void )
  {
    unsigned int maxId = 0;
    Neurites neurites = _morfo->neurites( );
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

} // end namespace neurolots

// EOF
