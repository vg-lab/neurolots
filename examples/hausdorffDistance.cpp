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
#include <CGAL/Polygon_mesh_processing/distance.h>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Surface_mesh;

int main( int argc, char* argv[] )
{

  if ( argc < 3 )
  {
    std::cerr << "Error: Usage: " << argv[0]
              << " mesh_file[.off] mesh_file[.off] [sample per unit]"
              << std::endl;
    return -1;
  }

  int samples = 100;

  try
  {
    std::string meshFile0( argv[1] );
    std::string meshFile1( argv[2] );

    if ( argc > 3 )
      samples = atoi( argv[3]);

    Surface_mesh surfaceMesh0;
    std::ifstream input0(meshFile0);

    Surface_mesh surfaceMesh1;
    std::ifstream input1(meshFile1);

    if ( !input0 || !(input0 >> surfaceMesh0) || ( surfaceMesh0.is_empty()))
    {
      std::cerr << "File Error: " << meshFile0
                << " is not a .off file or is not two manifold mesh."
                << std::endl;
      input0.close( );
      return -1;
    }

    if ( !input1 || !(input1 >> surfaceMesh1) || ( surfaceMesh1.is_empty()))
    {
      std::cerr << "File Error: " << meshFile1
                << " is not a .off file or is not two manifold mesh."
                << std::endl;
      input1.close( );
      return -1;
    }


    CGAL::Polygon_mesh_processing::triangulate_faces(surfaceMesh0);
    CGAL::Polygon_mesh_processing::triangulate_faces(surfaceMesh1);

    std::cout << CGAL::Polygon_mesh_processing::approximate_Hausdorff_distance
      <CGAL::Sequential_tag>(
      surfaceMesh0, surfaceMesh1,
      CGAL::Polygon_mesh_processing::parameters::number_of_points_per_area_unit(samples),
      CGAL::Polygon_mesh_processing::parameters::number_of_points_per_area_unit(samples))

      
      // CGAL::Polygon_mesh_processing::parameters::number_of_points_per_area_unit(samples),
      // CGAL::Polygon_mesh_processing::parameters::number_of_points_per_area_unit(samples
              << std::endl;

    input0.close( );
    input1.close( );
  }
  catch ( const char* msg )
  {
    std::cerr << "Error: " << std::string(msg) << std::endl;
  }
  return 0;
}
