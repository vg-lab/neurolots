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

  GeometricPrimitive::~GeometricPrimitive( )
  {

  }

  //Setter
  void GeometricPrimitive::A( int a_ )
  {
    _a = a_;
  }

  void GeometricPrimitive::B( int b_ )
  {
    _b = b_;
  }

  void GeometricPrimitive::C( int c_ )
  {
    _c = c_;
  }

  void GeometricPrimitive::D( int d_ )
  {
    _d = d_;
  }

  void GeometricPrimitive::E( int e_ )
  {
    _e = e_;
  }

  //Getter
  int GeometricPrimitive::A( void )
  {
    return _a;
  }

  int GeometricPrimitive::B( void )
  {
    return _b;
  }

  int GeometricPrimitive::C( void )
  {
    return _c;
  }

  int GeometricPrimitive::D( void )
  {
    return _d;
  }

  int GeometricPrimitive::E( void )
  {
    return _e;
  }

} // end namespace nlgenerator

//EOF
