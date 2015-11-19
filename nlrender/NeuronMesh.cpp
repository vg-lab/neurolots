#include "NeuronMesh.h"

using namespace std;

namespace neurolots
{

  NeuronMesh::NeuronMesh( nsol::NeuronMorphologyPtr morpho_ )
    : _morpho( morpho_ )
    , _isInit( false )
  {

  }

  NeuronMesh::~NeuronMesh( void )
  {
  }

  void NeuronMesh::Init( void )
  {
    if( !_isInit )
    {
      std::vector< float > vertices;
      std::vector< float > centers;
      std::vector< float > tangents;
      std::vector< unsigned int > mesh;

      NeuronMeshGenerator::GenerateMesh( _morpho, vertices, centers,
                                         tangents, mesh, _somaEnd );
      // VAO Generation
      glGenVertexArrays( 1, &vao_ );
      glBindVertexArray( vao_ );

      _size = ( unsigned int ) mesh.size( );


      vbo_ = ( GLuint * )malloc( sizeof( GLuint ) * 4 );
      glGenBuffers(4,vbo_);

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[0]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertices.size( ),
                    vertices.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( 0 );

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 1 ]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * centers.size( ),
                    centers.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( 1 );

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 2 ]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * tangents.size( ),
                    tangents.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( 2 );

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo_[ 3 ]);
      glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( int ) * _size,
                    mesh.data( ), GL_STATIC_DRAW );


      glBindVertexArray( 0 );

      vertices.clear( );
      centers.clear( );
      tangents.clear( );
      mesh.clear( );
      _isInit= true;
    }
  }

  void NeuronMesh::Regenerate( const float& alphaRadius_,
                               const std::vector< float >& alphaNeurites_ )
  {
    std::vector< float > vertices;
    std::vector< float > centers;
    std::vector< float > tangents;
    std::vector< unsigned int > mesh;

    NeuronMeshGenerator::GenerateMesh( _morpho, alphaRadius_, alphaNeurites_,
                                       vertices, centers, tangents, mesh,
                                       _somaEnd );
    // VAO Generation
    glBindVertexArray( vao_ );

    _size = ( unsigned int ) mesh.size( );

    glBindBuffer( GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertices.size( ),
                  vertices.data( ), GL_STATIC_DRAW );


    glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 1 ]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * centers.size( ),
                  centers.data( ), GL_STATIC_DRAW );


    glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 2 ]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * tangents.size( ),
                  tangents.data( ), GL_STATIC_DRAW );


    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo_[ 3 ]);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( int ) * _size,
                  mesh.data( ), GL_STATIC_DRAW );


    glBindVertexArray( 0 );

    vertices.clear( );
    centers.clear( );
    tangents.clear( );
    mesh.clear( );
  }

  void NeuronMesh::PaintSoma( void )
  {
    glBindVertexArray(vao_);
    glPatchParameteri( GL_PATCH_VERTICES, 3 );
    glDrawElements( GL_PATCHES, _somaEnd, GL_UNSIGNED_INT, 0 );
  }

  void NeuronMesh::PaintNeurites( void )
  {
    glBindVertexArray(vao_);
    glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawElements( GL_PATCHES,  _size -_somaEnd, GL_UNSIGNED_INT,
                 (void *) (_somaEnd * sizeof(unsigned int)));
  }

} // end namespace neurolots

// EOF
