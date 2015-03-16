/**
 * @file    Icospehere.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_ICOSPHERE__
#define __NEUROLOTS_ICOSPHERE__

//Eigen
#include <Eigen/Dense>

#include <Node.h>
#include <Tetrahedron.h>

#include "Edges.h"

#include <iostream>
#include <vector>


namespace neurolots
{


  class Icosphere
  {

    public:

      Icosphere( Eigen::Vector3f center_, float radius_, int level  );
      ~Icosphere( void );

      void PassToVector( std::vector< float >& vertices,
                         std::vector< float >& centers,
                         std::vector< unsigned int >& mesh );
      void PassContornToVector( std::vector< float >& vertices,
                                std::vector< float >& centers,
                                std::vector< unsigned int >& mesh );

    private:

      void _DivideSphere( int level );

      Eigen::Vector3f _center;
      float _radius;

      std::vector< fem::NodePtr > _nodes;
      std::vector< fem::TetrahedronPtr > _tetrahedra;

  };

} // end namespace neurolots

#endif // __NEUROLOTS_ICOSPHERE__

//EOF
