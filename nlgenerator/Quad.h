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

#include <neurolots/api.h>

namespace neurolots
{
  class Quad
  {
    public:

      NEUROLOTS_API
      Quad( fem::NodePtr node0_, fem::NodePtr node1_, fem::NodePtr node2_,
            fem::NodePtr node3_ );

      NEUROLOTS_API
      ~Quad( void );

      //Getters
      NEUROLOTS_API
      fem::NodePtr Node0( void );

      NEUROLOTS_API
      fem::NodePtr Node1( void );

      NEUROLOTS_API
      fem::NodePtr Node2( void );

      NEUROLOTS_API
      fem::NodePtr Node3( void );

      NEUROLOTS_API
      Eigen::Vector3f Center( void );

      NEUROLOTS_API
      bool Free( void );

      //Setters
      NEUROLOTS_API
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
      NEUROLOTS_API
      Quads( void );

      NEUROLOTS_API
      ~Quads( void );

      NEUROLOTS_API
      static void FacesToVectors( std::vector< QuadPtr >& quads,
                                  std::vector< fem::NodePtr >& nodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

      NEUROLOTS_API
      static QuadPtr NearerQuad( std::vector< QuadPtr >& quads,
                                 Eigen::Vector3f position );

  };

} // end namespace neurolots

#endif // __NEUROLOTS_QUAD__

// EOF
