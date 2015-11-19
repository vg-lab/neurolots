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
      TRIANGLES_FB,
      QUADS,
      QUADS_FB
    } TProgram;

    NLRENDER_API
    Program( TProgram type_, const std::string& path_ );

    NLRENDER_API
    ~Program( void );

    NLRENDER_API
    void Init( void );

    NLRENDER_API
    GLuint id( void );


  private:

    GLuint _LoadShader( const std::string& fileName_, GLenum type_ );
    void _ShaderInit( void );

    TProgram _type;

    GLuint id_;

    GLuint vshader_;
    GLuint teshader_;
    GLuint tcshader_;
    GLuint gshader_;
    GLuint fshader_;

    std::string _path;
  };

}// end namespace neurolots

#endif // __NEUROLOTS_PROGRAM__

//EOF
