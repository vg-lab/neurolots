#include "NeuronMesh.h"

namespace neurolots
{

  NeuronMesh::NeuronMesh( std::string & file_name, Program * _program,
                          Camera * _camera, Eigen::Vector3d _desp,
                          Eigen::Vector3d _color )
    : program_( _program )
    , camera_( _camera )
  {
    desp_.resize( 3 );

    desp_[ 0 ]= _desp.x( );
    desp_[ 1 ]= _desp.y( );
    desp_[ 2 ]= _desp.z( );

    color_.resize( 3 );

    color_[ 0 ]= _color.x( );
    color_[ 1 ]= _color.y( );
    color_[ 2 ]= _color.z( );

    Load( file_name );
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
      throw file_name + std::string( ": Bad format file" );

    std::ifstream inStream( file_name.c_str( ));
    if ( !inStream.is_open( ))
      throw file_name + std::string( ": Error openning the file" );

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
      throw file_name + std::string( ": Bad format file" );

    std::ifstream inStream( file_name.c_str( ));
    if ( !inStream.is_open( ))
      throw file_name + std::string( ": Error openning the file" );

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

    for ( int i = 0; i < nodeIndices_.size( ); i++){
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
    // VAO Generation
    glGenVertexArrays( 1, &vao_ );
    glBindVertexArray( vao_ );

    //VBOs Generation
    vbo_ = ( GLuint * )malloc( sizeof( GLuint ) * 4 );
    glGenBuffers(4,vbo_);

    glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 0 ]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertices_.size( ),
                  &vertices_[0], GL_STATIC_DRAW );
    glVertexAttribPointer( program_->inVertex( ), 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( program_->inVertex() );

    glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 1 ]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * centers_.size( ),
                  centers_.data( ), GL_STATIC_DRAW );
    glVertexAttribPointer( program_->inCenter( ), 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( program_->inCenter() );

    glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 2 ]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * tangents_.size( ),
                  tangents_.data( ), GL_STATIC_DRAW );
    glVertexAttribPointer( program_->inTangent( ), 3, GL_FLOAT, GL_FALSE, 0,
                           0 );
    glEnableVertexAttribArray( program_->inTangent() );

    size_ = mesh_.size( );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo_[ 3 ]);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( int ) * size_, &mesh_[0],
                  GL_STATIC_DRAW );


    std::cout << mesh_.size() / 4 << " facetas cargadas"<< std::endl;

    glBindVertexArray( 0 );
  }

  void NeuronMesh::Paint(void)
  {
    glUseProgram( program_->id( ));

    glUniform3fv( program_->uCameraPos( ), 1, camera_->GetCameraPos( ));
    glUniform3fv( program_->uColor( ), 1, color_.data( ));
    glUniform3fv( program_->uDesp( ), 1, desp_.data());

    glUniformMatrix4fv( program_->uView( ), 1, GL_FALSE,
                        camera_->GetViewMatrix());

    glUniformMatrix4fv( program_->uProy( ), 1, GL_FALSE,
                        camera_->GetProjectionMatrix( ));

    float lod = program_->lod( );
    float tng = program_->tng( );
    float max = program_->maxDist( );

    glUniform1fv( program_->uLod( ), 1, &lod);
    glUniform1fv( program_->uTng( ), 1, &tng);
    glUniform1fv( program_->uMaxDist( ), 1, &max);

    glBindVertexArray(vao_);

    glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawElements( GL_PATCHES, size_, GL_UNSIGNED_INT, 0 );
  }

  void NeuronMesh::CreateLinesMesh()
  {
    verticesLine_.clear( );
    meshLine_.clear( );

    int size = vertices_.size( );
    for ( int i=0; i < size; i++ )
    {
      verticesLine_.push_back( vertices_[ i ]);
    }
    for ( int i=0; i < size; i++ )
    {
      verticesLine_.push_back( centers_[ i ]);
    }

    size /= 3;
    for ( int i=0; i < size; i++ )
    {
      meshLine_.push_back( i );
      meshLine_.push_back( i + size );
    }

    std::cout << "Numero de lineas " << size << std::endl;
    std::cout << "Numero de indices " << meshLine_.size( ) << std::endl;
  }

} // end namespace neurolots

// EOF
