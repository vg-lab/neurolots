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
    ~GeometricPrimitive( void );

    //Setters

    NLGENERATOR_API
    void A( int a_ );

    NLGENERATOR_API
    void B( int b_ );

    NLGENERATOR_API
    void C( int c_ );

    NLGENERATOR_API
    void D( int d_ );

    NLGENERATOR_API
    void E( int e_ );

    //Getters

    NLGENERATOR_API
    int A( void );

    NLGENERATOR_API
    int B( void );

    NLGENERATOR_API
    int C( void );

    NLGENERATOR_API
    int D( void );

    NLGENERATOR_API
    int E( void );

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
