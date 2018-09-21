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
#include <webstreamer/webstreamer.hpp>
#include <webstreamer/console_logger.hpp>
#include <webstreamer/input_processor.hpp>
#include <webstreamer/mouse_event.hpp>
#include <webstreamer/keyboard_event.hpp>

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

class InputProcessor
  : public webstreamer::AsynchronousInputProcessor{

public:

  InputProcessor( reto::Camera* camera_, nlrender::Renderer* renderer_ )
    : webstreamer::AsynchronousInputProcessor( )
    , _adaptiveCriteria( true )
    , _wireframe( false )
    , _previousX( 0.0f )
    , _previousY( 0.0f )
    , _mouseDown( false )
    , _rotation( false )
    , _traslation( false )
    , _zoom( false )
    , _mouseWheelFactor( 1.2f )
    , _rotationScale( 10.0f )
    , _traslationScale( 1.0f )
  {
    _camera = camera_;
    _renderer = renderer_;
  }

  void ProcessMouseInput( const webstreamer::MouseEvent& event )
  {
    if ( event.action( ) == webstreamer::MouseAction::MOVE )
    {
      float x = event.x( );
      float y = event.y( );
      float deltaX = x - _previousX;
      float deltaY = y - _previousY;
      if( _rotation )
      {
        _camera->localRotation( deltaX * _rotationScale,
                                deltaY * _rotationScale );
      }
      if( _traslation )
      {
        _camera->localTranslation(
          Eigen::Vector3f ( -deltaX * _traslationScale * _camera->radius( ),
                            deltaY * _traslationScale * _camera->radius( ),
                            0.0f ) );
      }
      if ( _zoom )
      {
        _camera->radius( _camera->radius( ) + deltaY  * _camera->radius( ));
      }
      _previousX = x;
      _previousY = y;
    }
    else if ( event.action( ) == webstreamer::MouseAction::BUTTON_DOWN )
    {
      _mouseDown = true;
      _rotation = ( event.button( ) == webstreamer::MouseButton::MAIN );
      _traslation = ( event.button( ) == webstreamer::MouseButton::AUXILIARY );
      _zoom = ( event.button( ) == webstreamer::MouseButton::SECONDARY );
      _previousX = event.x( );
      _previousY = event.y( );
    }
    else if ( event.action( ) == webstreamer::MouseAction::BUTTON_UP )
    {
      _mouseDown = false;
      _rotation = false;
      _traslation = false;
      _zoom = false;
    }
    std::cout << "mouse event " << event.x( ) << " " << event.y( ) << std::endl;
    std::cout << (unsigned int)event.button( ) << std::endl;
  }

  void ProcessKeyboardInput( const webstreamer::KeyboardEvent& event )
  {
    char key = event.key( )[0];
    std::cout << key << std::endl;
    if ( event.action( ) == webstreamer::KeyboardAction::KEY_DOWN )
    {
       Eigen::Matrix4f view;
       switch( key )
       {
         // Camera control.
       case 'c':
       case 'C':
         _camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
         _camera->radius( 1000.0f );
         _camera->rotation( 0.0f, 0.0f );
         break;
       case 'w':
         _renderer->lod( ) += 0.1f;
         std::cout << "Level of subdivision " << _renderer->lod( ) << std::endl;
         break;
       case 's':
         _renderer->lod( ) -= 0.1f;
         std::cout << "Level of subdivision " << _renderer->lod( ) << std::endl;
         break;
       case 't':
         _adaptiveCriteria = !_adaptiveCriteria;
         if ( _adaptiveCriteria )
           _renderer->tessCriteria( ) = nlrender::Renderer::LINEAR;
         else
           _renderer->tessCriteria( ) = nlrender::Renderer::HOMOGENEOUS;
         break;
       }
    }
  }

protected:
  reto::Camera* _camera;
  nlrender::Renderer* _renderer;
  bool _adaptiveCriteria;
  bool _wireframe;

  float _previousX;
  float _previousY;

  bool _mouseDown;
  bool _rotation;
  bool _traslation;
  bool _zoom;

  float _mouseWheelFactor;
  float _rotationScale;
  float _traslationScale;
};


reto::Camera* camera;
nlgeometry::Meshes meshes;
nlrender::Renderer* renderer;
std::vector< Eigen::Matrix4f > models;
nlgeometry::OffWriter offw;
nlgeometry::ObjWriter objw;

using webstreamer::ConsoleLogger;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

webstreamer::WebStreamer webStreamer;
InputProcessor* inputProcessor;

bool wireframe = false;
bool adaptiveCriteria = false;

void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );
void keyboardFunc( unsigned char key, int, int );

int main( int argc, char* argv[] )
{
  if ( argc < 2 )
  {  std::cerr << "Error: Usage: " << argv[0]
              << " morphology_file[.swc|.h5]" << std::endl;
    return 1;
  }
  std::cout << "neurolots example: Morphology Render" << std::endl;
  webstreamer::CreateImmortalLogListener<ConsoleLogger>(webstreamer::LogLevel::DEBUG);

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( );
  DemoCallbacks::camera( camera );
  renderer = new nlrender::Renderer( );
  inputProcessor = new InputProcessor( camera, renderer );
  webStreamer.RegisterInputProcessor( inputProcessor );


  renderer->lod( ) = 0.3f;
  renderer->maximumDistance( ) = 1000.0f;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 3 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::CENTER;
  format[2] = nlgeometry::TAttribType::TANGENT;

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
      mesh = nlgenerator::MeshGenerator::generateMesh( morphology );
      std::cout << "Loaded morphology with: "
                << mesh->vertices( ).size( ) << " vertices, "
                << mesh->triangles( ).size( ) << " triangles and "
                << mesh->quads( ).size( ) << " quads" << std::endl;

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
  glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );
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
  renderer->viewMatrix( ) = view;
  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection;
  renderer->render( meshes, models, Eigen::Vector3f( 0.3f, 0.3f, 0.8f ));
  // for ( auto m: meshes )
  //   renderer->render( m, m->modelMatrix( ),
  //                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  glFlush( );
  glutSwapBuffers( );
  std::vector<unsigned char> data( WINDOW_WIDTH*WINDOW_HEIGHT*3);
  glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE,
               (void*) data.data( ));
  webStreamer.PushFrame( WINDOW_WIDTH, WINDOW_HEIGHT, data.data( ),
                         WINDOW_WIDTH*WINDOW_HEIGHT*3, true);
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
    std::cout << "Level of subdivision " << renderer->lod( ) << std::endl;
    break;
  case 's':
    renderer->lod( ) -= 0.1f;
    std::cout << "Level of subdivision " << renderer->lod( ) << std::endl;
    break;
  case 't':
    adaptiveCriteria = !adaptiveCriteria;
    if ( adaptiveCriteria )
      renderer->tessCriteria( ) = nlrender::Renderer::LINEAR;
    else
      renderer->tessCriteria( ) = nlrender::Renderer::HOMOGENEOUS;
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
