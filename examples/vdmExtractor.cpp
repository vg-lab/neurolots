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
nlgeometry::VDMapPtr vdmap;
Eigen::Matrix4f model;
unsigned int textureSize;

void initContext( int argc, char* argv[ ]);
void initOGL( void );

int main( int argc, char* argv[ ])
{
  if ( argc < 2 )
  {
    std::cerr << "Error: Usage: " << argv[0] << " spine_file[.obj] "
             << "-out file[.obj|.off] -lod levelOfDetail[float] "
             << "-alpha0 alpha0[float] -alpha1 alpha1[float] "
             << "-size textureSize[int]" << std::endl;
    return 1;
  }

  std::string inFile( argv[1] );
  std::string outFile( "out.obj" );
  float lod = 20.0f;
  float alpha0 = 0.5f;
  float alpha1 = 0.5f;
  float factor = 3.0f;
  textureSize = 65;

  for ( int i = 2; i < argc; i++ )
  {
    std::string option( argv[i] );
    try
    {
      if ( option.compare( "-out" ) == 0 )
      {
        i++;
        outFile = std::string( argv[i] );
      }
      else if ( option.compare( "-lod") == 0 )
      {
        i++;
        lod = atof( argv[i] );
      }
      else if ( option.compare( "-alpha0") == 0 )
      {
        i++;
        alpha0 = atof( argv[i] );
      }
      else if ( option.compare( "-alpha1") == 0 )
      {
        i++;
        alpha1 = atof( argv[i] );
      }
      else if ( option.compare( "-size") == 0 )
      {
        i++;
        textureSize = atof( argv[i] );
      }
      else if ( option.compare( "-factor") == 0 )
      {
        i++;
        factor = atof( argv[i] );
      }
    }
    catch( ... )
    {
      std::cerr << "Error: Usage: " << argv[0] << " spine_file[.obj] "
                << "-out file[.obj|.off] -lod levelOfDetail[float] "
                << "-alpha0 alpha0[float] -alpha1 alpha1[float] "
                << "-size textureSize[int]" << std::endl;
      return 1;
    }

  }

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( );
  DemoCallbacks::camera( camera );
  renderer = new nlrender::Renderer( );
  renderer->lod( ) = lod;

  nlgeometry::ObjReader objr;
  auto paraMethod0 = nlgeometry::Parametrizer::CURVATURE;
  auto paraMethod1 = nlgeometry::Parametrizer::UNDEFINED;

  auto mesh = objr.readMesh( inFile, false );
  nlgeometry::VDMGenerator::Instance( )->vdmapSize( textureSize );
  vdmap = nlgeometry::VDMGenerator::Instance( )->vectorDisplacementMapTexture(
    mesh, paraMethod0, paraMethod1, alpha0, alpha1, factor );
  model = mesh->modelMatrix( );
  delete mesh;

  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection;
  Eigen::Matrix4f view( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view;

  mesh = renderer->extract( vdmap, model );
  auto fileExt = boost::filesystem::extension( outFile );
  if ( fileExt.compare( ".obj" ) == 0 )
  {
    nlgeometry::ObjWriter::writeMesh( mesh, outFile );
    // std::cout << "Mesh saved to " << outFile << std::endl;
  }
  else if ( fileExt.compare( ".off" ) == 0 )
  {
    nlgeometry::OffWriter::writeMesh( mesh, outFile );
    // std::cout << "Mesh saved to " << outFile << std::endl;
  }

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

  glewExperimental = GL_TRUE;
  glewInit( );
}

void initOGL( void )
{
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );


  glEnable( GL_DEPTH_TEST );
  // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}
