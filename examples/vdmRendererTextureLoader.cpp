/**
 * Copyright (c) 2015-2018 GMRV/URJC.
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
reto::AbstractCameraController* cController;
nlrender::Renderer* renderer;
nlgeometry::VDMapCollectionPtr vdmCollec;
unsigned int textureSize;

bool showMesh = true;
bool wireMode = false;

void renderFunc( void );
void keyboardFunc( unsigned char key, int, int );
void initContext( int argc, char* argv[ ]);
void initOGL( void );

int main( int argc, char* argv[ ])
{
  std::cout << "neurolots example: VDM Render Reader" << std::endl;

  if ( argc != 3 )
  {
    std::cerr << "Error: Usage: " << argv[0]
              << " vdmTexture[.tiff] normalTexture[.tiff]"
              << std::endl;
    return 1;
  }

  std::string vdmTexture( argv[1] );
  std::string normalTexture( argv[2] );

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( );
  cController = new reto::OrbitalCameraController( camera );
  DemoCallbacks::camera( cController );
  renderer = new nlrender::Renderer( );

  vdmCollec = new nlgeometry::VDMapCollection( );
  vdmCollec->addVDMap(
    nlgeometry::VDMapReader::readVDMap( vdmTexture, normalTexture ));

  if ( !vdmCollec )
  {
    std::cerr << "Error: Usage: " << argv[0]
              << " vdmTexture[.tiff] normalTexture[.tiff]"
              << std::endl;
    return 1;
  }

  renderer->lod( ) = vdmCollec->vdmapsSize( ) - 1;


 Eigen::Array3f minimum =
    Eigen::Array3f::Constant( std::numeric_limits< float >::max( ));
  Eigen::Array3f maximum =
    Eigen::Array3f::Constant( std::numeric_limits< float >::min( ));

  for ( auto model: vdmCollec->models( ))
  {
    Eigen::Array3f pos( model.block( 0, 3, 1, 3 ));
    minimum = minimum.min( pos );
    maximum = maximum.max( pos );
  }

  minimum += Eigen::Array3f( -2.0f, -2.0f, -2.0f );
  maximum += Eigen::Array3f( 2.0f, 2.0f, 2.0f );
  Eigen::Vector3f center(( maximum + minimum ) * 0.5f );

  cController->position( center );
  cController->radius(
    ( center - Eigen::Vector3f( minimum )).norm( ) / sin( 3.1416f * 0.25f ));

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
  glutCreateWindow( "Neurolots example: vdm render reader" );

  glewExperimental = GL_TRUE;
  glewInit( );

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


  glEnable( GL_DEPTH_TEST );
  // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}

void renderFunc( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  Eigen::Matrix4f view = Eigen::Matrix4f( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view;
  Eigen::Matrix4f projection = Eigen::Matrix4f( camera->projectionMatrix( ));
  renderer->projectionMatrix( )= projection;
  renderer->render( vdmCollec );

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
  case 'm':
    wireMode = !wireMode;
    if ( wireMode )
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    else
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }
}
