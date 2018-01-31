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


void initContext( int argc, char* argv[ ]);
void initOGL( void );

int main( int argc, char* argv[] )
{
  std::cout << "neurolots example: Morphology Extractor" << std::endl;
  if ( argc < 2 )
  {  std::cerr << "Error: Usage: " << argv[0]
              << " morphology_file[.swc|.h5] -lod [float] -out [.obj|.off]"
               << std::endl;
    return 1;
  }
  std::string inFile( argv[1]);
  float lod = 1.0f;
  std::string outFile( "out.obj");
  for ( int i = 1; i < argc; ++i )
  {
    std::string option( argv[i]);
    try
    {
        if ( option.compare( "-lod" ) == 0 )
        {
          lod = std::atof( argv[i+1]);
          ++i;
        }
        else if ( option.compare( "-out" ) == 0 )
        {
          outFile = std::string( argv[i+1]);
          ++i;
        }
    }
    catch( ... )
    {
      std::cerr << "Error: Usage: " << argv[0]
                << " morphology_file[.swc|.h5] -lod [float] -out [.obj|.off]"
                << std::endl;
      return 1;
    }
  }
  initContext( argc, argv );
  initOGL( );

  reto::Camera camera;
  nlrender::Renderer renderer;
  renderer.lod( ) = lod;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 3 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::CENTER;
  format[2] = nlgeometry::TAttribType::TANGENT;

  nsol::SwcReader swcr;
#ifdef NSOL_USE_HDF5
  nsol::VasculatureReader vascur;
#endif
  auto fileExt = boost::filesystem::extension( inFile );
  nsol::MorphologyPtr morphology = nullptr;
  if ( fileExt.compare( ".swc" ) == 0 )
  {
    morphology = swcr.readMorphology( inFile );
    nsol::Simplifier::Instance( )->simplify(
      dynamic_cast< nsol::NeuronMorphologyPtr >( morphology ),
      nsol::Simplifier::DIST_NODES_RADIUS );
  }
#ifdef NSOL_USE_HDF5
  else if ( fileExt.compare( ".h5" ) == 0 )
  {
    morphology = vascur.loadMorphology( inFile );
    nsol::Simplifier::Instance( )->simplify(
      morphology, nsol::Simplifier::DIST_NODES_RADIUS );
    }
#endif
  if ( morphology )
  {
    mesh = nlgenerator::MeshGenerator::generateMesh( morphology );
    mesh->uploadGPU( format, nlgeometry::Facet::PATCHES );
    mesh->clearCPUData( );
    Eigen::Matrix4f projection( camera.projectionMatrix( ));
    renderer.projectionMatrix( ) = projection;
    Eigen::Matrix4f view( camera.viewMatrix( ));
    renderer.viewMatrix( ) = view;
    fileExt = boost::filesystem::extension( outFile );
    if ( fileExt.compare( ".obj" ) == 0 )
    {
      nlgeometry::ObjWriter::writeMesh(
        renderer.extract( mesh, mesh->modelMatrix( )), outFile );
      std::cout << "Mesh saved to " << outFile << std::endl;
    }
    else if ( fileExt.compare( ".off" ) == 0 )
    {
      nlgeometry::OffWriter::writeMesh(
        renderer.extract( mesh, mesh->modelMatrix( )), outFile );
      std::cout << "Mesh saved to " << outFile << std::endl;
    }
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
  glutCreateWindow( " Neurolots example: Obj Render" );

  glewExperimental = GL_TRUE;
  glewInit( );
}

void initOGL( void )
{
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

  glEnable( GL_DEPTH_TEST );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}
