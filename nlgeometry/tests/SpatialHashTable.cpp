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
#include <nlgeometry/nlgeometry.h>
#include "nlgeometryTests.h"

using namespace nlgeometry;

void checkEqualVertices( VertexPtr vertex0_, VertexPtr vertex1_ )
{
  BOOST_CHECK_EQUAL( vertex0_->id( ), vertex1_->id( ));
  BOOST_CHECK_EQUAL( vertex0_->position( ), vertex1_->position( ));
  BOOST_CHECK_EQUAL( vertex0_->normal( ), vertex1_->normal( ));
  BOOST_CHECK_EQUAL( vertex0_->color( ), vertex1_->color( ));
  BOOST_CHECK_EQUAL( vertex0_->uv( ), vertex1_->uv( ));
}

BOOST_AUTO_TEST_CASE( spatialHashTable )
{
  SpatialHashTable sht;

  VertexPtr in0 = new Vertex( Eigen::Vector3f( 1.0f, 1.0f, 1.0f ));
  VertexPtr in1 = new Vertex( Eigen::Vector3f( 2.0f, 2.0f, 2.0f ));
  VertexPtr in2 = new Vertex( Eigen::Vector3f( 1.0f, 1.0f, 1.0f ));
  VertexPtr in3 = new Vertex( Eigen::Vector3f( 2.0f, 2.0f, 2.0f ));

  VertexPtr out0 = sht.insert( in0 );
  VertexPtr out1 = sht.insert( in1 );
  VertexPtr out2 = sht.insert( in2 );
  VertexPtr out3 = sht.insert( in3 );

  checkEqualVertices( in0, out0 );
  checkEqualVertices( in1, out1 );
  checkEqualVertices( in2, out2 );
  checkEqualVertices( in3, out3 );

  BOOST_CHECK( in0 == out0 );
  BOOST_CHECK( in1 == out1 );
  BOOST_CHECK( in2 != out2 );
  BOOST_CHECK( in0 == out2 );
  BOOST_CHECK( in3 != out3 );
  BOOST_CHECK( in1 == out3 );
}
