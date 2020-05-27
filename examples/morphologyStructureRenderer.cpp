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
#include <boost/filesystem.hpp>

#include <nlgeometry/nlgeometry.h>
#include <nlgenerator/nlgenerator.h>
#include <nlrender/nlrender.h>
#include <reto/reto.h>
#include <nsol/nsol.h>

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
nlgeometry::OffWriter offw;
nlgeometry::ObjWriter objw;

unsigned int width = 600;
unsigned int height = 600;

bool wireframe = false;
bool adaptiveCriteria = false;

Eigen::Vector3f currentColor = Eigen::Vector3f( 0.0, 0.0, 1.0 );
nlgenerator::NodeIdToVerticesIds nodeIdToVerticesIds;

float nodeId = 0.0f;
std::vector< unsigned int > nodeIds;

void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );
void resizeFunc( int width_, int height_ );
void keyboardFunc( unsigned char key, int, int );

int main( int argc, char* argv[] )
{
  if ( argc < 2 )
  {  std::cerr << "Error: Usage: " << argv[0]
              << " morphology_file[.swc|.h5]" << std::endl;
    return 1;
  }
  std::cout << "neurolots example: Morphology Render" << std::endl;

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( );
  DemoCallbacks::camera( camera );
  renderer = new nlrender::Renderer( );

  renderer->lod( ) = 10.0f;
  renderer->tessCriteria( nlrender::Renderer::HOMOGENEOUS );
  renderer->colorFunc( nlrender::Renderer::PERVERTEX );
  renderer->maximumDistance( ) = 1000.0f;
  renderer->initTransparencySystem( width, height );
  renderer->alpha( ) = 0.2f;

  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 3 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::COLOR;
  format[2] = nlgeometry::TAttribType::CENTER;

  nsol::SwcReader swcr;

#ifdef NSOL_USE_HDF5
  nsol::VasculatureReader vascur;
#endif
  nlgeometry::AxisAlignedBoundingBox aabb;
  for ( int i = 1; i < argc; i++ )
  {
    std::string fileName( argv[i] );
    auto fileExt = boost::filesystem::extension( fileName );
    nsol::MorphologyPtr morphology = nullptr;
    if ( fileExt.compare( ".swc" ) == 0 )
    {
      morphology = swcr.readMorphology( fileName );
      nsol::Simplifier::Instance( )->simplify(
        dynamic_cast< nsol::NeuronMorphologyPtr >( morphology ),
        nsol::Simplifier::DIST_NODES_RADIUS );
    }
#ifdef NSOL_USE_HDF5
    else if ( fileExt.compare( ".h5" ) == 0 )
    {
      morphology = vascur.loadMorphology( fileName );
      nsol::Simplifier::Instance( )->simplify(
        morphology, nsol::Simplifier::DIST_NODES_RADIUS );

    }
#endif
    if ( morphology )
    {
      nlgenerator::NodeIdToVertices nodeIdToVertices;
      mesh = nlgenerator::MeshGenerator::generateStructureMesh(
        morphology, nodeIdToVertices, Eigen::Vector3f( 0.0f, 0.5f,0.8f ),
        true );
      std::cout << "Loaded morphology with: "
                << mesh->vertices( ).size( ) << " vertices, "
                << mesh->lines( ).size( ) << " lines, "
                << mesh->triangles( ).size( ) << " triangles and "
                << mesh->quads( ).size( ) << " quads" << std::endl;

      mesh->uploadGPU( format, nlgeometry::Facet::PATCHES );
      nlgenerator::MeshGenerator::verticesToIndices( nodeIdToVertices,
                                                     nodeIdToVerticesIds );
      mesh->clearCPUData( );

      meshes.push_back( mesh );
      models.push_back( Eigen::Matrix4f::Identity( ));

      mesh = nlgenerator::MeshGenerator::generateMesh( morphology );
      mesh->uploadGPU( format, nlgeometry::Facet::PATCHES );
      mesh->computeBoundingBox( );
      mesh->clearCPUData( );

      meshes.push_back( mesh );
      models.push_back( Eigen::Matrix4f::Identity( ));

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
    }
  }
  camera->pivot( aabb.center( ));
  camera->radius( aabb.radius( ) / sin( camera->fov( )));
  // camera->pivot( Eigen::Vector3f::Zero( ));
  // camera->radius( 1000.0f );

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
  glutInitWindowSize( width, height );
  glutInitWindowPosition( 0, 0 );
  glutCreateWindow( " Neurolots example: Obj Render" );

  glewExperimental = GL_TRUE;
  glewInit( );

  glutDisplayFunc( renderFunc );
  glutIdleFunc( DemoCallbacks::idleFunc );
  glutKeyboardFunc( keyboardFunc );
  glutMouseFunc( DemoCallbacks::mouseFunc );
  glutMotionFunc( DemoCallbacks::mouseMotionFunc );
  glutReshapeFunc( resizeFunc );
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

  if ( wireframe )
  {
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }
  else
  {
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }

  auto mesh = meshes[0];
  std::vector< float > buffer( mesh->verticesSize( )*3);
  for ( unsigned int i = 0; i < mesh->verticesSize( ); i++ )
  {
    buffer[i*3] = 0.0f;
    buffer[i*3+1] = 0.5f;
    buffer[i*3+2] = 0.5f;
  }
  nodeId += 0.005;
  nodeIds.push_back( std::trunc(nodeId) );
  currentColor = Eigen::Vector3f( 1.0f, 0.0f, 0.0f );
  nlgenerator::MeshGenerator::conformBuffer( nodeIds, nodeIdToVerticesIds,
                                               buffer, currentColor );
  mesh->uploadBuffer( nlgeometry::COLOR, buffer );

  renderer->setUpOpaqueTransparencyScene( Eigen::Vector3f( 1.0f, 1.0f, 1.0f ),
                                          width, height );


  Eigen::Matrix4f view = Eigen::Matrix4f( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view;
  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection;
  renderer->colorFunc( nlrender::Renderer::PERVERTEX );
  renderer->render( meshes[0], models[0], Eigen::Vector3f( 0.0f, 0.0f, 1.0f ),
                    true, true, true );

  renderer->setUpTransparentTransparencyScene( );

  renderer->colorFunc( nlrender::Renderer::GLOBAL );
  renderer->render( meshes[1], models[1], Eigen::Vector3f( 0.0f, 0.0f, 1.0f ),
                    true, true, true );

  renderer->composeTransparencyScene( 0 );

  glFlush( );
  glutSwapBuffers( );
}

void resizeFunc( int width_, int height_ )
{
  camera->ratio((( float ) width_ ) / height_ );
  width = width_;
  height = height_;
  glViewport( 0, 0, width_, height_ );
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
      glutPostRedisplay( );
      break;
  case 'w':
    renderer->lod( ) += 0.1f;
    std::cout << "Level of subdivision " << renderer->lod( ) << std::endl;
    break;
  case 's':
    renderer->lod( ) -= 0.1f;
    std::cout << "Level of subdivision " << renderer->lod( ) << std::endl;
    break;
  case 'q':
    renderer->alpha( ) += 0.01f;
    std::cout << "Alpha" << renderer->alpha( ) << std::endl;
    break;
  case 'a':
    renderer->alpha( ) -= 0.01f;
    std::cout << "Alpha" << renderer->alpha( ) << std::endl;
    break;
  case 't':
    adaptiveCriteria = !adaptiveCriteria;
    if ( adaptiveCriteria )
      renderer->tessCriteria( nlrender::Renderer::LINEAR );
    else
      renderer->tessCriteria( nlrender::Renderer::HOMOGENEOUS );
    break;
  case 'x':
    for ( auto mesh: meshes )
    {

      offw.writeMesh( renderer->extract( mesh,  mesh->modelMatrix( )),
                      "test.off" );
      std::cout << "Mesh extracted" << std::endl;
    }
    break;
  case 'z':
    for ( auto mesh: meshes )
    {

      objw.writeMesh( renderer->extract( mesh,  mesh->modelMatrix( )),
                      "test.obj" );
      std::cout << "Mesh extracted" << std::endl;
    }
    break;
  }
}
