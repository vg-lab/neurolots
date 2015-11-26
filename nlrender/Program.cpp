#include "Program.h"
#include "Config.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

// Layout location

// Attribs
// 0 Vertex vec3
// 1 Center vec3
// 2 Tangent vec3

// Uniform
// 0 Projection Matrix4f
// 1 View Matrix4f
// 2 Model Matrix4f

// 3 Color vec3
// 4 Camera Position vec3

// 5 Level Of Tesselation uint
// 6 Alpha Tangent Module float
// 7 Maximun Distance float

namespace neurolots
{

  Program::Program( TProgram type_, const std::string& path_ )
    : _type( type_ )
    , id_( 0 )
    , vshader_( 0 )
    , teshader_( 0 )
    , tcshader_( 0 )
    , gshader_( 0 )
    , fshader_( 0 )
    , _path( path_ )
  {
  }

  Program::~Program( void )
  {
  }

  GLuint Program::_LoadShader( const std::string& fileName_, GLenum type_ )
  {
    std::ifstream file;
    file.open( fileName_.c_str( ), std::ios::in );
    if( !file )
      return 0;

    //Calculo de la longitud del fichero
    file.seekg( 0, std::ios::end );
    unsigned int fileLen = file.tellg( );
    file.seekg( std::ios::beg );
    //Lectura del fichero
    char *source=new char[ fileLen + 1 ];
    int i = 0;
    while( file.good( ))
    {
      source[ i ] = file.get( );
      if( !file.eof( )) i++;
      else fileLen = i;
    }
    source[ fileLen ] = '\0';
    file.close( );

    //Creacion y compilacion del Shader
    GLuint shader;
    shader = glCreateShader( type_ );
    glShaderSource( shader, 1, ( const GLchar ** ) & source,
                    ( const GLint* ) & fileLen );

    glCompileShader( shader );
    delete source;

    //Comprobacion compilacion correcta
    GLint compiled;
    glGetShaderiv( shader, GL_COMPILE_STATUS, & compiled );
    if( !compiled )
    {
      GLint logLen;
      glGetShaderiv( shader, GL_INFO_LOG_LENGTH, & logLen );

      char *logString = new char[ logLen ];
      glGetShaderInfoLog( shader, logLen, NULL, logString );
      std::cout << "Error: " << logString << std::endl;
      delete logString;

      glDeleteShader( shader );
      return 0;
    }
    return shader;
  }

