/**
 * Copyright (c) 2015-2019 GMRV/URJC.
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

#include <iostream>
#include <iomanip>
#include <chrono>

#include <nlgeometry/nlgeometry.h>

void usageError( char* argv[ ] )
{
  std::cerr << "Usage error: " << argv[0]
            << " targetSamples[int] file0[.obj] file1[.obj]"
            << std::endl;
}

int main( int argc, char* argv[ ])
{
  if ( argc != 4 )
  {
    usageError( argv );
    return -1;
  }

  unsigned int targetSamples = std::stoi( std::string( argv[1] ));
  nlgeometry::ObjReader objr;
  auto mesh0 = objr.readMesh( std::string( argv[2] ), false );
  auto mesh1 = objr.readMesh( std::string( argv[3] ), false );

  if ( !mesh0 || !mesh1 )
  {
    usageError( argv );
    return -1;
  }

  std::cout << "Loaded mesh " << std::string( argv[1] ) << "\n\twith "
            << mesh0->vertices( ).size( ) << " vertices and "
            << mesh0->triangles( ).size( ) << " facets."
            << std::endl;
  std::cout << "Loaded mesh " << std::string( argv[2] ) << "\n\twith "
            << mesh1->vertices( ).size( ) << " vertices and "
            << mesh1->triangles( ).size( ) << " facets."
            << std::endl;

  float min, max, mean, rms;
  unsigned int samples0, samples1;

  auto startTime = std::chrono::system_clock::now( );
  nlgeometry::MeshMetrics::hausdorffDistance( min, max,
                                              mean, rms,
                                              samples0, samples1,
                                              mesh0, mesh1, targetSamples );
  auto endTime = std::chrono::system_clock::now( );
  auto duration = std::chrono::duration_cast< std::chrono::microseconds >
    ( endTime - startTime ).count( ) * 0.001f;
  std::cout << "Hausdorff Distance computed in " << std::fixed
            << std::setprecision(3) << (float)duration << " miliseconds"
            << std::endl;
  std::cout << "number of samples mesh0: " << samples0
            << " number of samples mesh1: " << samples1 << std::endl;
  std::cout << "\tmin: " << std::fixed << std::setprecision(6) << min
            << " max: " << std::fixed << std::setprecision(6) << max
            << " mean: " << std::fixed << std::setprecision(6) << mean
            << " rms: " << std::fixed << std::setprecision(6) << rms
            << std::endl;

  return 0;

}
