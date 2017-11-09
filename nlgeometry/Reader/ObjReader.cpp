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
#include "ObjReader.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace nlgeometry
{

  std::vector< std::string >
  ObjReader::_split( std::string& s_, char splitter_ )
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

  tIndex ObjReader::_splitStringToIndex( std::string& s_ )
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

  void ObjReader::_objToVectors( const char* fileName_,
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

} // mamespace nlgeometry