  void Program::_ShaderInit( void )
  {
    if ( !neurolots::nlrender::Config::isInitialized( ))
    {
      throw std::runtime_error( "nlrender has not been initialized" );
      return;
    }
    id_ = glCreateProgram( );

    std::string vname;
    std::string tcname;
    std::string tename;
    std::string gname;
    std::string fname;
    const char* fbVaryings[ ] = { "outValue0", "outValue1" };

    switch ( _type )
    {
    case LINES:
      vname = _path;
      vname.append( "/vshader.glsl" );
      fname = _path;
      fname.append( "/fshader.glsl" );

      std::cout << vname << std::endl;
      std::cout << fname << std::endl;

      vshader_ = _LoadShader( vname, GL_VERTEX_SHADER );
      fshader_ = _LoadShader( fname, GL_FRAGMENT_SHADER );

      glAttachShader( id_, vshader_ );
      glAttachShader( id_, fshader_ );
      break;

    case TRIANGLES:
      vname = _path;
      vname.append( "/vshader.glsl" );
      tcname = _path;
      tcname.append( "/tcshader.glsl" );
      tename = _path;
      tename.append( "/teshader.glsl" );
      fname = _path;
      fname.append( "/fshader.glsl" );

      std::cout << "Triangles shaders: " << std::endl;
      std::cout << "\t" << vname << std::endl;
      std::cout << "\t" << tcname << std::endl;
      std::cout << "\t" << tename << std::endl;
      std::cout << "\t" << fname << std::endl;

      vshader_ = _LoadShader( vname, GL_VERTEX_SHADER );
      tcshader_ = _LoadShader( tcname, GL_TESS_CONTROL_SHADER );
      teshader_ = _LoadShader( tename, GL_TESS_EVALUATION_SHADER );
      fshader_ = _LoadShader( fname, GL_FRAGMENT_SHADER );

      glAttachShader( id_, vshader_ );
      glAttachShader( id_, tcshader_ );
      glAttachShader( id_, teshader_ );
      glAttachShader( id_, fshader_ );
      break;

    case TRIANGLES_FB:
      vname = _path;
      vname.append( "/feedback/vshader.glsl" );
      tcname = _path;
      tcname.append( "/feedback/tcshader.glsl" );
      tename = _path;
      tename.append( "/feedback/teshader.glsl" );
      gname = _path;
      gname.append( "/feedback/gshader.glsl" );

      std::cout << "Triangles shaders feedback: " << std::endl;
      std::cout << "\t" << vname << std::endl;
      std::cout << "\t" << tcname << std::endl;
      std::cout << "\t" << tename << std::endl;
      std::cout << "\t" << gname << std::endl;

      vshader_ = _LoadShader( vname, GL_VERTEX_SHADER );
      tcshader_ = _LoadShader( tcname, GL_TESS_CONTROL_SHADER );
      teshader_ = _LoadShader( tename, GL_TESS_EVALUATION_SHADER );
      gshader_ = _LoadShader( gname, GL_GEOMETRY_SHADER );

      glAttachShader( id_, vshader_ );
      glAttachShader( id_, tcshader_ );
      glAttachShader( id_, teshader_ );
      glAttachShader( id_, gshader_ );

      glTransformFeedbackVaryings( id_, 2, fbVaryings, GL_SEPARATE_ATTRIBS );
      break;

    case QUADS:
      vname = _path;
      vname.append( "/vshader.glsl" );
      tcname = _path;
      tcname.append( "/tcshader.glsl" );
      tename = _path;
      tename.append( "/teshader.glsl" );
      fname = _path;
      fname.append( "/fshader.glsl" );

      std::cout << "Quads shaders: " << std::endl;
      std::cout << "\t" << vname << std::endl;
      std::cout << "\t" << tcname << std::endl;
      std::cout << "\t" << tename << std::endl;
      std::cout << "\t" << fname << std::endl;

      //comopilacion de shaders
      vshader_ = _LoadShader( vname, GL_VERTEX_SHADER );
      tcshader_ = _LoadShader( tcname, GL_TESS_CONTROL_SHADER );
      teshader_ = _LoadShader( tename, GL_TESS_EVALUATION_SHADER );
      fshader_ = _LoadShader( fname, GL_FRAGMENT_SHADER );

      glAttachShader( id_, vshader_ );
      glAttachShader( id_, tcshader_ );
      glAttachShader( id_, teshader_ );
      glAttachShader( id_, fshader_ );
      break;

    case QUADS_FB:
      vname = _path;
      vname.append( "/feedback/vshader.glsl" );
      tcname = _path;
      tcname.append( "/feedback/tcshader.glsl" );
      tename = _path;
      tename.append( "/feedback/teshader.glsl" );
      gname = _path;
      gname.append( "/feedback/gshader.glsl" );

      std::cout << "Quads shaders feedback: " << std::endl;
      std::cout << "\t" << vname << std::endl;
      std::cout << "\t" << tcname << std::endl;
      std::cout << "\t" << tename << std::endl;
      std::cout << "\t" << gname << std::endl;

      //comopilacion de shaders
      vshader_ = _LoadShader( vname, GL_VERTEX_SHADER );
      tcshader_ = _LoadShader( tcname, GL_TESS_CONTROL_SHADER );
      teshader_ = _LoadShader( tename, GL_TESS_EVALUATION_SHADER );
      gshader_ = _LoadShader( gname, GL_GEOMETRY_SHADER );

      glAttachShader( id_, vshader_ );
      glAttachShader( id_, tcshader_ );
      glAttachShader( id_, teshader_ );
      glAttachShader( id_, gshader_ );

      glTransformFeedbackVaryings( id_, 2, fbVaryings, GL_SEPARATE_ATTRIBS );
      break;
    }

    glLinkProgram( id_ );
    //Comprobacion de lincado
    int linked;
    glGetProgramiv( id_, GL_LINK_STATUS, &linked );
    if( !linked )
    {
      GLint logLen;
      glGetProgramiv( id_, GL_INFO_LOG_LENGTH, &logLen );

      char * logString = new char[ logLen ];
      glGetProgramInfoLog( id_, logLen, NULL, logString );
      std::cout << "Error: " << logString << std::endl;
      delete logString;

      glDeleteProgram( id_ );
      id_ = 0;
      return;
    }
  }

  void Program::Init( void )
  {
    _ShaderInit( );
  }

  GLuint Program::id( void )
  {
    return id_;
  }

} // end namespace neurolots

//EOF
