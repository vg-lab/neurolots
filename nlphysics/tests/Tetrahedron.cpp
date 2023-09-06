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

#include <limits.h>
#include <nlphysics/nlphysics.h>
#include "nlphysicsTests.h"

using namespace nlphysics;

BOOST_AUTO_TEST_CASE( tetrahedron_constructor )
{
  {
    Eigen::Vector3f vec3Zero( 0.0f, 0.0f, 0.0f );
    NodePtr node0 = new Node( vec3Zero, 0 );
    NodePtr node1 = new Node( vec3Zero, 1 );
    NodePtr node2 = new Node( vec3Zero, 2 );
    NodePtr node3 = new Node( vec3Zero, 3 );

    Tetrahedron tetrahedron( node0, node1, node2, node3 );

    BOOST_CHECK( tetrahedron.node0( ) == node0 );
    BOOST_CHECK( tetrahedron.node1( ) == node1 );
    BOOST_CHECK( tetrahedron.node2( ) == node2 );
    BOOST_CHECK( tetrahedron.node3( ) == node3 );

    BOOST_CHECK_EQUAL( tetrahedron.volume( ), 0.0f );

    BOOST_CHECK( !tetrahedron.face0( ));
    BOOST_CHECK( !tetrahedron.face1( ));
    BOOST_CHECK( !tetrahedron.face2( ));
    BOOST_CHECK( !tetrahedron.face3( ));
  }
  {
    Eigen::Vector3f vec3Zero( 0.0f, 0.0f, 0.0f );
    NodePtr node0 = new Node( vec3Zero, 0, true );
    NodePtr node1 = new Node( vec3Zero, 1, true );
    NodePtr node2 = new Node( vec3Zero, 2, true );
    NodePtr node3 = new Node( vec3Zero, 3, true );

    Tetrahedron tetrahedron( node0, node1, node2, node3 );

    BOOST_CHECK( tetrahedron.node0( ) == node0 );
    BOOST_CHECK( tetrahedron.node1( ) == node1 );
    BOOST_CHECK( tetrahedron.node2( ) == node2 );
    BOOST_CHECK( tetrahedron.node3( ) == node3 );

    BOOST_CHECK_EQUAL( tetrahedron.volume( ), 0.0f );

    BOOST_CHECK( tetrahedron.face0( ));
    BOOST_CHECK( tetrahedron.face1( ));
    BOOST_CHECK( tetrahedron.face2( ));
    BOOST_CHECK( tetrahedron.face3( ));
  }
}
