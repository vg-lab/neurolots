/**
 * @file    GeometricPrimitive.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "GeometricPrimitive.h"

namespace nlgenerator
{

  GeometricPrimitive::GeometricPrimitive( int a_, int b_, int c_, int d_ )
    : _a( a_ )
    , _b( b_ )
    , _c( c_ )
    , _d( d_ )
  {

  }

  GeometricPrimitive::GeometricPrimitive( int a_, int b_, int c_, int d_,
                                          int e_ )
    : _a( a_ )
    , _b( b_ )
    , _c( c_ )
    , _d( d_ )
    , _e( e_ )
  {

  }

  GeometricPrimitive::~GeometricPrimitive( )
  {

  }

  //Setter
  void GeometricPrimitive::a( int a_ )
  {
    _a = a_;
  }

  void GeometricPrimitive::b( int b_ )
  {
    _b = b_;
  }

  void GeometricPrimitive::c( int c_ )
  {
    _c = c_;
  }

  void GeometricPrimitive::d( int d_ )
  {
    _d = d_;
  }

  void GeometricPrimitive::e( int e_ )
  {
    _e = e_;
  }

  //Getter
  int GeometricPrimitive::a( void )
  {
    return _a;
  }

  int GeometricPrimitive::b( void )
  {
    return _b;
  }

  int GeometricPrimitive::c( void )
  {
    return _c;
  }

  int GeometricPrimitive::d( void )
  {
    return _d;
  }

  int GeometricPrimitive::e( void )
  {
    return _e;
  }

} // end namespace nlgenerator

//EOF
