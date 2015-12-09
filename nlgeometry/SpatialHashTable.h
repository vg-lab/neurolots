/**
 * @file    SpatialHashtable.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLGEOMETRY_SPATIAL_HASH_TABLE__
#define __NLGEOMETRY_SPATIAL_HASH_TABLE__

#include "Vertex.h"

#include <neurolots/nlgeometry/api.h>

namespace nlgeometry
{
  class SpatialHashTable
  {
  public:

    NLGEOMETRY_API
    SpatialHashTable( unsigned int size_ = 1000,
                      float cellSize_ = 0.1f,
                      float tolerance_ = 0.00001f,
                      unsigned int prime1_ = 73856093,
                      unsigned int prime2_ = 19349663,
                      unsigned int prime3_ = 83492791 );

    NLGEOMETRY_API
    ~SpatialHashTable( void );

    NLGEOMETRY_API
    VertexPtr insert( const VertexPtr& vertex_ );

    NLGEOMETRY_API
    void vertices( Vertices& vertices_ ) const;

  private:

    bool _equal( const VertexPtr v0, const VertexPtr v1 ) const;

    unsigned int _size;
    float _cellSize;
    float _tolerance;
    unsigned int _prime1;
    unsigned int _prime2;
    unsigned int _prime3;

    std::vector< Vertices > _table;

  };

} // end namespace neurolots

#endif // __NLGEOMETRY_SPATIAL_HASH__

// EOF
