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

BOOST_AUTO_TEST_CASE( sectionQuad_constructor )
{
  OrbitalVertexPtr vertex0 =
    new OrbitalVertex( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
  OrbitalVertexPtr vertex1 =
    dynamic_cast< OrbitalVertexPtr >( vertex0->clone( ));
  OrbitalVertexPtr vertex2 =
    dynamic_cast< OrbitalVertexPtr >( vertex0->clone( ));
  OrbitalVertexPtr vertex3 =
    dynamic_cast< OrbitalVertexPtr >( vertex0->clone( ));

  SectionQuad sectionQuad( vertex0, vertex1, vertex2, vertex3 );

  BOOST_CHECK( sectionQuad.vertex0( ) == vertex0 );
  BOOST_CHECK( sectionQuad.vertex1( ) == vertex1 );
  BOOST_CHECK( sectionQuad.vertex2( ) == vertex2 );
  BOOST_CHECK( sectionQuad.vertex3( ) == vertex3 );

  sectionQuad.inverse( );
  BOOST_CHECK( sectionQuad.vertex0( ) == vertex0 );
  BOOST_CHECK( sectionQuad.vertex1( ) == vertex3 );
  BOOST_CHECK( sectionQuad.vertex2( ) == vertex2 );
  BOOST_CHECK( sectionQuad.vertex3( ) == vertex1 );

  sectionQuad.displace( Eigen::Vector3f( 1.0f, 2.0f, 3.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex0( )->center( ),
                     Eigen::Vector3f( 1.0f, 2.0f, 3.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex1( )->center( ),
                     Eigen::Vector3f( 1.0f, 2.0f, 3.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex2( )->center( ),
                     Eigen::Vector3f( 1.0f, 2.0f, 3.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex3( )->center( ),
                     Eigen::Vector3f( 1.0f, 2.0f, 3.0f ));

  sectionQuad.place( Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex0( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex1( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex2( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex3( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));

  sectionQuad.rotate( Eigen::Matrix3f::Identity( ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex0( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex1( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex2( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex3( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));

  sectionQuad.rotate( Eigen::Quaternion< float >::Identity( ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex0( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex1( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex2( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  BOOST_CHECK_EQUAL( sectionQuad.vertex3( )->center( ),
                     Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
}
