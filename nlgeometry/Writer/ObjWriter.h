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
#ifndef __NLGEOMETRY_OBJ_WRITER__
#define __NLGEOMETRY_OBJ_WRITER__

#include "../Mesh.h"

#include <nlgeometry/api.h>

namespace nlgeometry
{

  /* \class ObjWriter */
  class ObjWriter
  {

  public:

    /**
     * Static method to write mesh to a obj file
     */
    static void writeMesh( MeshPtr mesh, const std::string& fileName_ );

    /**
     * Static method to write a vector of facets and vertices to a obj file
     */
    static void writeMesh( Facets& facets_, Vertices& vertices_,
                           const std::string& fileName_ );

  }; // class ObjWriter

} // namespace nlgeometry

#endif
