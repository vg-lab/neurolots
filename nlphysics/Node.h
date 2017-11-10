/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of nsol <https://github.com/gmrvvis/neurolots>
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

#ifndef __NLPHYSICS_NODE__
#define __NLPHYSICS_NODE__

#include <Eigen/Dense>

#include <vector>

#include <nlphysics/api.h>

namespace nlphysics
{

  class Node;
  typedef Node* NodePtr;
  typedef std::vector< NodePtr > Nodes;

  /* \class Node */
  class Node
  {

  public:

    /**
     * Constructor
     * @param intialPosition_ vector with the initial position
     * @param id_ node index
     * @param contour_ condition of inner or contour node
     */
    NLPHYSICS_API
    Node( Eigen::Vector3f& initialPostition_, unsigned int id_,
          bool contour_ = false );

    /**
     * Default destructor
     */
    NLPHYSICS_API
    ~Node( void );

    /**
     * Method that returns the node id
     * @return the node id
     */
    NLPHYSICS_API
    unsigned int& id( void );

    NLPHYSICS_API
    unsigned int id( void ) const;

    /**
     * Method that returns the node initial position
     * @return the node initial position
     */
    NLPHYSICS_API
    Eigen::Vector3f& initialPosition( void );

    NLPHYSICS_API
    Eigen::Vector3f initialPosition( void )  const;

    /**
     * Method that returns the node displacement
     * @return the node displacement
     */
    NLPHYSICS_API
    Eigen::Vector3f& displacement( void );

    NLPHYSICS_API
    Eigen::Vector3f displacement( void )  const;

    /**
     * Method that returns the node position
     * @return the node position
     */
    NLPHYSICS_API
    Eigen::Vector3f& position( void );

    NLPHYSICS_API
    Eigen::Vector3f position( void )  const;

    /**
     * Method that returns the node center
     * @return the node center
     */
    NLPHYSICS_API
    Eigen::Vector3f& center( void );

    NLPHYSICS_API
    Eigen::Vector3f center( void )  const;

    /**
     * Method that returns the node tangent
     * @return the node tangent
     */
    NLPHYSICS_API
    Eigen::Vector3f& tangent( void );

    NLPHYSICS_API
    Eigen::Vector3f tangent( void )  const;

    /**
     * Method that returns the node force
     * @return the node force
     */
    NLPHYSICS_API
    Eigen::Vector3f& force( void );

    NLPHYSICS_API
    Eigen::Vector3f force( void )  const;

    /**
     * Method that returns the node fixed condition
     * @return the node fixed condition
     */
    NLPHYSICS_API
    bool& fixed( void );

    NLPHYSICS_API
    bool fixed( void )  const;

    /**
     * Method that returns the node contour condition
     * @return the node contour condition
     */
    NLPHYSICS_API
    bool& contour( void );

    NLPHYSICS_API
    bool contour( void )  const;

  protected:

    //! Index
    unsigned int _id;

    //! Initial position
    Eigen::Vector3f _initialPosition;

    //! Displacement
    Eigen::Vector3f _displacement;

    //! Actual position
    Eigen::Vector3f _position;

    //! Node center
    Eigen::Vector3f _center;

    //! Node tangetn
    Eigen::Vector3f _tangent;

    //! Force
    Eigen::Vector3f _force;

    //! Condition if node is fixed or not
    bool _fixed;

    //! Condition that indicates if the node is a inner node or a contr
    bool _contour;

  }; // class Node

} // namespace nlphysics

#endif
