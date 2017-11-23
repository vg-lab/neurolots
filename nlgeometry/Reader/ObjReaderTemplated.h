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
#ifndef __NLGEOMETRY_OBJ_READER__
#define __NLGEOMETRY_OBJ_READER__

#include "../Mesh.h"
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <sstream>

#include <nlgeometry/api.h>

namespace nlgeometry
{

  typedef struct tIndex{
    int position;
    int normal;
    int uv;
  } tIndex;

  typedef struct tFacet{
    std::vector< tIndex> indices;
  } tFacet;


  /* \class ObjReaderTemplated */
  template < class VERTEX >
  class ObjReaderTemplated
  {

  public:

    /**
     * Static method that fill the referenced mesh
     */
    void readMesh( MeshPtr mesh_, const std::string& fileName_,
                      bool quadsToTriangles_ = true ) const;

    /**
     * Static method that return a mesh pointer with the geometric data loaded
     */
    MeshPtr readMesh( const std::string& fileName_,
                      bool quadsToTriangles_ = true ) const;
  protected:

    static std::vector< std::string > _split( std::string& s_, char splitter_ );

    static tIndex _splitStringToIndex( std::string& s_ );

    static void _objToVectors( const std::string& fileName_,
                               std::vector< Eigen::Vector3f >& positions_,
                               std::vector< Eigen::Vector3f >& normals_,
                               std::vector< Eigen::Vector2f >& uvs_,
                               std::vector< tFacet >& triangles_,
                               std::vector< tFacet >& quads_ );
  }; // class ObjReaderTemplated


  typedef ObjReaderTemplated< Vertex > ObjReader;

  template < class VERTEX >
  void ObjReaderTemplated< VERTEX>::readMesh(
    MeshPtr mesh_, const std::string& fileName_, bool quadsToTriangles_ ) const
  {
    std::vector< Eigen::Vector3f > positions;
    std::vector< Eigen::Vector3f > normals;
    std::vector< Eigen::Vector2f > uvs;

    std::vector< tFacet > trianglesVec;
    std::vector< tFacet > quadsVec;

    _objToVectors( fileName_.c_str( ), positions, normals, uvs,
                   trianglesVec, quadsVec );

    Vertices& vertices = mesh_->vertices( );
    Facets& triangles = mesh_->triangles( );
    Facets& quads = mesh_->quads( );

    vertices.clear( );
    triangles.clear( );
    quads.clear( );

    std::unordered_map< std::string, VertexPtr > vertexMap;

    for( auto triangle: trianglesVec )
    {
      std::vector< VertexPtr > facet;
      for( auto index: triangle.indices )
      {
        std::ostringstream oss;
        std::string key;
        oss << index.position << "," << index.normal << "," << index.uv;
        key = oss.str( );
        oss.str("");
        oss.clear();

        VertexPtr vertex;

        auto search = vertexMap.find( key );
        if ( search != vertexMap.end( ))
        {
          vertex = search->second;
        }
        else
        {
          Eigen::Vector3f position = positions[ index.position ];
          Eigen::Vector3f normal( 0.0f, 0.0f, 0.0f );
          Eigen::Vector3f color( 0.0f, 0.0f, 0.0f );
          Eigen::Vector2f uv( 0.0f, 0.0f );
          if ( index.normal >= 0 )
            normal = normals[ index.normal ];
          if ( index.uv >= 0 )
            uv = uvs[ index.uv ];
          vertex = new VERTEX( position, normal, color, uv );
          vertices.push_back( vertex );
          vertexMap.insert( std::make_pair( key, vertex ));
        }
        facet.push_back( vertex );
      }
      triangles.push_back( new Facet( facet[0], facet[1], facet[2] ));
    }

    for( auto quad: quadsVec )
    {
      std::vector< VertexPtr > facet;
      for( auto index: quad.indices )
      {
        std::ostringstream oss;
        std::string key;
        oss << index.position << "," << index.normal << "," << index.uv;
        key = oss.str( );
        oss.str("");
        oss.clear();

        VertexPtr vertex;

        auto search = vertexMap.find( key );
        if ( search != vertexMap.end( ))
        {
          vertex = search->second;
        }
        else
        {
          Eigen::Vector3f position = positions[ index.position ];
          Eigen::Vector3f normal( 0.0f, 0.0f, 0.0f );
          Eigen::Vector3f color( 0.0f, 0.0f, 0.0f );
          Eigen::Vector2f uv( 0.0f, 0.0f );
          if ( index.normal >= 0 )
            normal = normals[ index.normal ];
          if ( index.uv >= 0 )
            uv = uvs[ index.uv ];
          vertex = new VERTEX( position, normal, color, uv );
          vertices.push_back( vertex );
          vertexMap.insert( std::make_pair( key, vertex ));
        }
        facet.push_back( vertex );
      }
      if ( quadsToTriangles_ )
      {
        triangles.push_back( new Facet( facet[0], facet[1], facet[2] ));
        triangles.push_back( new Facet( facet[0], facet[2], facet[3] ));
      }
      else
        quads.push_back( new Facet( facet[0], facet[1], facet[3], facet[2] ));
    }

    positions.clear( );
    normals.clear( );
    uvs.clear( );
    trianglesVec.clear( );
    quadsVec.clear( );
    vertexMap.clear( );
  }

