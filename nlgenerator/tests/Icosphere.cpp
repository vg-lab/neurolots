/**
 * Copyright (c) 2015-2017 VG-Lab/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
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

#include <nlgenerator/nlgenerator.h>
#include <nsol/nsol.h>

#include "nlgeneratorTests.h"

using namespace nlgenerator;

BOOST_AUTO_TEST_CASE( icosphere_constructor )
{
  {
    Icosphere icosphere;
    auto facets = icosphere.surface( );
    BOOST_CHECK_EQUAL( facets.size( ), 512 );
    facets = icosphere.structure( );
    BOOST_CHECK_EQUAL( facets.size( ), 16384 );
  }
  {
    Icosphere icosphere( Eigen::Vector3f( 1.0f, 0.0f, 0.0f ), 2.0f, 2 );
    auto facets = icosphere.surface( );
    BOOST_CHECK_EQUAL( facets.size( ), 128 );
    facets = icosphere.structure( );
    BOOST_CHECK_EQUAL( facets.size( ), 2048 );
  }
}
