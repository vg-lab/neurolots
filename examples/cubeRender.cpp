/**
 * Copyright (c) 2015-2017 VG-Lab/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/vg-lab/neurolots>
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

#include "Cube.h"
#include "Shaders.h"

reto::OrbitalCameraController* camera;
reto::ShaderProgram* program;
nlgeometry::Meshes meshes;

void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );

int main( int argc, char* argv[] )
{
  std::cout << "neurolots example: Cube Render" << std::endl;

  initContext( argc, argv );
  initOGL( );

  camera = new reto::OrbitalCameraController( );
  DemoCallbacks::camera( camera );

  nlgeometry::AxisAlignedBoundingBox aabb;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 2 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::NORMAL;

  mesh = new Cube( );
  std::cout << "Loaded cube with: "
                << mesh->vertices( ).size( ) << " vertices, "
                << mesh->triangles( ).size( ) << " triangles and "
                << mesh->quads( ).size( ) << " quads" << std::endl;
  mesh->computeNormals( );
  mesh->uploadGPU( format );
  mesh->computeBoundingBox( );
  mesh->clearCPUData( );

  aabb = mesh->aaBoundingBox( );

  meshes.push_back( mesh );

  camera->position( aabb.center( ));
  camera->radius( aabb.radius( ) / sin( camera->camera()->fieldOfView( )));

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

  glutCreateWindow( "Neurolots example: Cube Render" );

  glewExperimental = GL_TRUE;
  glewInit( );

  glutDisplayFunc( renderFunc );
  glutIdleFunc( DemoCallbacks::idleFunc );
  glutKeyboardFunc( DemoCallbacks::keyboardFunc );
  glutMouseFunc( DemoCallbacks::mouseFunc );
  glutMotionFunc( DemoCallbacks::mouseMotionFunc );
  glutReshapeFunc( DemoCallbacks::resizeFunc );
}

void initOGL( void )
{
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

  program = new reto::ShaderProgram( );
  program->loadVertexShaderFromText( examples::example_vert );
  program->loadFragmentShaderFromText( examples::example_frag );
  program->compileAndLink( );
  program->autocatching( );

  glEnable( GL_DEPTH_TEST );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void renderFunc( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  program->use( );
  program->sendUniform4m("proj", camera->camera()->projectionMatrix( ));
  program->sendUniform4m("view", camera->camera()->viewMatrix( ));

  for ( auto mesh: meshes )
  {
    program->sendUniform4m("model", mesh->modelMatrixVectorized( ));
    mesh->renderTriangles( );
    mesh->renderQuads( );
  }

  glFlush( );
  glutSwapBuffers( );
}
