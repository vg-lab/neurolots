/**
 * @file    Vertex.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "Vertex.h"

namespace nlgeometry
{

  Vertex::Vertex(  const Eigen::Vector3f& position_,
                   const Eigen::Vector3f& normal_ )
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
