/**
 * @file    Shader.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "Shader.h"

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

#include <iostream>

namespace nlrender
{

  Shader::Shader( unsigned int type_, const std::string& source_ )
    : _id( 0 )
    , _type( type_ )
  {
    _init( source_ );
  }

  Shader::~Shader( void )
  {
    glDeleteShader( _id );
  }

  unsigned int& Shader::id( void )
  {
    return _id;
  }

  unsigned int& Shader::type( void )
  {
    return _type;
  }

  void Shader::_init( const std::string& source_ )
  {
    _id = glCreateShader( _type );

    _compileShader( source_ );
  }

  void Shader::_compileShader( const std::string& source_ )
  {
    size_t sourceLen = source_.size( ) + 1;
    char* source = new char[ sourceLen ];

    std::copy( source_.begin( ), source_.end( ), source );
    source[ sourceLen - 1 ] = '\0';

    glShaderSource( _id, 1, ( const char ** )& source,
                    ( const int* )& sourceLen );

    glCompileShader( _id );
    delete[] source;

    //Compilation checking
    int compiled;
    glGetShaderiv( _id, GL_COMPILE_STATUS, & compiled );
    if( !compiled )
    {
      int logLen;
      glGetShaderiv( _id, GL_INFO_LOG_LENGTH, & logLen );

      char *logString = new char[ logLen ];
      glGetShaderInfoLog( _id, logLen, NULL, logString );
      std::cout << "Error: " << logString << std::endl;
      delete[] logString;

      glDeleteShader( _id );
      _id = 0;
    }
  }

  std::string Shader::sourceCode( TShader type_ )
  {
    std::string code;

    switch( type_ )
    {
// TRIANGLES
    case TRIANGLES_VERTEX:
      code = std::string(
        "#version 440\n "
        "subroutine float levelDistType( vec3 position );\n "
        "layout( location = 0 ) subroutine uniform levelDistType levelDist;\n "
        "layout( location = 0 ) in vec3 inVertex;\n "
        "layout( location = 1 ) in vec3 inCenter;\n "
        "layout( location = 2 ) in vec3 inTangent;\n "
        "out vec3 vPosition;\n"
        "out vec3 vCenter;\n"
        "out float vlot;\n"
        "layout( location = 2 ) uniform mat4 model;\n"
        "layout( location = 4 ) uniform vec3 cameraPos;\n"
        "layout( location = 5 ) uniform float lod;\n"
        "layout( location = 7 ) uniform float maxDist;\n"
        "layout( index = 0 ) subroutine( levelDistType )\n "
        "float homogeneous( vec3 postion ){\n "
        "  return lod;}\n "
        "layout( index = 1 ) subroutine( levelDistType )\n "
        "float linear( vec3 position ){\n "
        "  return ( lod - 1 ) * clamp( ( 1.0 - length( position\n"
        "    - cameraPos ) / maxDist ), 0.0, 1.0 );}\n "
        "void main( void ){\n"
        "  vPosition = ( model * vec4(inVertex, 1.0 )).xyz;\n"
        "  vCenter = ( model * vec4( inCenter, 1.0 )).xyz;\n"
        "  vlot = levelDist( vCenter ) * length( inTangent ) + 1.0;}\n"
        );
      break;
    case TRIANGLES_TESS_CONTROL:
      code = std::string(
        "#version 440\n"
        "layout(vertices=3) out;\n"
        "in vec3 vPosition[];\n"
        "in vec3 vCenter[];\n"
        "in float vlot[];\n"
        "out vec3 tcCenter[];\n"
        "out vec3 tcNormal[];\n"
        "out float tcRadius[];\n"
        "#define ID gl_InvocationID\n"

        "void main(){\n"
        "  tcCenter[ID] = vCenter[ID];\n"
        "  tcNormal[ID] = normalize( vPosition[ID]-vCenter[ID] );\n"
        "  tcRadius[ID] = distance( vPosition[ID], vCenter[ID]);\n"
        "  float tcLod = (vlot[0]+vlot[1]+vlot[2])/3;\n"
        "  gl_TessLevelInner[0] = tcLod;\n"
        "  gl_TessLevelOuter[0] = (vlot[1]+vlot[2])/2;\n"
        "  gl_TessLevelOuter[1] = (vlot[2]+vlot[0])/2;\n"
        "  gl_TessLevelOuter[2] = (vlot[0]+vlot[1])/2;}\n"
        );
      break;
    case TRIANGLES_TESS_EVALUATION:
      code = std::string(
        "#version 440\n"
        "layout(triangles) in;\n"
        "in vec3 tcCenter[];\n"
        "in vec3 tcNormal[];\n"
        "in float tcRadius[];\n"
        "out vec3 position;\n"
        "out vec3 normal;\n"
        "out vec3 L;\n"
        "layout( location = 0 ) uniform mat4 proy;\n"
        "layout( location = 1 ) uniform mat4 view;\n"
        "layout( location = 4 ) uniform vec3 cameraPos;\n"

        "void main(){\n"
        "  vec3 tePosition;\n"
        "  vec3 teCenter;\n"
        "  float teRadius;\n"
        "  float u=gl_TessCoord.x;\n"
        "  float v=gl_TessCoord.y;\n"
        "  float w=gl_TessCoord.z;\n"
        "  normal = normalize(tcNormal[0]*u+tcNormal[1]*v+tcNormal[2]*w);\n"
        "  teCenter = tcCenter[0]*u+tcCenter[1]*v+tcCenter[2]*w;\n"
        "  teRadius = tcRadius[0]*u+tcRadius[1]*v+tcRadius[2]*w;\n"
        "  tePosition = teCenter+normal*teRadius;\n"
        "  L = normalize(cameraPos-tePosition);\n"
        "  position = tePosition;\n"
        "  gl_Position = proy*view*vec4(tePosition,1);}\n"
        );
      break;
    case TRIANGLES_GEOMETRY:
      code = std::string(
        "#version 440\n"
        "layout( triangles ) in;\n"
        "layout( triangle_strip, max_vertices = 3 ) out;\n"
        "in vec3 position[3];\n"
        "in vec3 normal[3];\n"
        "in vec3 L[3];\n"
        "out vec3 gnormal;\n"
        "out vec3 gL;\n"
        "out vec3 outValue0;\n"
        "out vec3 outValue1;\n"
        "void main( void ){\n"
        "  outValue0 = position[0];\n"
        "  outValue1 = normal[0];\n"
        "  gnormal = normal[0];\n"
        "  gL = L[0];\n"
        "  gl_Position = gl_in[0].gl_Position;\n"
        "  EmitVertex( );\n"
        "  outValue0 = position[1];\n"
        "  outValue1 = normal[1];\n"
        "  gnormal = normal[1];\n"
        "  gL = L[1];\n"
        "  gl_Position = gl_in[1].gl_Position;\n"
        "  EmitVertex( );\n"
        "  outValue0 = position[2];\n"
        "  outValue1 = normal[2];\n"
        "  gnormal = normal[2];\n"
        "  gL = L[2];\n"
        "  gl_Position = gl_in[2].gl_Position;\n"
        "  EmitVertex( );\n"
        "  EndPrimitive( );}\n"
        );
      break;
    case TRIANGLES_FRAGMENT:
      code = std::string(
        "#version 440\n"
        "out vec4 oColor;\n"
        "in vec3 normal;\n"
        "in vec3 L;\n"
        "layout( location = 3 ) uniform vec3 color;\n"
        "void main(){\n"
        "  vec3 N=normalize(normal);\n"
        "  float dif=dot(N,L);\n"
        "  dif=clamp(dif,0.0,1.0);\n"
        "  oColor = vec4(color*dif,1.0)+vec4(color*0.3,1.0);}\n"
        );
      break;
// QUADS
    case QUADS_VERTEX:
      code = std::string(
        "#version 440\n"
        "subroutine float levelDistType( vec3 position );\n "
        "layout( location = 0 ) subroutine uniform levelDistType levelDist;\n "
        "layout( location = 0 ) in vec3 inVertex;\n"
        "layout( location = 1 ) in vec3 inCenter;\n"
        "layout( location = 2 ) in vec3 inTangent;\n"
        "out vec3 vPosition;\n"
        "out vec3 vCenter;\n"
        "out vec3 vTangent;\n"
        "out float vlot;\n"
        "layout( location = 2 ) uniform mat4 model;\n"
        "layout( location = 4 ) uniform vec3 cameraPos;\n"
        "layout( location = 5 ) uniform float lod;\n"
        "layout( location = 7 ) uniform float maxDist;\n"
        "layout( index = 0 ) subroutine( levelDistType )\n "
        "float homogeneous( vec3 postion ){\n "
        "  return lod;}\n "
        "layout( index = 1 ) subroutine( levelDistType )\n "
        "float linear( vec3 position ){\n "
        "  return ( lod - 1 ) * clamp( ( 1.0 - length( position\n"
        "    - cameraPos ) / maxDist ), 0.0, 1.0 );}\n "
        "void main( void ){\n"
        "  vPosition = ( model * vec4(inVertex, 1.0 )).xyz;\n"
        "  vCenter = ( model * vec4( inCenter, 1.0 )).xyz;\n"
        "  vTangent = ( model * vec4( inTangent, 0.0 )).xyz;\n"
        "  vlot = levelDist( vCenter ) + 1.0;}\n"
        );
      break;
    case QUADS_TESS_CONTROL:
      code = std::string(
        "#version 440\n"
        "layout(vertices=4) out;\n"
        "in vec3 vPosition[];\n"
        "in vec3 vCenter[];\n"
        "in vec3 vTangent[];\n"
        "in float vlot[];\n"
        "out vec3 tcCenter[];\n"
        "out float r[];\n"
        "out vec3 tcNormal[];\n"
        "out vec3 m[];\n"
        "layout( location = 6 )uniform float tng;\n"
        "#define ID gl_InvocationID\n"
        "void main(){\n"
        "  tcCenter[ID] = vCenter[ID];\n"
        "  tcNormal[ID] = normalize( vPosition[ID]-vCenter[ID] );\n"
        "  r[ID]=distance(vPosition[ID],vCenter[ID]);\n"
        "  m[ID]=( tng + 1.0 )* normalize(vTangent[ID]) *\n"
        "    distance(vCenter[0],vCenter[2]) / 8;\n"
        "  float tcLod = (vlot[0]+vlot[1]+vlot[2]+vlot[3])/4;\n"
        "  gl_TessLevelInner[0] = tcLod;\n"
        "  gl_TessLevelInner[1] = tcLod;\n"
        "  gl_TessLevelOuter[0] = (vlot[0]+vlot[2])/2;\n"
        "  gl_TessLevelOuter[1] = (vlot[0]+vlot[1])/2;\n"
        "  gl_TessLevelOuter[2] = (vlot[1]+vlot[3])/2;\n"
        "  gl_TessLevelOuter[3] = (vlot[2]+vlot[3])/2;}\n"
        );
      break;
    case QUADS_TESS_EVALUATION:
      code = std::string(
        "#version 440\n"
        "layout(quads) in;\n"
        "in vec3 tcCenter[];\n"
        "in float r[];\n"
        "in vec3 tcNormal[];\n"
        "in vec3 m[];\n"
        "out vec3 position;\n"
        "out vec3 normal;\n"
        "out vec3 L;\n"
        "layout( location = 0 ) uniform mat4 proy;\n"
        "layout( location = 1 ) uniform mat4 view;\n"
        "layout( location = 2 ) uniform mat4 model;\n"
        "layout( location = 4 ) uniform vec3 cameraPos;\n"
        "void main( ){\n"
        "  vec3 tePosition;\n"
        "  vec3 teCenter;\n"
        "  vec3 teNormal;\n"
        "  float u=gl_TessCoord.x;\n"
        "  float u_2=u*u;\n"
        "  float u_3=u_2*u;\n"
        "  float v=gl_TessCoord.y;\n"
        "  float v_2=v*v;\n"
        "  float v_3=v_2*v;\n"
        "  teCenter = (2*v_3-3*v_2+1)*tcCenter[0]+(v_3-2*v_2+v)*m[0]+\n"
        "    (v_3-v_2)*m[2]+(-2*v_3+3*v_2)*tcCenter[2];\n"
        "  vec3 a_t=mix(tcNormal[0],tcNormal[1],u);\n"
        "  vec3 b_t=mix(tcNormal[2],tcNormal[3],u);\n"
        "  teNormal = normalize(mix(a_t,b_t,v));\n"
        "  float radius=mix(r[0],r[2],v);\n"
        "  tePosition = (radius*teNormal+teCenter);\n"
        "  normal = teNormal;\n"
        "  L = normalize(cameraPos-tePosition);\n"
        "  position = tePosition;\n"
        "  gl_Position = proy*view*vec4(tePosition,1.0);}\n"
        );
      break;
    case QUADS_GEOMETRY:
      code = std::string(
        "#version 440\n"
        "layout( triangles ) in;\n"
        "layout( triangle_strip, max_vertices = 3 ) out;\n"
        "in vec3 position[3];\n"
        "in vec3 normal[3];\n"
        "in vec3 L[3];\n"
        "out vec3 gnormal;\n"
        "out vec3 gL;\n"
        "out vec3 outValue0;\n"
        "out vec3 outValue1;\n"
        "void main( void ){\n"
        "  outValue0 = position[0];\n"
        "  outValue1 = normal[0];\n"
        "  gnormal = normal[0];\n"
        "  gL = L[0];\n"
        "  gl_Position = gl_in[0].gl_Position;\n"
        "  EmitVertex( );\n"
        "  outValue0 = position[1];\n"
        "  outValue1 = normal[1];\n"
        "  gnormal = normal[1];\n"
        "  gL = L[1];\n"
        "  gl_Position = gl_in[1].gl_Position;\n"
        "  EmitVertex( );\n"
        "  outValue0 = position[2];\n"
        "  outValue1 = normal[2];\n"
        "  gnormal = normal[2];\n"
        "  gL = L[2];\n"
        "  gl_Position = gl_in[2].gl_Position;\n"
        "  EmitVertex( );\n"
        "  EndPrimitive( );}\n"
        );
      break;
    case QUADS_FRAGMENT:
      code = std::string(
        "#version 440\n"
        "out vec4 oColor;\n"
        "in vec3 normal;\n"
        "in vec3 L;\n"
        "layout( location = 3 )uniform vec3 color;\n"
        "void main(){\n"
        "  vec3 N=normalize(normal);\n"
        "  float dif=dot(N,L);\n"
        "  dif=clamp(dif,0.0,1.0);\n"
        "  oColor = vec4(color*dif,1.0)+vec4(color*0.3,1.0);}\n"
        );
      break;

    }
    return code;
  }

} // end namespace nlrender
