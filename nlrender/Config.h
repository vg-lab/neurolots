/**
 * @file    Config.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>,
 * Pablo Toharia <pablo.toharia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLRENDER_CONFIG__
#define __NLRENDER_CONFIG__

#include <neurolots/nlrender/api.h>

namespace neurolots
{
  namespace nlrender
  {
    class NLRENDER_API Config
    {

    public:
      static void init( void );
      static bool isInitialized( void );

    private:
      static bool _initialized;

    };

  } // namespace nlrender
} // namespace neurolots


#endif //__NLRENDER_CONFIG__
