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
#ifndef __EXAMPLES_DEMO_CALLBACKS__
#define __EXAMPLES_DEMO_CALLBACKS__

#include <reto/reto.h>

class DemoCallbacks
{

public:

  static void camera( reto::OrbitalCameraController* camera_ );

  static void idleFunc( void );

  static void keyboardFunc( unsigned char key_, int x_, int y_ );

  static void mouseFunc( int button_, int state_, int x_, int y_ );

  static void mouseMotionFunc( int x_, int y_ );

  static void resizeFunc( int width_, int height_ );

protected:

  static reto::OrbitalCameraController* _camera;

  // X Y mouse position.
  static int _previousX;
  static int _previousY;

  static bool _wireframe;
  static bool _mouseDown;
  static bool _mouseScrolling;
  static bool _rotation;
  static bool _traslation;

  static float _mouseWheelFactor;
  static float _rotationScale;
  static float _traslationScale;

}; // class DemosCallBacks

#endif
