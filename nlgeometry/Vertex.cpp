#include "Vertex.h"

namespace neurolots
{

  Vertex::Vertex(  Eigen::Vector3f position_, Eigen::Vector3f normal_ )
    : _id( 0 )
    , _position( position_ )
    , _normal( normal_ )
  {

  }

  Vertex::~Vertex( void )
  {

  }

  unsigned int& Vertex::id( void )
  {
    return _id;
  }

  Eigen::Vector3f& Vertex::position( void )
  {
    return _position;
  }

  Eigen::Vector3f& Vertex::normal( void )
  {
    return _normal;
  }

} // end namespace neurolots
