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
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#include <iostream>
#include <fstream>


namespace neurolots
{

  class Program
  {

    public:

      typedef enum
      {
        LINES = 0,
        TRIANGLES,
        QUADS
      } TProgram;

      Program( TProgram type_ = LINES );
      ~Program( void );

      void Init( void );

      //Seters
      void lod( float _lod );
      void tng( float _tng );
      void maxDist( float _maxDist );

      //Geters
      TProgram type( void );
      GLuint id( void );
      GLint inVertex( void );
      GLint inCenter( void );
      GLint inTangent( void );
      GLuint vshader( void );
      GLuint teshader( void );
      GLuint tcshader( void );
      GLuint fshader( void );
      GLint uProy( void );
      GLint uView( void );
      GLint uModel( void );
      GLint uColor( void );
      GLint uCameraPos( void );
      GLint uDesp( void );
      GLint uLod( void );
      GLint uTng( void );
      GLint uMaxDist( void );

      float lod( void );
      float tng( void );
      float maxDist(void);

    private:

      GLuint LoadShader( const char * fileName, GLenum type );
      void ShaderInit( void );

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
  };

}// end namespace neurolots

#endif // __NEUROLOTS_PROGRAM__

//EOF
