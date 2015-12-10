/**
 * @file    Vertex.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLGEOMETRY_VERTEX__
#define __NLGEOMETRY_VERTEX__

#include <Eigen/Dense>
#include <vector>

#include <nlgeometry/api.h>

namespace nlgeometry
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

} // end namespace nlgeometry

#endif // __NLGEOMETRY_VERTEX__

// EOF
