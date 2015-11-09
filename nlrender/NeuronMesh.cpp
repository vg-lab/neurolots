#include "NeuronMesh.h"

using namespace std;

namespace neurolots
{

  NeuronMesh::NeuronMesh( nsol::NeuronMorphologyPtr morpho_,
                          Program * programTriangles_, Program * programQuads_ )
    : _morpho( morpho_ )
    , _programTriangles( programTriangles_ )
    , _programQuads( programQuads_ )
    , _isInit( false )
    , _paintSoma( true )
    , _paintNeurites( true )
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
      glVertexAttribPointer( _programTriangles->inVertex( ), 3, GL_FLOAT,
                             GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( _programTriangles->inVertex() );
      glVertexAttribPointer( _programQuads->inVertex( ), 3, GL_FLOAT,
                             GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( _programQuads->inVertex() );


      glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 1 ]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * centers.size( ),
                    centers.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( _programTriangles->inCenter( ), 3, GL_FLOAT,
                             GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( _programTriangles->inCenter() );
      glVertexAttribPointer( _programQuads->inCenter( ), 3, GL_FLOAT,
                             GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( _programQuads->inCenter() );


      glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 2 ]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * tangents.size( ),
                    tangents.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( _programQuads->inTangent( ), 3, GL_FLOAT,
                             GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( _programQuads->inTangent() );


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

  void NeuronMesh::Paint(void)
  {



    glBindVertexArray(vao_);

    if( _paintSoma )
    {
      glUseProgram( _programTriangles->id( ));

      glPatchParameteri( GL_PATCH_VERTICES, 3 );
      glDrawElements( GL_PATCHES, _somaEnd, GL_UNSIGNED_INT, 0 );
    }

    if( _paintNeurites )
    {
      glUseProgram( _programQuads->id( ));


      glPatchParameteri( GL_PATCH_VERTICES, 4 );
      glDrawElements( GL_PATCHES,  _size -_somaEnd, GL_UNSIGNED_INT,
                 (void *) (_somaEnd * sizeof(unsigned int)));
    }
  }

  bool NeuronMesh::PaintSoma( void )
  {
    return _paintSoma;
  }

  bool NeuronMesh::PaintNeurites( void )
  {
    return _paintNeurites;
  }

  void NeuronMesh::PaintSoma( bool paintSoma_ )
  {
    _paintSoma = paintSoma_;
  }

  void NeuronMesh::PaintNeurites( bool paintNeurites_ )
  {
    _paintNeurites = paintNeurites_;
  }

} // end namespace neurolots

// EOF
