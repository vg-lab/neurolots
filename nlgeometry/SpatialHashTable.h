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
#ifndef __NLGEOMETRY_SPATIAL_HASH_TABLE__
#define __NLGEOMETRY_SPATIAL_HASH_TABLE__

#include "Vertex.h"

#include <nlgeometry/api.h>

namespace nlgeometry
{

  /* \class SpatialHashTable */
  class SpatialHashTable
  {

  public:

    /**
     * Default Constructor
     * @param size_ number of Spatial Hash Table cells
     * @param cellSize_ size of the cells
     * @param tolerance_ vertices position comparison tolerance
     * @param prime0_ X axis prime number of the hash function
     * @param prime1_ Y axis prime number of the hash function
     * @param prime2_ Z axis prime number of the hash function
     */
    NLGEOMETRY_API
    SpatialHashTable( unsigned int size_ = 100000,
                      float cellSize_ = 0.1f,
                      float tolerance_ = 0.00001f,
                      unsigned int primeX_ = 73856093,
                      unsigned int primeY_ = 19349663,
                      unsigned int primeZ_ = 83492791 );

    /**
     * Default destructor
     */
    NLGEOMETRY_API
    ~SpatialHashTable( void );

    /**
     * Method that return a pointer to a vertex in the table with the same
     * position of the given vertex if the is not vertex with the same position
     * the given vertex will be inserted in the table and returned
     * @param vertex_ vertex pointer to be inserted in the table
     * @return vertex pointer to the inserted vertex or equivalent
     */
    NLGEOMETRY_API
    VertexPtr insert( const VertexPtr& vertex_ );

    /**
     * Method that return a vector of the vertices contained in the table
     * @return a list of vector of vertices contained in the table
     */
    NLGEOMETRY_API
    void vertices( Vertices& vertices_ ) const;

  private:

    bool _equal( const VertexPtr v0, const VertexPtr v1 ) const;

    //! Number of cells in the table
    unsigned int _size;

    //! Size of the cells of the table
    float _cellSize;

    //! Vertices position comparison tolerance
    float _tolerance;

    //! X axis prime number of the hash function
    unsigned int _primeX;

    //! Y axis prime number of the hash function
    unsigned int _primeY;

    //! Z axis prime number of the hash function
    unsigned int _primeZ;

    //! Table
    std::vector< Vertices > _table;

  }; // class SpatialHashTable

} // namespace nlgeometry

#endif
