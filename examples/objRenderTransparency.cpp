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
#include <algorithm>

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
reto::ShaderProgram* firstPassProgram;
reto::ShaderProgram* secondPassProgram;
reto::Texture2D* opaqueTexture;
reto::Texture2D* accumTexture;
reto::Texture2D* revealageTexture;
reto::Texture2D* depthTexture;
unsigned int screenFbo;
unsigned int sceneFbo;
unsigned int enableTransparencyIndex;
unsigned int disableTransparencyIndex;

unsigned int quadVao;

nlgeometry::Meshes meshes;

float alpha = 0.5f;

void keyboardFunc( unsigned char key_, int x_, int y_ );
void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );
void uploadQuad( void );

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

  camera = new reto::Camera( );
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

  camera->pivot( aabb.center( ));
  camera->radius( aabb.radius( ) / sin( camera->fov( )));

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
  glutKeyboardFunc( keyboardFunc );
  glutMouseFunc( DemoCallbacks::mouseFunc );
  glutMotionFunc( DemoCallbacks::mouseMotionFunc );
  glutReshapeFunc( DemoCallbacks::resizeFunc );
}

void initOGL( void )
{
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

  auto texConfig = reto::TextureConfig( );
  texConfig.internalFormat = GL_RGBA32F;
  texConfig.format = GL_RGBA;
  texConfig.type = GL_FLOAT;
  texConfig.wrapS = GL_LINEAR;
  texConfig.wrapT = GL_LINEAR;

  opaqueTexture = new reto::Texture2D( texConfig, 600, 600 );
  accumTexture = new reto::Texture2D( texConfig, 600, 600 );
  revealageTexture = new reto::Texture2D( texConfig, 600, 600 );

  texConfig.internalFormat = GL_DEPTH_COMPONENT;
  texConfig.format = GL_DEPTH_COMPONENT;
  texConfig.type = GL_UNSIGNED_INT;
  depthTexture = new reto::Texture2D( texConfig, 600, 600 );

  glGenFramebuffers( 1, &screenFbo );
  glBindFramebuffer( GL_FRAMEBUFFER, screenFbo );
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, opaqueTexture->handler( ), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D, depthTexture->handler( ), 0);

  glGenFramebuffers( 1, &sceneFbo );
  glBindFramebuffer( GL_FRAMEBUFFER, sceneFbo );
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, accumTexture->handler( ), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                         GL_TEXTURE_2D, revealageTexture->handler( ), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D, depthTexture->handler( ), 0);
  GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers( 2, drawBuffers );


  firstPassProgram = new reto::ShaderProgram( );
  firstPassProgram->loadVertexShaderFromText(
    examples::transparency_first_pass_vert );
  firstPassProgram->loadFragmentShaderFromText(
    examples::transparency_first_pass_frag );
  firstPassProgram->compileAndLink( );
  firstPassProgram->autocatching( );

  secondPassProgram = new reto::ShaderProgram( );
  secondPassProgram->loadVertexShaderFromText(
    examples::transparency_second_pass_vert );
  secondPassProgram->loadFragmentShaderFromText(
    examples::transparency_second_pass_frag );
  secondPassProgram->compileAndLink( );
  secondPassProgram->autocatching( );


  enableTransparencyIndex = glGetSubroutineIndex( firstPassProgram->program( ),
          GL_FRAGMENT_SHADER, "transparencyEnable" );
  disableTransparencyIndex = glGetSubroutineIndex( firstPassProgram->program( ),
          GL_FRAGMENT_SHADER, "transparencyDisable" );
  secondPassProgram->use( );
  glUniform1i( glGetUniformLocation( secondPassProgram->program( ),
                                     "opaqueTexture" ), 0);
  glUniform1i( glGetUniformLocation( secondPassProgram->program( ),
                                     "accumTexture" ), 1);
  glUniform1i( glGetUniformLocation( secondPassProgram->program( ),
                                     "revealageTexture" ), 2);


  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  uploadQuad( );
}

