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
#include "ObjWriter.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace nlgeometry
{

  void ObjWriter::writeMesh( const MeshPtr mesh, const std::string& fileName_,
                             const std::string& headerString_  )
  {
    Facets facets;
    facets.insert( facets.end( ), mesh->triangles( ).begin(),
                   mesh->triangles( ).end( ));
    facets.insert( facets.end( ), mesh->quads( ).begin(),
                   mesh->quads( ).end( ));
    writeMesh( facets, mesh->vertices( ), fileName_, headerString_ );
  }

  void ObjWriter::writeMesh( const Facets& facets_, const Vertices& vertices_,
                             const std::string& fileName_,
                             const std::string& headerString_ )
  {
    std::ofstream outStream(fileName_.c_str());
    if(!outStream.is_open())
    {
      std::cerr <<  fileName_ << ": Error creating the file" << std::endl;
      return;
    }

    outStream << headerString_ << "\n" << std::endl;

    std::unordered_map< VertexPtr, unsigned int> vertexId;
    for ( unsigned int i = 0; i < ( unsigned int ) vertices_.size( ); i++ )
    {
      vertexId[ vertices_[i] ] = i + 1;
      outStream << "v " << vertices_[i]->position( ).x( ) << " "
                << vertices_[i]->position( ).y( ) << " "
                << vertices_[i]->position( ).z( ) << "\n";
    }
    for( nlgeometry::VertexPtr vertex: vertices_ )
    {
      outStream << "vn " << vertex->normal( ).x( ) << " "
                << vertex->normal( ).y( ) << " "
                << vertex->normal( ).z( ) << "\n";
    }
    for( nlgeometry::FacetPtr facet: facets_ )
    {
      if ( facet->vertex3( ))
      {
        outStream << "f " << vertexId[ facet->vertex0( )] << "//"
                  << vertexId[ facet->vertex0( )] << " "
                  << vertexId[ facet->vertex1( )] << "//"
                  << vertexId[ facet->vertex1( )] << " "
                  << vertexId[ facet->vertex2( )] << "//"
                  << vertexId[ facet->vertex2( )]
                  << "\n";
        outStream << "f " << vertexId[ facet->vertex1( )] << "//"
                  << vertexId[ facet->vertex1( )] << " "
                  << vertexId[ facet->vertex3( )] << "//"
                  << vertexId[ facet->vertex3( )] << " "
                  << vertexId[ facet->vertex2( )] << "//"
                  << vertexId[ facet->vertex2( )]
                  << "\n";
      }
      else
        outStream << "f " << vertexId[ facet->vertex0( )] << "//"
                  << vertexId[ facet->vertex0( )] << " "
                  << vertexId[ facet->vertex1( )] << "//"
                  << vertexId[ facet->vertex1( )] << " "
                  << vertexId[ facet->vertex2( )] << "//"
                  << vertexId[ facet->vertex2( )]
                  << "\n";
    }
    vertexId.clear( );
    outStream.close( );
  }

} // mamespace nlgeometry
