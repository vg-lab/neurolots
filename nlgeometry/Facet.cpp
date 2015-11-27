#include "Facet.h"

namespace neurolots
{

  Facet::Facet(  VertexPtr v0_, VertexPtr v1_, VertexPtr v2_ )
    : _v0( v0_ )
    , _v1( v1_ )
    , _v2( v2_ )
  {

  }

  Facet::~Facet( void )
  {

  }

  VertexPtr& Facet::v0( void )
  {
    return _v0;
  }

  VertexPtr& Facet::v1( void )
  {
    return _v1;
  }

  VertexPtr& Facet::v2( void )
  {
    return _v2;
  }

} // end namespace neurolots
