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

#include <nlgeometry/nlgeometry.h>
#include <nlgenerator/nlgenerator.h>
#include <nlrender/nlrender.h>
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
nlgeometry::Meshes meshes;
nlrender::Renderer* renderer;
std::vector< Eigen::Matrix4f > models;
nlgeometry::ObjWriter objw;

bool wireframe = false;

void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );
void keyboardFunc( unsigned char key, int, int );

int main( int argc, char* argv[] )
{
  std::cout << "nlrender example: Morphology Render" << std::endl;

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( );
  DemoCallbacks::camera( camera );
  renderer = new nlrender::Renderer( );

  renderer->lod( ) = 0;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 3 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::CENTER;
  format[2] = nlgeometry::TAttribType::TANGENT;

  nsol::SwcReader swcr;

  for ( int i = 1; i < argc; i++ )
  {
    nsol::NeuronMorphologyPtr morphology =
      swcr.readMorphology( std::string( argv[i] ));
    mesh = nlgenerator::MeshGenerator::generateMesh( morphology );
    std::cout << "Loaded morphology with: "
              << mesh->vertices( ).size( ) << " vertices, "
              << mesh->triangles( ).size( ) << " triangles and "
              << mesh->quads( ).size( ) << " quads" << std::endl;

    mesh->uploadGPU( format, nlgeometry::Facet::PATCHES );
    meshes.push_back( mesh );
    models.push_back( Eigen::Matrix4f::Identity( ));
  }

  camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
  camera->radius( 100.0f );
  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection.transpose( );
  Eigen::Matrix4f view( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view.transpose( );

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
  glutCreateWindow( " Neurolots example: Obj Render" );

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
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void renderFunc( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  Eigen::Matrix4f view = Eigen::Matrix4f( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view.transpose( );
  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection.transpose( );
  for ( auto m: meshes )
    renderer->render( m, m->modelMatrix( ),
                      Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  glFlush( );
  glutSwapBuffers( );
}

void keyboardFunc( unsigned char key, int, int )
{
  Eigen::Matrix4f view;
  switch( key )
  {
    // Camera control.
    case 'c':
    case 'C':
      camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
      camera->radius( 1000.0f );
      camera->rotation( 0.0f, 0.0f );
      glutPostRedisplay( );
      break;
    case 'm':
    case 'M':
      wireframe = !wireframe;
      if ( wireframe )
      {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }
      else
      {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
      }
      glutPostRedisplay( );
      break;
  case 'w':
    renderer->lod( ) += 0.1f;
    break;
  case 's':
    renderer->lod( ) -= 0.1f;
    break;
  case 'x':
    for ( auto mesh: meshes )
    {

      objw.writeMesh( renderer->extract( mesh,  mesh->modelMatrix( )),
                      "test.obj" );
    }
    break;
  }
}
