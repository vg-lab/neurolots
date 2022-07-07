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
#ifndef __NLGENERATOR_JOINT_NODE__
#define __NLGENERATOR_JOINT_NODE__

#include "../nlgeometry/SectionQuad.h"

#include <nsol/nsol.h>

#include <nlgenerator/api.h>

namespace nlgenerator
{
  class JointNode;
  typedef JointNode* JointNodePtr;
  typedef std::vector< JointNodePtr > JointNodes;

  /* \class JointNode */
  class JointNode
  {
  public:

    /**
     * Constructor
     * @param position_ position of the joint node
     * @param radius_ radius of the joint node
     */
    NLGENERATOR_API
    JointNode( const Eigen::Vector3f& position_, float radius_ );

    /**
     * Default destructor
     */
    NLGENERATOR_API
    ~JointNode( void );

    /**
     * Method that returns the joint node position
     * @return the joint node position
     */
    NLGENERATOR_API
    Eigen::Vector3f& position( void );

    /**
     * Method that returns the join node radius
     * @return the joint node radius
     */
    NLGENERATOR_API
    float& radius( void );

    /**
     * Method that returns the connected soma conditional
     * @return the connected soma conditional
     */
    NLGENERATOR_API
    bool& connectedSoma( void );

    /**
     * Method that returns the corresponding section quad for each neighbour
     * nsol node
     * @param neighbour_ nsol neighbour node
     * @return the corresponding section quad
     */
    NLGENERATOR_API
    nlgeometry::SectionQuadPtr sectionQuad( nsol::NodePtr neighbour_ );

    /**
     * Method that returns the first section quad
     * @return the first section quad
     */
    NLGENERATOR_API
    nlgeometry::SectionQuadPtr sectionQuad( void );

    /**
     * Method that return the first neighbour node
     * @return the first neigbour node
     */
    NLGENERATOR_API
    nsol::NodePtr neighbour( void );

    /**
     * Method that returns the number of neighbors of the joint node
     * @return the number of neighbors of the joint node
     */
    NLGENERATOR_API
    unsigned int numberNeighbors( void );

    /**
     * Method that adds a new nsol neighbour node to the joint
     * @param neighbour_ nsol neighbour node
     */
    NLGENERATOR_API
    void addNeighbour( nsol::NodePtr neighbour_ );

    /**
     * Method that computes the joint node geometry
     */
    NLGENERATOR_API
    void computeGeometry( void );

  protected:

    //! Joint node position
    Eigen::Vector3f _position;

    //! Joint node radius
    float _radius;

    //! Conditional that indicates if the joint node is connected to the soma
    bool _connectedSoma;

    //! Joint node neighbour nodes
    std::unordered_map< nsol::NodePtr, nlgeometry::SectionQuadPtr > _neighbors;

  }; // class JointNode

} // namespace nlgenerator

#endif
