#include "Program.h"
#include "Config.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

//OpenGL
#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
#include <GL/glew.h>
#endif
#ifdef Darwin
#include <gl.h>
#include <glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif


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
    , vshader_( nullptr )
    , teshader_( nullptr )
    , tcshader_( nullptr )
    , gshader_( nullptr )
    , fshader_( nullptr )
    , _path( path_ )
  {
  }

  Program::~Program( void )
  {

  }

  void Program::Init( void )
  {
    if ( !neurolots::nlrender::Config::isInitialized( ))
    {
      throw std::runtime_error( "nlrender has not been initialized" );
      return;
    }

    id_ = glCreateProgram( );

    std::string vsource;
    std::string tcsource;
    std::string tesource;
    std::string fsource;

    const char* fbVaryings[ ] = { "outValue0", "outValue1" };

    if ( _path.size( ) > 0 )  // Shaders from NEUROLOTS_SHADERS_PATH
    {
      vsource =  _LoadShader( std::string( _path + "/vshader.glsl" ));
      tcsource = _LoadShader( std::string( _path + "/tcshader.glsl" ));
      tesource = _LoadShader( std::string( _path + "/teshader.glsl" ));

      if( _type == TRIANGLES || _type == QUADS )
      {
        fsource = _LoadShader( std::string( _path + "/fshader.glsl" ));
      }
      else
      {
        fsource = _LoadShader( std::string( _path + "/gshader.glsl" ));
      }
    }
    else // Sahders from source code
    {
      if( _type == TRIANGLES || _type == TRIANGLES_FB )
      {
        vsource = Shader::sourceCode( Shader::TRIANGLES_VERTEX );
        tcsource = Shader::sourceCode( Shader::TRIANGLES_TESS_CONTROL );
        tesource = Shader::sourceCode( Shader::TRIANGLES_TESS_EVALUATION );
        if( _type == TRIANGLES )
        {
          fsource = Shader::sourceCode( Shader::TRIANGLES_FRAGMENT );
        }
        else
        {
          fsource = Shader::sourceCode( Shader::TRIANGLES_GEOMETRY );
        }
      }
      else
      {
        vsource = Shader::sourceCode( Shader::QUADS_VERTEX );
        tcsource = Shader::sourceCode( Shader::QUADS_TESS_CONTROL );
        tesource = Shader::sourceCode( Shader::QUADS_TESS_EVALUATION );
        if( _type == QUADS )
        {
          fsource = Shader::sourceCode( Shader::QUADS_FRAGMENT );
        }
        else
        {
          fsource = Shader::sourceCode( Shader::QUADS_GEOMETRY );
        }
      }
    }

    vshader_ = new Shader( GL_VERTEX_SHADER, vsource );
    tcshader_ = new Shader( GL_TESS_CONTROL_SHADER, tcsource );
    teshader_ = new Shader( GL_TESS_EVALUATION_SHADER, tesource );

    if( _type == TRIANGLES || _type == QUADS )
    {
      fshader_ = new Shader( GL_FRAGMENT_SHADER, fsource );
    }
    else
    {
      fshader_ = new Shader( GL_GEOMETRY_SHADER, fsource );
    }

    glAttachShader( id_, vshader_->id( ));
    glAttachShader( id_, tcshader_->id( ));
    glAttachShader( id_, teshader_->id( ));
    glAttachShader( id_, fshader_->id( ));

    if( _type == TRIANGLES_FB || _type == QUADS_FB )
      glTransformFeedbackVaryings( id_, 2, fbVaryings, GL_SEPARATE_ATTRIBS );

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

  unsigned int& Program::id( void )
  {
    return id_;
  }

  std::string Program::_LoadShader( const std::string& fileName_ )
  {
    std::string source;

    std::ifstream file;
    file.open( fileName_.c_str( ), std::ios::in );
    if( !file )
      return source;

    std::string line;
    while( std::getline( file, line ))
    {
      source.append( line );
      source.append( "\n" );
    }
    file.close( );

    return source;
  }

} // end namespace neurolots

//EOF
