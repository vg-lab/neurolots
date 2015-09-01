/**
 * @file    Tetrahedron.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_QUAD__
#define __NEUROLOTS_QUAD__

#include "../nlfem/Node.h"
#include <cfloat>

#include <neurolots/nlgenerator/api.h>

namespace neurolots
{
  class Quad
  {
    public:

      NLGENERATOR_API
      Quad( fem::NodePtr node0_, fem::NodePtr node1_, fem::NodePtr node2_,
            fem::NodePtr node3_ );

      NLGENERATOR_API
      ~Quad( void );

      //Getters
      NLGENERATOR_API
      fem::NodePtr Node0( void );

      NLGENERATOR_API
      fem::NodePtr Node1( void );

      NLGENERATOR_API
      fem::NodePtr Node2( void );

      NLGENERATOR_API
      fem::NodePtr Node3( void );

      NLGENERATOR_API
      Eigen::Vector3f Center( void );

      NLGENERATOR_API
      bool Free( void );

      //Setters
      NLGENERATOR_API
      void Free( bool free_ );

    private:

      fem::NodePtr _node0;
      fem::NodePtr _node1;
      fem::NodePtr _node2;
      fem::NodePtr _node3;

      Eigen::Vector3f _center;

      bool _free;



  };

  typedef Quad* QuadPtr;

  class Quads
  {
    public:
      NLGENERATOR_API
      Quads( void );

      NLGENERATOR_API
      ~Quads( void );

      NLGENERATOR_API
      static void FacesToVectors( std::vector< QuadPtr >& quads,
                                  std::vector< fem::NodePtr >& nodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

      NLGENERATOR_API
      static QuadPtr NearerQuad( std::vector< QuadPtr >& quads,
                                 Eigen::Vector3f position );

  };

} // end namespace neurolots

#endif // __NEUROLOTS_QUAD__

// EOF
