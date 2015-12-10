/**
 * @file    Config.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>,
 * Pablo Toharia <pablo.toharia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "Config.h"
#include "GL/glew.h"

namespace nlrender
{

  bool Config::_initialized = false;

  void Config::init( void )
  {
    if ( !_initialized )
    {
      glewExperimental = GL_TRUE;
      glewInit( );
      _initialized = true;
    }
  }

  bool Config::isInitialized( void )
  {
    return _initialized;
  }

} // namespace nlrender
