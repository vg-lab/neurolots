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
#include "ObjWriter.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace nlgeometry
{

  void ObjWriter::writeMesh( MeshPtr mesh, const std::string& fileName_ )
  {
    Facets facets;
    facets.insert( facets.end( ), mesh->triangles( ).begin(),
                   mesh->triangles( ).end( ));
    facets.insert( facets.end( ), mesh->quads( ).begin(),
                   mesh->quads( ).end( ));
    writeMesh( facets, mesh->vertices( ), fileName_ );
  }

  void ObjWriter::writeMesh( Facets& facets_, Vertices& vertices_,
                  const std::string& fileName_ )
  {
    std::ofstream outStream(fileName_.c_str());
    if(!outStream.is_open())
    {
      std::cerr <<  fileName_ << ": Error creating the file" << std::endl;
      return;
    }

    for ( unsigned int i = 0; i < ( unsigned int ) vertices_.size( ); i++ )
    {
      vertices_[i]->id( ) = i + 1;
      outStream << "v " << vertices_[i]->position( ).x( ) << " "
                << vertices_[i]->position( ).y( ) << " "
                << vertices_[i]->position( ).z( ) << std::endl;
    }
    for( nlgeometry::VertexPtr vertex: vertices_ )
    {
      outStream << "vn " << vertex->normal( ).x( ) << " "
                << vertex->normal( ).y( ) << " "
                << vertex->normal( ).z( ) << std::endl;
    }
    for( nlgeometry::FacetPtr facet: facets_ )
    {
      outStream << "f " << facet->vertex0( )->id( ) << "//"
                <<  facet->vertex0( )->id( ) << " " << facet->vertex1( )->id( )
                << "//" <<  facet->vertex1( )->id( ) << " "
                << facet->vertex2( )->id( ) << "//" <<  facet->vertex2( )->id( )
                << std::endl;
    }

    outStream.close();
  }

} // mamespace nlgeometry
