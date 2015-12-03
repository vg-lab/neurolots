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

#include "../nlfem/Node.h"
#include "../nlfem/Tetrahedron.h"
#include "../nlfem/Fem.h"

#include "Edges.h"
#include "Quad.h"
#include "VectorizedNode.h"

#include <iostream>
#include <vector>

#include <neurolots/nlgenerator/api.h>

namespace neurolots
{


  class Icosphere
  {

    public:

      NLGENERATOR_API
      Icosphere( Eigen::Vector3f center_, float radius_, unsigned int level  );

      NLGENERATOR_API
      ~Icosphere( void );

      NLGENERATOR_API
      void CalculateSoma( std::vector< VectorizedNodePtr >& firstNodes );

      NLGENERATOR_API
      void TrianglesToVector( std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

      NLGENERATOR_API
      void TrianglesToVector( std::vector< VectorizedNodePtr >& firstNodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

      NLGENERATOR_API
      void QuadsToVector( std::vector< float >& vertices,
                               std::vector< float >& centers,
                               std::vector< float >& tangents,
                               std::vector< unsigned int >& mesh );


    private:

      void _DivideSphere( unsigned int level );
      void _CalculateCenters( void );

      Eigen::Vector3f _center;
      float _radius;

      std::vector< fem::NodePtr > _nodes;
      std::vector< fem::TetrahedronPtr > _tetrahedra;
      std::vector< QuadPtr > _quads;

  };

} // end namespace neurolots

#endif // __NEUROLOTS_ICOSPHERE__

//EOF
