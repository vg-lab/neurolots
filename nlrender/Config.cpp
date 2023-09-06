/**
 * Copyright (c) 2015-2017 VG-Lab/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>,
 * Pablo Toharia <pablo.toharia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/vg-lab/neurolots>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
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
