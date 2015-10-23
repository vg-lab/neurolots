/**
 * @file    Progam.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_PROGRAM__
#define __NEUROLOTS_PROGRAM__


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

#include <string>

#include <neurolots/nlrender/api.h>

namespace neurolots
{

  class Program
  {

  public:

    typedef enum
    {
      LINES = 0,
      TRIANGLES,
      QUADS,
    } TProgram;

    NLRENDER_API
    Program( TProgram type_, const std::string& path_ );

    NLRENDER_API
    ~Program( void );

    NLRENDER_API
    void Init( void );

    //Setters
    NLRENDER_API
    void lod( float _lod );

    NLRENDER_API
    void tng( float _tng );

    NLRENDER_API
    void maxDist( float _maxDist );

    //Getters
    NLRENDER_API
    TProgram type( void );

    NLRENDER_API
    GLuint id( void );

    NLRENDER_API
    GLint inVertex( void );

    NLRENDER_API
    GLint inCenter( void );

    NLRENDER_API
    GLint inTangent( void );

    NLRENDER_API
    GLuint vshader( void );

    NLRENDER_API
    GLuint teshader( void );

    NLRENDER_API
    GLuint tcshader( void );

    NLRENDER_API
    GLuint fshader( void );

    NLRENDER_API
    GLint uProy( void );

    NLRENDER_API
    GLint uView( void );

    NLRENDER_API
    GLint uModel( void );

    NLRENDER_API
    GLint uColor( void );

    NLRENDER_API
    GLint uCameraPos( void );

    NLRENDER_API
    GLint uDesp( void );

    NLRENDER_API
    GLint uLod( void );

    NLRENDER_API
    GLint uTng( void );

    NLRENDER_API
    GLint uMaxDist( void );

    NLRENDER_API
    float lod( void );

    NLRENDER_API
    float tng( void );

    NLRENDER_API
    float maxDist(void);

  private:

    GLuint _LoadShader( const std::string& fileName_, GLenum type_ );
    void _ShaderInit( void );

    TProgram _type;

    GLuint id_;
    GLint inVertex_;
    GLint inCenter_;
    GLint inTangent_;

    GLuint vshader_;
    GLuint teshader_;
    GLuint tcshader_;
    GLuint gshader_;
    GLuint fshader_;

    GLint uProy_;
    GLint uView_;
    GLint uModel_;
    GLint uColor_;
    GLint uCameraPos_;
    GLint uDesp_;
    GLint uLod_;
    GLint uTng_;
    GLint uMaxDist_;

    float maxDist_;
    float lod_;
    float tng_;

    std::string _path;
  };

}// end namespace neurolots

#endif // __NEUROLOTS_PROGRAM__

//EOF
