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



namespace nlgenerator
{

  class GeometricPrimitive
  {

    public:

      GeometricPrimitive( int a_ );
      GeometricPrimitive( int a_, int b_, int c_, int d_ );
      ~GeometricPrimitive( void );

      //Setters
      void A( int a_ );
      void B( int b_ );
      void C( int c_ );
      void D( int d_ );
      void E( int e_ );

      //Getters
      int A( void );
      int B( void );
      int C( void );
      int D( void );
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
