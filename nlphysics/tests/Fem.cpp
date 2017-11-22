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
#include <nlphysics/nlphysics.h>

#include "nlphysicsTests.h"

#include <boost/test/floating_point_comparison.hpp>

using namespace nlphysics;

BOOST_AUTO_TEST_CASE( fem_constructor )
{
  Nodes nodes;
  auto node0 = new Node( Eigen::Vector3f( 0.0f, 1.0f, 0.0f ), 0 );
  nodes.push_back( node0 );
  node0->fixed( ) = true;
  auto node1 = new Node( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ), 1 );
  nodes.push_back( node1 );
  node1->fixed( ) = true;
  auto node2 = new Node( Eigen::Vector3f( 1.0f, 0.0f, 0.0f ), 2 );
  nodes.push_back( node2 );
  node2->position( ) = Eigen::Vector3f( 2.0f, 0.0f, .0f );
  node2->fixed( ) = true;
  auto node3 = new Node( Eigen::Vector3f( 0.0f, 0.0f, 1.0f ), 3 );
  nodes.push_back( node3 );

  Tetrahedra tetrahedra;
  auto tetrahedron = new Tetrahedron( node0, node1, node2, node3 );
  tetrahedra.push_back( tetrahedron );
  Fem fem( nodes, tetrahedra );
  fem.solve( );

  BOOST_REQUIRE_CLOSE( node3->position( ).x( ),  0.0f, 0.00001f );
  BOOST_REQUIRE_CLOSE( node3->position( ).y( ),  0.0f, 0.00001f );
  BOOST_REQUIRE_CLOSE( node3->position( ).z( ),  0.571429f, 0.0001f );

  delete node0;
  delete node1;
  delete node2;
  delete node3;
  delete tetrahedron;
}
