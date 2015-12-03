/**
 * @file    Vertex.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_VERTEX__
#define __NEUROLOTS_VERTEX__

#include <Eigen/Dense>
#include <vector>

#include <neurolots/nlgeometry/api.h>

namespace neurolots
{
  class Vertex
  {
  public:

    NLGEOMETRY_API
    Vertex( const Eigen::Vector3f& position_, const Eigen::Vector3f& normal_ );

    NLGEOMETRY_API
    ~Vertex( void );

    // GETTERS

    NLGEOMETRY_API
    unsigned int& id( void );

    NLGEOMETRY_API
    Eigen::Vector3f& position( void );

    NLGEOMETRY_API
    Eigen::Vector3f& normal( void );

    //SETTERS

  private:

    unsigned int _id;
    Eigen::Vector3f _position;
    Eigen::Vector3f _normal;
  };

  typedef Vertex* VertexPtr;

  typedef std::vector< VertexPtr > Vertices;

} // end namespace neurolots

#endif // __NEUROLOTS_VERTEX__

// EOF
