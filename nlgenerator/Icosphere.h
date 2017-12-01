/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/gmrvvis/neurolots>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef __NLGENERATOR_ICOSPHERE__
#define __NLGENERATOR_ICOSPHERE__

#include "../nlgeometry/SectionQuad.h"
#include "../nlphysics/Fem.h"

#include "JointNode.h"

#include <nlgenerator/api.h>


class MyPair
{
public:

  bool operator==( const MyPair& other_ )
  {
    return ( this->first_int == other_.first_int &&
             this->second_int == other_.second_int );
  }

  unsigned int first_int;
  unsigned int second_int;

};

namespace std {
  template<>
  struct hash<MyPair> {
  public:
    size_t operator()(const MyPair &myPair) const
    {
        size_t h1 = std::hash<unsigned int>()(myPair.first_int);
        size_t h2 = std::hash<unsigned int>()(myPair.second_int);
        return h1 ^ ( h2 << 1 );
    }
  };
}

namespace nlgenerator
{

  class Quad;
  typedef Quad* QuadPtr;
  typedef std::vector< QuadPtr > Quads;

  /* \class Quad */
  class Quad
  {

  public:

    /**
     * Constructor
     * @param node0_ first quad node
     * @param node1_ second quad node
     * @param node2_ third quad node
     * @param node3_ fourth quad node
     */
    NLGENERATOR_API
    Quad( nlphysics::NodePtr node0_, nlphysics::NodePtr node1_,
          nlphysics::NodePtr node2_, nlphysics::NodePtr node3_ )
      : _node0( node0_ )
      , _node1( node1_ )
      , _node2( node2_ )
      , _node3( node3_ )
    {
    }

    /**
     * Default destructor
     */
    ~Quad( void ) { }

    /**
     * Method that returns the reference to the first node
     * @return reference to the first node
     */
    nlphysics::NodePtr node0( void ){ return _node0; }

    /**
     * Method that returns the reference to the second node
     * @return reference to the second node
     */
    nlphysics::NodePtr node1( void ){ return _node1; }

    /**
     * Method that returns the reference to the third node
     * @return reference to the third node
     */
    nlphysics::NodePtr node2( void ){ return _node2; }


    /**
     * Method that returns the reference to the fourth node
     * @return reference to the fourth node
     */
    nlphysics::NodePtr node3( void ){ return _node3; }

  protected:

    //! Quad first node
    nlphysics::NodePtr _node0;

    //! Quad second node
    nlphysics::NodePtr _node1;

    //! Quad third node
    nlphysics::NodePtr _node2;

    //! Quad fourth node
    nlphysics::NodePtr _node3;

  };

  class Icosphere;
  typedef Icosphere* IcospherePtr;

  /* \class Icosphere */
  class Icosphere
  {

  public:

    /**
     * Default constructor
     */
    NLGENERATOR_API
    Icosphere( const Eigen::Vector3f& center_ =
               Eigen::Vector3f( 0.0f, 0.0f, 0.0f ),
               float radius_ = 1.0f,
               unsigned int subdivisionlevel_ = 3 );

    /**
     * Default destructor
     */
    NLGENERATOR_API
    ~Icosphere( );

    /**
     * Method that computes the final icospehere shape
     * @param joints_ joint nodes that conects to the icospehere
     */
    NLGENERATOR_API
    nlgeometry::Facets compute( const std::vector< JointNodePtr >& joints_ );

    /**
     * Method that return the final icoshepre shape as facets
     * @return the final icospehere shape as facets
     */
    NLGENERATOR_API
    nlgeometry::Facets surface( void );

    /**
     * Method that return the final icoshepre structure as facets
     * @return the final icospehere structure as facets
     */
    NLGENERATOR_API
    nlgeometry::Facets structure( void );


  protected:

    void _surface( nlgeometry::Facets& facets_,
                   std::unordered_map< unsigned int,
                   nlgeometry::OrbitalVertexPtr >& vertices_ );

    nlphysics::NodePtr _nearestSurfaceNode( const Eigen::Vector3f& point_ )
      const;

    QuadPtr _nearestSurfaceQuad( const Eigen::Vector3f& point_ ) const;

    void _subdivideSphere( unsigned int subdivisionlevel_ );

    void _computeCenters( void );

    nlphysics::NodePtr _middleNode(
      nlphysics::NodePtr node0_, nlphysics::NodePtr node1_,
      std::unordered_map< MyPair, nlphysics::NodePtr,
      std::hash<MyPair> >& newNodes_ );

    nlgeometry::OrbitalVertexPtr _nodeToVertex(
      nlphysics::NodePtr node_,
      std::unordered_map< unsigned int, nlgeometry::OrbitalVertexPtr >&
      vertices_ );

    //! Icosphere center position
    Eigen::Vector3f _center;

    //! Icosphere radius
    float _radius;

    //! Icosphere nodes
    nlphysics::Nodes _nodes;

    //! Icosphere surface nodes
    nlphysics::Nodes _surfaceNodes;

    //! Icosphere tetrahedra
    nlphysics::Tetrahedra _tetrahedra;

    //! Icosphere surface tetrahedra
    nlphysics::Tetrahedra _surfaceTetrahedra;

    //! Icosphere section quads
    Quads _surfaceQuads;

    //! Finite element method system
    nlphysics::Fem* _femSystem;

  }; // class Icosphere

} // namespace nlgenerator

#endif
