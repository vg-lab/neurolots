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
#include "DemoCallbacks.h"

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

reto::Camera* DemoCallbacks::_camera = nullptr;

int DemoCallbacks::_previousX = 0;

int DemoCallbacks::_previousY = 0;

bool DemoCallbacks::_wireframe = false;

bool DemoCallbacks::_mouseDown = false;

bool DemoCallbacks::_mouseScrolling = false;

bool DemoCallbacks::_rotation = false;

bool DemoCallbacks::_traslation = false;

float DemoCallbacks::_mouseWheelFactor = 1.2f;

float DemoCallbacks::_rotationScale = 0.01f;

float DemoCallbacks::_traslationScale = 0.2f;

void DemoCallbacks::camera( reto::Camera* camera_ )
{
  _camera = camera_;
}

void DemoCallbacks::idleFunc( void )
{
  glutPostRedisplay( );
}

void DemoCallbacks::keyboardFunc(
  unsigned char key_, int /*x_*/, int /*y_*/ )
{
  switch( key_ )
  {
  // Camera control.
  case 'c':
  case 'C':
    _camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
    _camera->radius( 1000.0f );
    _camera->rotation( 0.0f, 0.0f );
    glutPostRedisplay( );
    break;
  case 'm':
  case 'M':
    _wireframe = !_wireframe;
    if ( _wireframe )
    {
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    glutPostRedisplay( );
    break;
  }
}

void DemoCallbacks::mouseFunc(
  int button_, int state_, int x_, int y_ )
{
  /**
   * GLUT
   * button: 0 (left), 1 (central), 2 (right), 3 (wheel up), 4 (wheel down).
   * state: GLUT_DOWN (button down), GLUT_UP (button released).
   */
  if ( state_ == GLUT_DOWN )
  {
    _mouseDown = true;
    if ( button_ == 0 ) _rotation = true;
    if ( button_ == 1 ) _traslation = true;
    if (( button_ == 3 ) || ( button_ == 4 ))
    {
      _mouseScrolling = true;
      float newRadius = ( button_ == 3 ) ?
        _camera->radius( ) / _mouseWheelFactor :
        _camera->radius( ) * _mouseWheelFactor;
      _camera->radius( newRadius );
      glutPostRedisplay();
    }
    // We save X and Y previous positions.
    _previousX = x_;
    _previousY = y_;
  }
  else
  {
    _mouseDown = false;
    if( button_ == 0 ) _rotation = false;
    if( button_ == 1 ) _traslation = false;
    if (( button_ == 3 ) || ( button_ == 4 ))
    {
      _mouseScrolling = false;
    }
  }
}

void DemoCallbacks::mouseMotionFunc( int x_, int y_ )
{
 if( _mouseDown )
  {
    float deltaX = ( float )x_ - _previousX;
    float deltaY = ( float )y_ - _previousY;
    if( _rotation )
    {
      _camera->localRotation( deltaX * _rotationScale,
                              deltaY * _rotationScale );
    }
    if( _traslation )
    {
      _camera->localTranslation( Eigen::Vector3f ( -deltaX * _traslationScale,
                                                   deltaY * _traslationScale,
                                                   0.0f ) );
    }
    _previousX = x_;
    _previousY = y_;
    glutPostRedisplay();
  }
}

void DemoCallbacks::resizeFunc( int width_, int height_ )
{
  _camera->ratio((( float ) width_ ) / height_ );
  glViewport( 0, 0, width_, height_ );
}
