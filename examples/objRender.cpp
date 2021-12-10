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

reto::OrbitalCameraController* camera;
reto::ShaderProgram* program;
nlgeometry::Meshes meshes;

void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );

int main( int argc, char* argv[] )
{
  std::cout << "neurolots example: Obj Render" << std::endl;
  if( argc < 2 )
  {
    std::cerr << "Usage error: " << argv[0] << " file[.obj]" << std::endl;
    return -1;
  }

  initContext( argc, argv );
  initOGL( );

  camera = new reto::OrbitalCameraController( );
  DemoCallbacks::camera( camera );

  nlgeometry::AxisAlignedBoundingBox aabb;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 2 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::NORMAL;

  nlgeometry::ObjReader objr;

  for ( int i = 1; i < argc; i++ )
  {

    mesh = objr.readMesh( std::string( argv[i] ), false );

    if ( mesh->vertices( ).size( ) > 0  && mesh->triangles( ).size( ) > 0 )
    {
      std::cout << "Loaded " << argv[i] << " with: "
                << mesh->vertices( ).size( ) << " vertices, "
                << mesh->triangles( ).size( ) << " triangles and "
                << mesh->quads( ).size( ) << " quads" << std::endl;
      mesh->computeNormals( );
      mesh->uploadGPU( format );
      mesh->computeBoundingBox( );
      mesh->clearCPUData( );

      nlgeometry::AxisAlignedBoundingBox meshAABB = mesh->aaBoundingBox( );

      if ( meshAABB.minimum( ).x( ) < aabb.minimum( ).x( ))
        aabb.minimum( ).x( ) = meshAABB.minimum( ).x();
      if ( meshAABB.minimum( ).y( ) < aabb.minimum( ).y( ))
        aabb.minimum( ).y( ) = meshAABB.minimum( ).y();
      if ( meshAABB.minimum( ).z( ) < aabb.minimum( ).z( ))
        aabb.minimum( ).z( ) = meshAABB.minimum( ).z();

      if ( meshAABB.maximum( ).x( ) > aabb.maximum( ).x( ))
        aabb.maximum( ).x( ) = meshAABB.maximum( ).x();
      if ( meshAABB.maximum( ).y( ) > aabb.maximum( ).y( ))
        aabb.maximum( ).y( ) = meshAABB.maximum( ).y();
      if ( meshAABB.maximum( ).z( ) > aabb.maximum( ).z( ))
        aabb.maximum( ).z( ) = meshAABB.maximum( ).z();

      meshes.push_back( mesh );
    }
    else
      std::cout << "File " << argv[i] << " not loaded" << std::endl;
  }

  camera->position( aabb.center( ));
  camera->radius( aabb.radius( ) / sin( camera->camera()->fieldOfView()));

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
  glutCreateWindow( "Neurolots example: Obj Render" );

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
