/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/gmrvvis/neurolots>
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

#include <limits.h>
#include <nlphysics/nlphysics.h>
#include "nlphysicsTests.h"

using namespace nlphysics;

BOOST_AUTO_TEST_CASE( node_constructor )
{
  {
    Eigen::Vector3f vec3Zero( 0.0f, 0.0f, 0.0f );

    Node node( vec3Zero, 0 );

    BOOST_CHECK_EQUAL( node.id( ), 0 );
    BOOST_CHECK_EQUAL( node.initialPosition( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.displacement( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.position( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.center( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.tangent( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.force( ), vec3Zero );
    BOOST_CHECK( !node.fixed( ));
    BOOST_CHECK( !node.contour( ));
  }
  {
    Eigen::Vector3f vec3Zero( 0.0f, 0.0f, 0.0f );

    Node node( vec3Zero, 0, true );

    BOOST_CHECK_EQUAL( node.id( ), 0 );
    BOOST_CHECK_EQUAL( node.initialPosition( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.displacement( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.position( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.center( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.tangent( ), vec3Zero );
    BOOST_CHECK_EQUAL( node.force( ), vec3Zero );
    BOOST_CHECK( !node.fixed( ));
    BOOST_CHECK( node.contour( ));
  }
}
