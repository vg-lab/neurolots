#include "Config.h"
#include "GL/glew.h"

namespace neurolots
{
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
} // namespace neurolots
