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

  GeometricPrimitive::GeometricPrimitive( int vertexA_, int vertexB_,
                                          int vertexC_, int vertexD_ )
    : _vertexA( vertexA_ )
    , _vertexB( vertexB_ )
    , _vertexC( vertexC_ )
    , _vertexD( vertexD_ )
  {

  }

  GeometricPrimitive::GeometricPrimitive( int vertexA_, int vertexB_,
                                          int vertexC_, int vertexD_,
                                          int vertexE_ )
    : _vertexA( vertexA_ )
    , _vertexB( vertexB_ )
    , _vertexC( vertexC_ )
    , _vertexD( vertexD_ )
    , _vertexE( vertexE_ )
  {

  }

  GeometricPrimitive::~GeometricPrimitive( )
  {

  }

  //Setter
  void GeometricPrimitive::vertexA( int vertexA_ )
  {
    _vertexA = vertexA_;
  }

  void GeometricPrimitive::vertexB( int vertexB_ )
  {
    _vertexB = vertexB_;
  }

  void GeometricPrimitive::vertexC( int vertexC_ )
  {
    _vertexC = vertexC_;
  }

  void GeometricPrimitive::vertexD( int vertexD_ )
  {
    _vertexD = vertexD_;
  }

  void GeometricPrimitive::vertexE( int vertexE_ )
  {
    _vertexE = vertexE_;
  }

  //Getter
  int GeometricPrimitive::vertexA( void )
  {
    return _vertexA;
  }

  int GeometricPrimitive::vertexB( void )
  {
    return _vertexB;
  }

  int GeometricPrimitive::vertexC( void )
  {
    return _vertexC;
  }

  int GeometricPrimitive::vertexD( void )
  {
    return _vertexD;
  }

  int GeometricPrimitive::vertexE( void )
  {
    return _vertexE;
  }

} // end namespace nlgenerator

//EOF
