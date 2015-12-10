/**
 * @file    Facet.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLGEOMETRY_FACET__
#define __NLGEOMETRY_FACET__

#include "Vertex.h"

#include <nlgeometry/api.h>

namespace nlgeometry
{
  class Facet
  {
  public:

    NLGEOMETRY_API
    Facet( const VertexPtr& v0_, const VertexPtr& v1_, const VertexPtr& v2_ );

    NLGEOMETRY_API
    ~Facet( void );

    // GETTERS
    NLGEOMETRY_API
    VertexPtr& v0( void );

    NLGEOMETRY_API
    VertexPtr& v1( void );

    NLGEOMETRY_API
    VertexPtr& v2( void );

    NLGEOMETRY_API
    VertexPtr v0( void ) const;

    NLGEOMETRY_API
    VertexPtr v1( void ) const;

    NLGEOMETRY_API
    VertexPtr v2( void ) const;


    //SETTERS

  private:

    VertexPtr _v0;
    VertexPtr _v1;
    VertexPtr _v2;

  };

  typedef Facet* FacetPtr;
  typedef std::vector< FacetPtr > Facets;

} // end namespace nlgeometry

#endif // __NLGEOMETRY_FACET__

// EOF
