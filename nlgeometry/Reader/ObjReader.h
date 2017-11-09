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
#ifndef __NLGEOMETRY_OBJ_READER__
#define __NLGEOMETRY_OBJ_READER__

#include "../Mesh.h"
#include <unordered_map>

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


  /* \class ObjReader */
  class ObjReader
  {

  public:

    /**
     * Static method that fill the referenced mesh
     */
    template < class VERTEX = Vertex >
    static void readMesh( MeshPtr mesh, const std::string& fileName_,
                          bool quadsToTriangles_ = true )
    {
      std::vector< Eigen::Vector3f > positions;
      std::vector< Eigen::Vector3f > normals;
      std::vector< Eigen::Vector2f > uvs;

      std::vector< tFacet > trianglesVec;
      std::vector< tFacet > quadsVec;

      _objToVectors( fileName_.c_str( ), positions, normals, uvs,
                     trianglesVec, quadsVec );

      Vertices& vertices = mesh->vertices( );
      Facets& triangles = mesh->triangles( );
      Facets& quads = mesh->quads( );

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


    /**
     * Static method that return a mesh pointer with the geometric data loaded
     */
    template < class VERTEX = Vertex >
    static MeshPtr readMesh( const std::string& fileName_,
                             bool quadsToTriangles_ = true )
    {
      MeshPtr mesh = new Mesh( );
      readMesh< VERTEX >( mesh, fileName_, quadsToTriangles_ );
      return mesh;
    }

  private:

    static std::vector< std::string > _split( std::string& s_, char splitter_ );

    static tIndex _splitStringToIndex( std::string& s_ );

    static void _objToVectors( const char* fileName_,
                               std::vector< Eigen::Vector3f >& positions_,
                               std::vector< Eigen::Vector3f >& normals_,
                               std::vector< Eigen::Vector2f >& uvs_,
                               std::vector< tFacet >& triangles_,
                               std::vector< tFacet >& quads_ );
  }; // class ObjReader

} // namespace nlgeometry

#endif
