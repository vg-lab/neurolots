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

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3                    Point;
typedef CGAL::Surface_mesh<Point>          Surface_mesh;

int main( int argc, char* argv[] )
{

  if ( argc < 2 )
  {
    std::cerr << "Error: Usage: " << argv[0] << " mesh_file[.off]" << std::endl;
    return -1;
  }

  try
  {
    std::string file( argv[1] );

    Surface_mesh surfaceMesh;
    std::ifstream input(file);

    if ( !input || !(input >> surfaceMesh) || ( surfaceMesh.is_empty()))
    {
      std::cerr << "File Error: " << file
                << " is not a .off file or is not two manifold mesh."
                << std::endl;
      input.close( );
      return -1;
    }
    CGAL::Polygon_mesh_processing::triangulate_faces(surfaceMesh);

    float area = 0.0f;
    area = CGAL::Polygon_mesh_processing::area( surfaceMesh );
    float volume = 0.0f;
    volume = CGAL::Polygon_mesh_processing::volume( surfaceMesh );

    input.close( );

    std::cout << "Area: " << area << "\nVolume: "
              << volume << std::endl;
  }
  catch ( const char* msg )
  {
    std::cerr << "Error: " << std::string(msg) << std::endl;
  }
  return 0;
}
