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

#include <Node.h>
#include <cfloat>

namespace neurolots
{
  class Quad
  {
    public:

      Quad( fem::NodePtr node0_, fem::NodePtr node1_, fem::NodePtr node2_,
            fem::NodePtr node3_ );
      ~Quad( void );

      //Getters
      fem::NodePtr Node0( void );
      fem::NodePtr Node1( void );
      fem::NodePtr Node2( void );
      fem::NodePtr Node3( void );

      Eigen::Vector3f Center( void );

      bool Free( void );

      //Setters
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
      Quads( void );
      ~Quads( void );

      static void FacesToVectors( std::vector< QuadPtr >& quads,
                                  std::vector< fem::NodePtr >& nodes,
                                  std::vector< float >& vertices,
                                  std::vector< float >& centers,
                                  std::vector< float >& tangents,
                                  std::vector< unsigned int >& mesh );

      static QuadPtr NearerQuad( std::vector< QuadPtr >& quads,
                                 Eigen::Vector3f position );

  };

} // end namespace neurolots

#endif // __NEUROLOTS_QUAD__

// EOF
