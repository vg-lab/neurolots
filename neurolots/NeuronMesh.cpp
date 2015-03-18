#include "NeuronMesh.h"

using namespace std;

namespace neurolots
{

  NeuronMesh::NeuronMesh( nsol::NeuronMorphologyPtr morpho_,
                          Program * programTriangles_, Program * programQuads_,
                          Camera * _camera )
    : _morpho( morpho_ )
    , _programTriangles( programTriangles_ )
    , _programQuads( programQuads_ )
    , camera_( _camera )
    , _isInit( false )
  {
    color_.resize( 3 );

    color_[ 0 ]= 0.2;
    color_[ 1 ]= 0.5;
    color_[ 2 ]= 0.7;


    NeuronMeshGenerator::GenerateMeshQuads( _morpho, vertices_, centers_,
                                            tangents_, mesh_, _somaEnd );
  }

  NeuronMesh::~NeuronMesh( void )
  {
  }

  void NeuronMesh::Load( std::string & file_name )
  {
    std::stringstream ss1;
    ss1 << file_name << ".obj";
    std::string objPath = ss1.str( );

    std::stringstream ss2;
    ss2<< file_name << ".mobj";
    std::string mobjPath = ss2.str( );

    ReadOBJ( objPath );
    ReadMOBJ( mobjPath );
  }



  void NeuronMesh::ReadOBJ( std::string & file_name )
  {
    if ( file_name.length() < 5 ||
         file_name.compare( file_name.length( ) - 3, 3, "obj" ))
    {
      cout << file_name << ": Bad format file" << endl;
      exit(1);
    }

    std::ifstream inStream( file_name.c_str( ));
    if ( !inStream.is_open( ))
    {
      cout << file_name << ": Error opennig the file" << endl;
      exit(1);
    }

    std::string line;
    while ( !inStream.eof( ))
    {
      std::getline( inStream, line );
      std::istringstream iss( line );
      std::istringstream iss2( line );
      int a, b, c, d;
      char ch;
      float x, y, z;

      if (( iss >> ch >> a >> b >> c >> d ))
      {
        if ( ch == 'f' )
        {
          mesh_.push_back( a - 1 );
          mesh_.push_back( b - 1 );
          mesh_.push_back( d - 1 );
          mesh_.push_back( c - 1 );
        }
      }
      else
        if (( iss2 >> ch >> x >> y >> z ))
          if ( ch == 'v' )
          {
            vertices_.push_back( x );
            vertices_.push_back( y );
            vertices_.push_back( z );
          }
    }
    inStream.close( );
  }

  void NeuronMesh::ReadMOBJ( std::string &file_name )
  {
    std::vector<Eigen::Vector3d> tangs;
    std::vector<Eigen::Vector3d> cents;

    if ( file_name.length() < 6 ||
         file_name.compare( file_name.length( ) - 4, 4, "mobj" ))
    {
      cout << file_name << ": Bad format file" << endl;
      exit(1);
    }

    std::ifstream inStream( file_name.c_str( ));
    if ( !inStream.is_open( ))
    {
      cout << file_name << ": Error opennig the file" << endl;
      exit(1);
    }
    std::string line;
    while( !inStream.eof( ))
    {
      std::getline( inStream, line );
      std::istringstream iss( line );
      std::istringstream iss2( line );
      int idN;
      char ch;
      float x, y, z;
      if (( iss >> ch >> x >> y >> z ))
      {
        if(  ch == 't' )
        {
          Eigen::Vector3d t( x, y, z );
          tangs.push_back( t );
        }
        if( ch == 'c' )
        {
          Eigen::Vector3d cent( x, y, z );
          cents.push_back( cent );
        }
      }
      else
        if (( iss2 >> ch >> idN ))
          if ( ch == 'i' )
            nodeIndices_.push_back( idN-1 );
    }
    inStream.close();

    for ( unsigned int i = 0; i < nodeIndices_.size( ); i++){
      int id = nodeIndices_[ i ];
      tangents_.push_back( tangs[id].x( ));
      tangents_.push_back( tangs[id].y( ));
      tangents_.push_back( tangs[id].z( ));
      centers_.push_back( cents[id].x( ));
      centers_.push_back( cents[id].y( ));
      centers_.push_back( cents[id].z( ));
    }
  }

  void NeuronMesh::Init( void )
  {
    if( !_isInit )
    {
      // VAO Generation
      glGenVertexArrays( 1, &vao_ );
      glBindVertexArray( vao_ );

      _size = mesh_.size( );


      vbo_ = ( GLuint * )malloc( sizeof( GLuint ) * 4 );
      glGenBuffers(4,vbo_);

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[0]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertices_.size( ),
                    vertices_.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( _programTriangles->inVertex( ), 3, GL_FLOAT, GL_FALSE, 0,
                             0 );
      glEnableVertexAttribArray( _programTriangles->inVertex() );
      glVertexAttribPointer( _programQuads->inVertex( ), 3, GL_FLOAT, GL_FALSE, 0,
                                   0 );
      glEnableVertexAttribArray( _programQuads->inVertex() );

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 1 ]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * centers_.size( ),
                    centers_.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( _programTriangles->inCenter( ), 3, GL_FLOAT, GL_FALSE, 0,
                             0 );
      glEnableVertexAttribArray( _programTriangles->inCenter() );
      glVertexAttribPointer( _programQuads->inCenter( ), 3, GL_FLOAT, GL_FALSE, 0,
                                   0 );
      glEnableVertexAttribArray( _programQuads->inCenter() );

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 2 ]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * tangents_.size( ),
                    tangents_.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( _programQuads->inTangent( ), 3, GL_FLOAT, GL_FALSE, 0,
                             0 );
      glEnableVertexAttribArray( _programQuads->inTangent() );

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo_[ 3 ]);
      glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( int ) * _size,
                    mesh_.data( ), GL_STATIC_DRAW );


      glBindVertexArray( 0 );

      vertices_.clear( );
      centers_.clear( );
      tangents_.clear( );
      _isInit= true;
    }
  }

  void NeuronMesh::Paint(void)
  {



    glBindVertexArray(vao_);

    glUseProgram( _programTriangles->id( ));

    glPatchParameteri( GL_PATCH_VERTICES, 3 );
    glDrawElements( GL_PATCHES, _somaEnd, GL_UNSIGNED_INT, 0 );

//    glDrawRangeElements( GL_TRIANGLES, 0, _somaEnd, _somaEnd, GL_UNSIGNED_INT,
//                             0 );

    glUseProgram( _programQuads->id( ));
    glUniform3fv( _programTriangles->uColor( ), 1, color_.data( ));


    glPatchParameteri( GL_PATCH_VERTICES, 4 );
//    glDrawRangeElements( GL_PATCHES, _somaEnd, _size, _size-_somaEnd, GL_UNSIGNED_INT,
//                         0 );
    glDrawElements( GL_PATCHES,  _size-_somaEnd, GL_UNSIGNED_INT,
                 (void *) (_somaEnd*sizeof(int)));

  }

} // end namespace neurolots

// EOF
