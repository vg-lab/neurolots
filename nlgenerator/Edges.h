/**
 * @file    Edges.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLGENERATOR_EDGE__
#define __NLGENERATOR_EDGE__

//Eigen

#include "../nlfem/Node.h"

#include <iostream>
#include <vector>

#include <nlgenerator/api.h>

namespace nlgenerator
{


  class Edge
  {

    public:

      NLGENERATOR_API
      Edge( unsigned int node_, nlfem::NodePtr middleNode_ );

      NLGENERATOR_API
      ~Edge( void );

      //Getters
      NLGENERATOR_API
      unsigned int Node( void );

      NLGENERATOR_API
      nlfem::NodePtr MiddleNode( void );

    private:

      unsigned int _node;
      nlfem::NodePtr _middleNode;

  };

  class Edges
  {
    public:
      NLGENERATOR_API
      Edges( Eigen::Vector3f _center, float _radius );

      NLGENERATOR_API
      ~Edges( void );

      NLGENERATOR_API
      nlfem::NodePtr AddEdge( std::vector< nlfem::NodePtr >& nodes,
                              nlfem::NodePtr node0, nlfem::NodePtr node1 );

      NLGENERATOR_API
      void Clear( void );

    private:

      nlfem::NodePtr _MiddleNode( std::vector< nlfem::NodePtr >& nodes,
                                  nlfem::NodePtr node0, nlfem::NodePtr node1);

      Eigen::Vector3f _center;
      float _radius;
      std::vector< std::vector< Edge >> _edges;

  };



} // end namespace nlgenerator

#endif // __NLGENERATOR_EDGE__

//EOF