void renderFunc( void )
{
  // program->use( );
  // program->sendUniform4m("proj", camera->projectionMatrix( ));
  // program->sendUniform4m("view", camera->viewMatrix( ));

  // for ( auto mesh: meshes )
  // {
  //   program->sendUniform4m("model", mesh->modelMatrixVectorized( ));
  //   mesh->renderTriangles( );
  //   mesh->renderQuads( );
  // }
  firstPassProgram->use( );
  firstPassProgram->sendUniform4m("proj", camera->projectionMatrix( ));
  firstPassProgram->sendUniform4m("view", camera->viewMatrix( ));

  glBindFramebuffer( GL_FRAMEBUFFER, screenFbo );
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
  glDepthMask(GL_TRUE);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glDisable( GL_BLEND );
  glEnable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );
  glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &disableTransparencyIndex );
  auto model = meshes[0]->modelMatrixVectorized( );
  model[14] -= 2.0f;
  firstPassProgram->sendUniform4m("model", model );
  meshes[0]->renderTriangles( );
  meshes[0]->renderQuads( );
  model[14] += 2.0f;


  glBindFramebuffer( GL_FRAMEBUFFER, sceneFbo );
  float zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
  float ones[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glClearBufferfv( GL_COLOR, 0, zeros );
  glClearBufferfv( GL_COLOR, 1, ones );
  glEnable( GL_BLEND );
  glBlendFunci(0, GL_ONE, GL_ONE);
  glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
  glDisable( GL_CULL_FACE );
  glDepthMask( GL_FALSE );
  glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &enableTransparencyIndex );
  firstPassProgram->sendUniform4m("model", model );
  firstPassProgram->sendUniformf("alpha", alpha );
  meshes[0]->renderTriangles( );
  meshes[0]->renderQuads( );

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glDisable( GL_DEPTH_TEST );
  glEnable( GL_CULL_FACE );
  glDisable( GL_BLEND );
  glDepthMask(GL_TRUE);
  secondPassProgram->use( );
  glBindVertexArray( quadVao );
  opaqueTexture->bind( 0 );
  accumTexture->bind( 1 );
  revealageTexture->bind( 2 );
  glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                  (void*) 0 );

  glFlush( );
  glutSwapBuffers( );
}

void uploadQuad( void )
{
  std::vector< float > positions( 8 );
  positions[0] = -1.0; positions[1] = 1.0;
  positions[2] = -1.0; positions[3] = -1.0;
  positions[4] = 1.0; positions[5] = -1.0;
  positions[6] = 1.0; positions[7] = 1.0;
  std::vector< unsigned int > indices( 6 );
  indices[0] = 0; indices[1] = 1; indices[2] = 2;
  indices[3] = 0; indices[4] = 2; indices[5] = 3;

  glGenVertexArrays( 1, &quadVao );
  glBindVertexArray( quadVao );

  std::vector< unsigned int > vbos(2);
  glGenBuffers( 2, vbos.data( ));

  glBindBuffer( GL_ARRAY_BUFFER, vbos[0] );
  glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * positions.size( ),
                positions.data( ), GL_STATIC_DRAW );
  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( 0 );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbos[1] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int) *
                indices.size( ), indices.data( ), GL_STATIC_DRAW );
  glBindVertexArray( 0 );
}

void keyboardFunc( unsigned char key_, int /*x_*/, int /*y_*/ )
{
  switch( key_ )
  {
  // Camera control.
  case 'w':
  case 'W':
    alpha += 0.01;
    alpha = std::max(0.0f, std::min(alpha, 1.0f));
    std::cout << alpha << std::endl;
    break;
  case 's':
  case 'S':
    alpha -= 0.01;
    alpha = std::max(0.0f, std::min(alpha, 1.0f));
    std::cout << alpha << std::endl;
    break;
  }
}
