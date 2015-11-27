/**
 * @file    Facet.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_FACET__
#define __NEUROLOTS_FACET__

#include "Vertex.h"

#include <neurolots/nlgeometry/api.h>

namespace neurolots
{
  class Facet
  {
  public:

    Facet( VertexPtr v0_, VertexPtr v1_, VertexPtr v2_ );
    ~Facet( void );

    // GETTERS

    VertexPtr& v0( void );
    VertexPtr& v1( void );
    VertexPtr& v2( void );

    //SETTERS

  private:

    VertexPtr _v0;
    VertexPtr _v1;
    VertexPtr _v2;

  };

  typedef Facet* FacetPtr;
  typedef std::vector< FacetPtr > Facets;

} // end namespace neurolots

#endif // __NEUROLOTS_FACET__

// EOF
