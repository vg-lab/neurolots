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

BOOST_AUTO_TEST_CASE( facet_constructor )
{
  {
    Facet facet;

    BOOST_CHECK( facet.vertex0( ) == nullptr );
    BOOST_CHECK( facet.vertex1( ) == nullptr );
    BOOST_CHECK( facet.vertex2( ) == nullptr );
    BOOST_CHECK( facet.vertex3( ) == nullptr );

    std::vector< unsigned int > indices;
    indices = facet.getIndicesAs( Facet::TRIANGLES );
    BOOST_CHECK_EQUAL( indices.size( ), 0 );
    indices = facet.getIndicesAs( Facet::PATCHES );
    BOOST_CHECK_EQUAL( indices.size( ), 0 );
  }
  {
    VertexPtr vertex = new Vertex( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
    vertex->clone( );
    Facet facet( vertex, vertex, vertex );

    BOOST_CHECK( facet.vertex0( ) == vertex );
    BOOST_CHECK( facet.vertex1( ) == vertex );
    BOOST_CHECK( facet.vertex2( ) == vertex );
    BOOST_CHECK( facet.vertex3( ) == nullptr );

    std::vector< unsigned int > indices;
    indices = facet.getIndicesAs( Facet::TRIANGLES );
    BOOST_CHECK_EQUAL( indices.size( ), 3 );
    indices = facet.getIndicesAs( Facet::PATCHES );
    BOOST_CHECK_EQUAL( indices.size( ), 3 );
  }
  {
    VertexPtr vertex = new Vertex( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
    Facet facet( vertex, vertex, vertex, vertex );

    BOOST_CHECK( facet.vertex0( ) == vertex );
    BOOST_CHECK( facet.vertex1( ) == vertex );
    BOOST_CHECK( facet.vertex2( ) == vertex );
    BOOST_CHECK( facet.vertex3( ) == vertex );

    std::vector< unsigned int > indices;
    indices = facet.getIndicesAs( Facet::TRIANGLES );
    BOOST_CHECK_EQUAL( indices.size( ), 6 );
    indices = facet.getIndicesAs( Facet::PATCHES );
    BOOST_CHECK_EQUAL( indices.size( ), 4 );
  }
}
