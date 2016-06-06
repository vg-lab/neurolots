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
    GeometricPrimitive( int a_, int b_, int c_, int d_ );

    NLGENERATOR_API
    GeometricPrimitive( int a_, int b_, int c_, int d_, int e_ );

    NLGENERATOR_API
    ~GeometricPrimitive( void );

    //Setters

    NLGENERATOR_API
    void a( int a_ );

    NLGENERATOR_API
    void b( int b_ );

    NLGENERATOR_API
    void c( int c_ );

    NLGENERATOR_API
    void d( int d_ );

    NLGENERATOR_API
    void e( int e_ );

    //Getters

    NLGENERATOR_API
    int a( void );

    NLGENERATOR_API
    int b( void );

    NLGENERATOR_API
    int c( void );

    NLGENERATOR_API
    int d( void );

    NLGENERATOR_API
    int e( void );

    private:

      int _a;
      int _b;
      int _c;
      int _d;
      int _e;

  };

  typedef GeometricPrimitive * GeometricPrimitivePtr;

} // end namespace nlgenerator

#endif // __NLGENERATOR_GEOMETRIC_PRIMITIVE__

//EOF
