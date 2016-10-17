/**
 * @file    Program.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLRENDER_PROGRAM__
#define __NLRENDER_PROGRAM__

#include "Shader.h"

#include <string>

#include <nlrender/api.h>

namespace nlrender
{

  class Program
  {

  public:

    typedef enum
    {
      TRIANGLES = 0,
      TRIANGLES_FB,
      QUADS,
      QUADS_FB
    } TProgram;

    NLRENDER_API
    Program( TProgram type_ );

    NLRENDER_API
    ~Program( void );

    NLRENDER_API
    void init( void );

    NLRENDER_API
    unsigned int& id( void );

    NLRENDER_API
    void projectionMatrix( float* projMat_ );

    NLRENDER_API
    void viewMatrix( float* viewMat_ );

    NLRENDER_API
    void modelMatrix( float* modelMat_ );

    NLRENDER_API
    void color( float* colorVec_ );

    NLRENDER_API
    void cameraPosition( float* cameraPos_ );

    NLRENDER_API
    void levelOfTessellation( float* levelTess_ );

    NLRENDER_API
    void alphaTangentModule( float* alphaTangentMod_ );

    NLRENDER_API
    void maximumDistance( float* maxDistance_ );

    NLRENDER_API
    void tessellationDistanceFunc( unsigned int* tessDistanceFunc_ );



  private:

    std::string _loadShader( const std::string& fileName_  );

    TProgram _type;

    unsigned int _id;

    ShaderPtr _vshader;
    ShaderPtr _teshader;
    ShaderPtr _tcshader;
    ShaderPtr _gshader;
    ShaderPtr _fshader;

    unsigned int _projMat;
    unsigned int _viewMat;
    unsigned int _modelMat;
    unsigned int _colorVec;
    unsigned int _cameraPos;
    unsigned int _levelTess;
    unsigned int _alphaTangentMod;
    unsigned int _maxDistance;
  };

}// end namespace nlrender

#endif // __NLRENDER_PROGRAM__

//EOF
