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
#include <boost/test/floating_point_comparison.hpp>

using namespace nlgenerator;

BOOST_AUTO_TEST_CASE( joint_constructor )
{
  JointNode joint( Eigen::Vector3f::Zero( ), 1.0f );

  BOOST_CHECK_EQUAL( joint.position( ), Eigen::Vector3f::Zero( ));
  BOOST_REQUIRE_CLOSE( joint.radius( ),  1.0f, 0.00001f );
  BOOST_CHECK( !joint.connectedSoma( ));
  BOOST_CHECK_EQUAL( joint.numberNeighbors( ), 0 );
}
BOOST_AUTO_TEST_CASE( joint_compute_geometry)
{
  auto node0 = new nsol::Node( Eigen::Vector3f( -1.0f, 0.0f, 0.0f ), 0, 1.0f );
  auto node1 = new nsol::Node( Eigen::Vector3f( 1.0f, 0.0f, 0.0f ), 1, 1.0f );
  JointNode joint( Eigen::Vector3f::Zero( ), 1.0f );
  joint.addNeighbour( node0 );
  joint.addNeighbour( node1 );
  joint.computeGeometry( );
  BOOST_CHECK( joint.sectionQuad( node0 ));
  BOOST_CHECK( joint.sectionQuad( node1 ));
  delete node0;
  delete node1;
}