  template < class VERTEX >
  MeshPtr ObjReaderTemplated< VERTEX >::readMesh( const std::string& fileName_,
                                               bool quadsToTriangles_ ) const
  {
    MeshPtr mesh = new Mesh( );
    readMesh( mesh, fileName_, quadsToTriangles_ );
    return mesh;
  }

  template < class VERTEX >
  std::vector< std::string >
  ObjReaderTemplated< VERTEX >::_split( std::string& s_, char splitter_ )
  {
    std::vector< std::string > strings;
    std::istringstream f( s_ );
    std::string s;
    while(( getline( f, s, splitter_ )))
    {
      strings.push_back( s );
    }
    return strings;
  }

  template < class VERTEX >
  tIndex ObjReaderTemplated< VERTEX >
  ::_splitStringToIndex( std::string& s_ )
  {
    tIndex index;
    std::vector< std::string > indexString = _split( s_, '/');

    index.position = std::stoi( indexString[0] ) - 1;
    index.uv = -1;
    index.normal = -1;

    if ( indexString.size( ) > 1 )
      if ( !indexString[1].empty( ))
        index.uv = std::stoi( indexString[1] ) - 1;
    if ( indexString.size( ) > 2 )
      if ( !indexString[2].empty( ))
        index.normal = std::stoi( indexString[2] ) - 1;

    return index;
  }

  template < class VERTEX >
  void ObjReaderTemplated< VERTEX >::_objToVectors(
    const std::string& fileName_,
    std::vector<Eigen::Vector3f>& positions_,
    std::vector<Eigen::Vector3f>& normals_,
    std::vector<Eigen::Vector2f>& uvs_,
    std::vector< tFacet >& triangles_,
    std::vector< tFacet >& quads_ )
  {
    std::fstream inFile( fileName_ );
    std::string line;

    while( std::getline( inFile, line ))
    {
      std::istringstream iss0( line );
      std::istringstream iss1( line );
      std::istringstream iss2( line );
      float paramX, paramY, paramZ;
      std::string type, facet0, facet1, facet2, facet3;
      tIndex index0, index1, index2, index3;

      if (( iss0 >> type >> paramX >> paramY >> paramZ ))
      {
        if ( type.compare( "v" ) == 0 )
          positions_.push_back( Eigen::Vector3f( paramX, paramY, paramZ ));
        else if ( type.compare( "vn" ) == 0 )
          normals_.push_back( Eigen::Vector3f( paramX, paramY, paramZ ));
        else if ( type.compare( "vt" ) == 0 )
          uvs_.push_back( Eigen::Vector2f( paramX, paramY ));
      }
      else if( iss0 >> type >> paramX >> paramY )
      {
        if( type.compare( "vt") == 0 )
          uvs_.push_back( Eigen::Vector2f( paramX, paramY ));
      }

      if(( iss1 >> type >> facet0 >> facet1 >> facet2 >> facet3 ))
      {
        if ( type.compare( "f" ) == 0 )
        {
          index0 = _splitStringToIndex( facet0 );
          index1 = _splitStringToIndex( facet1 );
          index2 = _splitStringToIndex( facet2 );
          index3 = _splitStringToIndex( facet3 );

          tFacet facet;
          facet.indices.resize( 4 );
          facet.indices[0] = index0;
          facet.indices[1] = index1;
          facet.indices[2] = index2;
          facet.indices[3] = index3;

          quads_.push_back( facet );
        }
      }
      else if(( iss2 >> type >> facet0 >> facet1 >> facet2 ))
      {
        if ( type.compare( "f" ) == 0 )
        {
          index0 = _splitStringToIndex( facet0 );
          index1 = _splitStringToIndex( facet1 );
          index2 = _splitStringToIndex( facet2 );

          tFacet facet;
          facet.indices.resize( 3 );
          facet.indices[0] = index0;
          facet.indices[1] = index1;
          facet.indices[2] = index2;

          triangles_.push_back( facet );
        }
      }
    }
  }

} // namespace nlgeometry

#endif
