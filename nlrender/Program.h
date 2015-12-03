/**
 * @file    Progam.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_PROGRAM__
#define __NEUROLOTS_PROGRAM__

#include "Shader.h"

#include <string>

#include <neurolots/nlrender/api.h>

namespace neurolots
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
    Program( TProgram type_, const std::string& path_ = std::string( "" ));

    NLRENDER_API
    ~Program( void );

    NLRENDER_API
    void Init( void );

    NLRENDER_API
    unsigned int& id( void );


  private:

    std::string _LoadShader( const std::string& fileName_  );

    TProgram _type;

    unsigned int id_;

    ShaderPtr vshader_;
    ShaderPtr teshader_;
    ShaderPtr tcshader_;
    ShaderPtr fshader_;

    std::string _path;
  };

}// end namespace neurolots

#endif // __NEUROLOTS_PROGRAM__

//EOF
