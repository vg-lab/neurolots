/**
 * @file    GeometricPrimitive.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLGENERATOR_GEOMETRIC_PRIMITIVE__
#define __NLGENERATOR_GEOMETRIC_PRIMITIVE__

#include <nlgenerator/api.h>

namespace nlgenerator
{

  class GeometricPrimitive
  {

  public:

    NLGENERATOR_API
    GeometricPrimitive( int vertexA_, int vertexB_, int vertexC_,
                        int vertexD_ );

    NLGENERATOR_API
    GeometricPrimitive( int vertexA_, int vertexB_, int vertexC_,
                        int vertexD_, int vertexE_ );

    NLGENERATOR_API
    ~GeometricPrimitive( void );

    //Setters

    NLGENERATOR_API
    void vertexA( int vertexA_ );

    NLGENERATOR_API
    void vertexB( int vertexB_ );

    NLGENERATOR_API
    void vertexC( int vertexC_ );

    NLGENERATOR_API
    void vertexD( int vertexD_ );

    NLGENERATOR_API
    void vertexE( int vertexE_ );

    //Getters

    NLGENERATOR_API
    int vertexA( void );

    NLGENERATOR_API
    int vertexB( void );

    NLGENERATOR_API
    int vertexC( void );

    NLGENERATOR_API
    int vertexD( void );

    NLGENERATOR_API
    int vertexE( void );

    private:

      int _vertexA;
      int _vertexB;
      int _vertexC;
      int _vertexD;
      int _vertexE;

  };

  typedef GeometricPrimitive * GeometricPrimitivePtr;

} // end namespace nlgenerator

#endif // __NLGENERATOR_GEOMETRIC_PRIMITIVE__

//EOF
