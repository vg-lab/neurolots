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

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <boost/foreach.hpp>
#include <fstream>

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

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3                    Point;
typedef CGAL::Surface_mesh<Point>          Surface_mesh;

reto::Camera* camera;
nlrender::Renderer* renderer;
nlgeometry::OffWriter offw;


void initContext( int argc, char* argv[ ]);
void initOGL( void );

std::vector< std::string > split( const std::string& string_, char splitter_ )
{
  std::vector< std::string > strings;
  std::istringstream f( string_ );
  std::string s;
  while(( getline( f, s, splitter_ )))
  {
    strings.push_back( s );
  }
  return strings;
}

int main( int argc, char* argv[] )
{
  if ( argc < 2 )
    std::cerr << "Error: Usage: " << argv[0]
              << " morphology_file[.swc|.h5]" << std::endl;
  std::cout << "nlrender example: Morphology Measure" << std::endl;

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( );
  renderer = new nlrender::Renderer( );

  renderer->lod( ) = 0;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 3 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::CENTER;
  format[2] = nlgeometry::TAttribType::TANGENT;

  nsol::SwcReader swcr;
  nsol::VasculatureReader vascur;
  auto fileName = std::string( argv[1] );
  nsol::MorphologyPtr morphology = nullptr;
  morphology = vascur.loadMorphology( fileName );
  nsol::Simplifier::Instance( )->repairSections( morphology );

  mesh = nlgenerator::MeshGenerator::generateMesh( morphology );
  mesh->uploadGPU( format, nlgeometry::Facet::PATCHES );
  mesh->clearCPUData( );

  camera->pivot( Eigen::Vector3f::Zero( ));
  camera->radius( 100.0f );

  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection.transpose( );
  Eigen::Matrix4f view( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view.transpose( );

  offw.writeMesh( renderer->extract( mesh,  mesh->modelMatrix( )),
                  "test.off" );

  std::ifstream input("test.off");

  Surface_mesh surfaceMesh;
  if ( !input || !(input >> surfaceMesh) || ( surfaceMesh.is_empty()))
  {
    std::cout << "Not 2 manifold mesh.*************" << std::endl;
    return 1;
  }
  CGAL::Polygon_mesh_processing::triangulate_faces(surfaceMesh);

  float area = 0.0f;
  area = CGAL::Polygon_mesh_processing::area( surfaceMesh );
  float volume = 0.0f;
  volume = CGAL::Polygon_mesh_processing::volume( surfaceMesh );

  std::cout << "Area: " << area << "\nVolume: "
            << volume << std::endl;
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
