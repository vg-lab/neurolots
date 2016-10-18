/**
 * @file    Program.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

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

// 8 Tessellation dist rubroutine

namespace nlrender
{

  Program::Program( TProgram type_ )
    : _type( type_ )
    , _id( 0 )
    , _vshader( nullptr )
    , _teshader( nullptr )
    , _tcshader( nullptr )
    , _gshader( nullptr )
    , _fshader( nullptr )
    , _projMat( 0 )
    , _viewMat( 0 )
    , _modelMat( 0 )
    , _colorVec( 0 )
    , _cameraPos( 0 )
    , _levelTess( 0 )
    , _alphaTangentMod( 0 )
    , _maxDistance( 0 )
  {

  }

  Program::~Program( void )
  {

  }

  void Program::init( void )
  {
    if ( !nlrender::Config::isInitialized( ))
    {
      throw std::runtime_error( "nlrender has not been initialized" );
      return;
    }

    _id = glCreateProgram( );

    switch( _type )
    {
    case TRIANGLES:
      _vshader = new Shader( GL_VERTEX_SHADER,
        Shader::sourceCode( Shader::TRIANGLES_VERTEX ));
      _tcshader = new Shader( GL_TESS_CONTROL_SHADER,
        Shader::sourceCode( Shader::TRIANGLES_TESS_CONTROL ));
      _teshader = new Shader( GL_TESS_EVALUATION_SHADER,
        Shader::sourceCode( Shader::TRIANGLES_TESS_EVALUATION ));
      _fshader = new Shader( GL_FRAGMENT_SHADER,
        Shader::sourceCode( Shader::TRIANGLES_FRAGMENT ));
      break;
    case TRIANGLES_FB:
      _vshader = new Shader( GL_VERTEX_SHADER,
        Shader::sourceCode( Shader::TRIANGLES_VERTEX ));
      _tcshader = new Shader( GL_TESS_CONTROL_SHADER,
        Shader::sourceCode( Shader::TRIANGLES_TESS_CONTROL ));
      _teshader = new Shader( GL_TESS_EVALUATION_SHADER,
        Shader::sourceCode( Shader::TRIANGLES_TESS_EVALUATION ));
      _gshader = new Shader( GL_GEOMETRY_SHADER,
        Shader::sourceCode( Shader::TRIANGLES_GEOMETRY ));
      break;
    case QUADS:
      _vshader = new Shader( GL_VERTEX_SHADER,
        Shader::sourceCode( Shader::QUADS_VERTEX ));
      _tcshader = new Shader( GL_TESS_CONTROL_SHADER,
        Shader::sourceCode( Shader::QUADS_TESS_CONTROL ));
      _teshader = new Shader( GL_TESS_EVALUATION_SHADER,
        Shader::sourceCode( Shader::QUADS_TESS_EVALUATION ));
      _fshader = new Shader( GL_FRAGMENT_SHADER,
        Shader::sourceCode( Shader::QUADS_FRAGMENT ));
      break;
    case QUADS_FB:
      _vshader = new Shader( GL_VERTEX_SHADER,
        Shader::sourceCode( Shader::QUADS_VERTEX ));
      _tcshader = new Shader( GL_TESS_CONTROL_SHADER,
        Shader::sourceCode( Shader::QUADS_TESS_CONTROL ));
      _teshader = new Shader( GL_TESS_EVALUATION_SHADER,
        Shader::sourceCode( Shader::QUADS_TESS_EVALUATION ));
      _fshader = new Shader( GL_GEOMETRY_SHADER,
        Shader::sourceCode( Shader::QUADS_GEOMETRY ));
    }

    if ( _vshader )
      glAttachShader( _id, _vshader->id( ));
    if ( _tcshader )
      glAttachShader( _id, _tcshader->id( ));
    if ( _teshader )
      glAttachShader( _id, _teshader->id( ));
    if ( _gshader )
      glAttachShader( _id, _gshader->id( ));
    if ( _fshader )
      glAttachShader( _id, _fshader->id( ));

    if( _type == TRIANGLES_FB || _type == QUADS_FB )
    {
      const char* fbVaryings[ ] = { "outValue0", "outValue1" };
      glTransformFeedbackVaryings( _id, 2, fbVaryings, GL_SEPARATE_ATTRIBS );
    }

    glBindAttribLocation( _id,  0,  "inVertex" );
    glBindAttribLocation( _id,  1,  "inCenter" );
    glBindAttribLocation( _id,  2,  "inTangent" );

    glLinkProgram( _id );
    //Comprobacion de lincado
    int linked;
    glGetProgramiv( _id, GL_LINK_STATUS, &linked );
    if( !linked )
    {
      GLint logLen;
      glGetProgramiv( _id, GL_INFO_LOG_LENGTH, &logLen );

      char * logString = new char[ logLen ];
      glGetProgramInfoLog( _id, logLen, NULL, logString );
      std::cout << "Error: " << logString << std::endl;
      delete[] logString;

      glDeleteProgram( _id );
      _id = 0;
      return;
    }

    _projMat = glGetUniformLocation( _id, "proy" );
    _viewMat = glGetUniformLocation( _id, "view" );
    _modelMat = glGetUniformLocation( _id, "model" );
    _colorVec = glGetUniformLocation( _id, "color" );
    _cameraPos = glGetUniformLocation( _id, "cameraPos" );
    _levelTess = glGetUniformLocation( _id, "lod" );
    _alphaTangentMod = glGetUniformLocation( _id, "tng" );
    _maxDistance = glGetUniformLocation( _id, "maxDist" );

  }

  unsigned int& Program::id( void )
  {
    return _id;
  }

  std::string Program::_loadShader( const std::string& fileName_ )
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

  void Program::projectionMatrix( float* projMat_ )
  {
    glUseProgram( _id );
    glUniformMatrix4fv( _projMat, 1, GL_FALSE, projMat_ );
  }

  void Program::viewMatrix( float* viewMat_ )
  {
    glUseProgram( _id );
    glUniformMatrix4fv( _viewMat, 1, GL_FALSE, viewMat_ );
  }

  void Program::modelMatrix( float* modelMat_ )
  {
    glUseProgram( _id );
    glUniformMatrix4fv( _modelMat, 1, GL_FALSE, modelMat_ );
  }

  void Program::color( float* colorVec_ )
  {
    glUseProgram( _id );
    glUniform3fv( _colorVec, 1, colorVec_ );
  }

  void Program::cameraPosition( float* cameraPos_ )
  {
    glUseProgram( _id );
    glUniform3fv( _cameraPos, 1, cameraPos_ );
  }

  void Program::levelOfTessellation( float* levelTess_ )
  {
    glUseProgram( _id );
    glUniform1fv( _levelTess, 1, levelTess_ );
  }

  void Program::alphaTangentModule( float* alphaTangentMod_ )
  {
    glUseProgram( _id );
    glUniform1fv( _alphaTangentMod, 1, alphaTangentMod_ );
  }

  void Program::maximumDistance( float* maxDistance_ )
  {
    glUseProgram( _id );
    glUniform1fv( _maxDistance, 1, maxDistance_ );
  }

  void Program::tessellationDistanceFunc( unsigned int* tessDistanceFunc_ )
  {
    glUseProgram( _id );
    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, tessDistanceFunc_ );
  }

} // end namespace nlrender

//EOF
