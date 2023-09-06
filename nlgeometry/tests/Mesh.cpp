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
#include <nlgeometry/nlgeometry.h>
#include "nlgeometryTests.h"

using namespace nlgeometry;

BOOST_AUTO_TEST_CASE( mesh_constructor )
{
  float minLimit = std::numeric_limits<float>::min( );
  float maxLimit = std::numeric_limits<float>::max( );
  Eigen::Vector3f minimum( maxLimit, maxLimit, maxLimit );
  Eigen::Vector3f maximum( minLimit, minLimit, minLimit );

  Mesh mesh;

  BOOST_CHECK_EQUAL( mesh.vertices( ).size( ), 0 );
  BOOST_CHECK_EQUAL( mesh.triangles( ).size( ), 0 );
  BOOST_CHECK_EQUAL( mesh.quads( ).size( ), 0 );

  AxisAlignedBoundingBox aabb = mesh.aaBoundingBox( );
  BOOST_CHECK_EQUAL( aabb.minimum( ), minimum );
  BOOST_CHECK_EQUAL( aabb.maximum( ), maximum );
}
