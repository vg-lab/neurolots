/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of nsol <https://github.com/gmrvvis/neurolots>
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
  BOOST_CHECK( vertex0_ != vertex1_ );
  BOOST_CHECK_EQUAL( vertex0_->id( ), vertex1_->id( ));
  BOOST_CHECK_EQUAL( vertex0_->position( ), vertex1_->position( ));
  BOOST_CHECK_EQUAL( vertex0_->normal( ), vertex1_->normal( ));
  BOOST_CHECK_EQUAL( vertex0_->color( ), vertex1_->color( ));
  BOOST_CHECK_EQUAL( vertex0_->uv( ), vertex1_->uv( ));
}

BOOST_AUTO_TEST_CASE( vertex_constructor )
{
  Eigen::Vector3f vec3Zero( 0.0f, 0.0f, 0.0f );
  Eigen::Vector2f vec2Zero( 0.0f, 0.0f );

  Vertex vertex( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));

  BOOST_CHECK_EQUAL( vertex.id( ), 0 );
  BOOST_CHECK_EQUAL( vertex.position( ), vec3Zero );
  BOOST_CHECK_EQUAL( vertex.normal( ), vec3Zero );
  BOOST_CHECK_EQUAL( vertex.color( ), vec3Zero );
  BOOST_CHECK_EQUAL( vertex.uv( ), vec2Zero );
}

BOOST_AUTO_TEST_CASE( vertex_clone )
{
  Vertex vertex( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
  VertexPtr otherVertex = vertex.clone( );

  checkEqualVertices( &vertex, otherVertex );
}

BOOST_AUTO_TEST_CASE( vertex_meythods )
{
  Vertex vertex( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));

  vertex.displace( Eigen::Vector3f( 1.0f, 2.0f, 3.0f ));
  BOOST_CHECK_EQUAL( vertex.position( ), Eigen::Vector3f( 1.0f, 2.0f, 3.0f ));

  vertex.place( Eigen::Vector3f( 3.0f, 2.0f, 1.0f ));
  BOOST_CHECK_EQUAL( vertex.position( ), Eigen::Vector3f( 3.0f, 2.0f, 1.0f ));

  vertex.rotate( Eigen::Matrix3f::Identity( ));
  BOOST_CHECK_EQUAL( vertex.position( ), Eigen::Vector3f( 3.0f, 2.0f, 1.0f ));

  vertex.rotate( Eigen::Quaternion< float >::Identity( ));
  BOOST_CHECK_EQUAL( vertex.position( ), Eigen::Vector3f( 3.0f, 2.0f, 1.0f ));

  vertex.norm( 5.0f );
  BOOST_CHECK_EQUAL( vertex.position( ).norm( ), 5.0f );
}
