/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/gmrvvis/neurolots>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include <iostream>

#include <nlrender/nlrender.h>
#include <nlgeometry/nlgeometry.h>
#include <reto/reto.h>

#include "DemoCallbacks.h"

//OpenGL
#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
  #include <GL/glew.h>
#endif
#ifdef Darwin
#define __gl_h_
  #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GL/freeglut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/freeglut.h>
#endif

#include "Shaders.h"

reto::Camera* camera;
nlrender::Renderer* renderer;
reto::ShaderProgram* programUV;
nlgeometry::MeshPtr meshUV;
Eigen::Matrix4f model;
Eigen::Matrix4f modelUV;
unsigned int texture;

bool showMesh = true;

void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );
void keyboardFunc( unsigned char key_, int x_, int y_ );

int main( int argc, char* argv[ ])
{
  std::cout << "neurolots example: Mesh Parametrizer" << std::endl;
  if ( argc < 2 )
  {
    std::cerr << "Usage error: " << argv[0] << " file[.obj]" << std::endl;
    return -1;
  }

  initContext( argc, argv );
  initOGL( );

  unsigned int paraId = 0;
  float alpha0 = 1.0f;
  float alpha1 = 1.0f;
  unsigned int textureSize = 33;
  for ( int i = 1; i < argc; i++ )
  {
    if ( std::string( argv[i]).compare( "-m") == 0 )
    {
      i++;
      paraId = std::stoi( argv[i] );
    }
    if ( std::string( argv[i]).compare( "-alpha0") == 0 )
    {
      i++;
      alpha0 = std::stof( argv[i] );
    }
    if ( std::string( argv[i]).compare( "-alpha1") == 0 )
    {
      i++;
      alpha1 = std::stof( argv[i] );
    }
    if ( std::string( argv[i]).compare( "-size") == 0 )
    {
      i++;
      textureSize = std::stoi( argv[i] );
    }
  }

  nlgeometry::Parametrizer::TVertexWeightMethod paraMethod0;
  nlgeometry::Parametrizer::TVertexWeightMethod paraMethod1;
  switch( paraId )
  {
  case 1:
    paraMethod0 = nlgeometry::Parametrizer::RANDOM;
    paraMethod1 = nlgeometry::Parametrizer::UNDEFINED;
    break;
  case 2:
    paraMethod0 = nlgeometry::Parametrizer::CURVATURE;
    paraMethod1 = nlgeometry::Parametrizer::UNDEFINED;
    break;
  case 3:
    paraMethod0 = nlgeometry::Parametrizer::AREA;
    paraMethod1 = nlgeometry::Parametrizer::UNDEFINED;
    break;
  case 4:
    paraMethod0 = nlgeometry::Parametrizer::CURVATURE;
    paraMethod1 = nlgeometry::Parametrizer::AREA;
    break;
  case 5:
    paraMethod0 = nlgeometry::Parametrizer::MEAN_VALUE;
    paraMethod1 = nlgeometry::Parametrizer::RANDOM;
    break;
  default:
    paraMethod0 = nlgeometry::Parametrizer::MEAN_VALUE;
    paraMethod1 = nlgeometry::Parametrizer::UNDEFINED;
  }
  camera = new reto::Camera( );
  renderer = new nlrender::Renderer( );
  renderer->lod( ) = 60;
  DemoCallbacks::camera( camera );

  nlgeometry::AttribsFormat formatUV( 1 );
  formatUV[0] = nlgeometry::TAttribType::UV;

  nlgeometry::ObjReader objr;
  meshUV = objr.readMesh( std::string( argv[1]), false );

  nlgeometry::VDMGenerator::Instance( )->vdmSize( textureSize );
  texture = nlgeometry::VDMGenerator::Instance( )->vectorDisplacementMapTexture(
    meshUV, paraMethod0, paraMethod1, alpha0, alpha1 );

  meshUV->uploadGPU( formatUV );

  model = Eigen::Matrix4f::Identity( );
  model.block< 3, 1 >( 0, 3 ) = Eigen::Vector3f( 1.1f, 0.0f, 0.0f );
  modelUV = Eigen::Matrix4f::Identity( );
  modelUV.block< 3, 1 >( 0, 3 ) = Eigen::Vector3f( -1.1f, 0.0f, 0.0f );

  camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
  camera->radius( 10.0f );

  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection;
  Eigen::Matrix4f view( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view;

  glutMainLoop( );
  return 0;
}

void initContext( int argc, char* argv[ ])
{
  glutInit( &argc, argv );
  glutInitContextVersion( 4, 0 );

  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize( 600, 600 );
  glutInitWindowPosition( 0, 0 );
  glutCreateWindow( "Neurolots example: Mesh Parametrizer" );

  glewExperimental = GL_TRUE;glewInit( );

  glutDisplayFunc( renderFunc );
  glutIdleFunc( DemoCallbacks::idleFunc );
  glutKeyboardFunc( keyboardFunc );
  glutMouseFunc( DemoCallbacks::mouseFunc );
  glutMotionFunc( DemoCallbacks::mouseMotionFunc );
  glutReshapeFunc( DemoCallbacks::resizeFunc );
}

void initOGL( void )
{
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

  programUV = new reto::ShaderProgram( );
  programUV->loadVertexShaderFromText( examples::uv_vert );
  programUV->loadFragmentShaderFromText( examples::uv_frag );
  programUV->compileAndLink( );
  programUV->autocatching( );

  glEnable( GL_DEPTH_TEST );
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}

void renderFunc( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  Eigen::Matrix4f view = Eigen::Matrix4f( camera->viewMatrix( ));
  Eigen::Matrix4f projection = Eigen::Matrix4f( camera->projectionMatrix( ));

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  programUV->use( );
  programUV->sendUniform4m("proj", projection.data( ) );
  programUV->sendUniform4m("view", view.data( ) );
  programUV->sendUniform4m("model", modelUV.data( ));
  meshUV->renderTriangles( );
  meshUV->renderQuads( );

  renderer->viewMatrix( ) = view;
  renderer->projectionMatrix( )= projection;
  unsigned int textureSize = nlgeometry::VDMGenerator::Instance( )->vdmSize( );
  renderer->render( texture, textureSize, model );

  glFlush( );
  glutSwapBuffers( );
}

void keyboardFunc( unsigned char key_, int, int )
{
  switch( key_ )
  {
  case 'w':
    renderer->lod( ) += 1.f;
    std::cout << "Level of tessellation: " << renderer->lod( ) << std::endl;
    break;
  case 's':
    renderer->lod( ) -= 1.f;
    std::cout << "Level of tessellation: " << renderer->lod( ) << std::endl;
    break;
  }
}
