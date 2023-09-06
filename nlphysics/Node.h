/**
 * Copyright (c) 2015-2017 VG-Lab/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neuroltos <https://github.com/vg-lab/neurolots>
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
    Node( const Eigen::Vector3f& initialPosition_, unsigned int id_ = 0,
          bool contour_ = false )
      : _id( id_ )
      , _initialPosition( initialPosition_ )
      , _displacement( Eigen::Vector3f::Zero( ))
      , _position( initialPosition_ )
      , _center( Eigen::Vector3f::Zero( ))
      , _tangent( Eigen::Vector3f::Zero( ))
      , _force( Eigen::Vector3f::Zero( ))
      , _fixed( false )
      , _contour( contour_ )
    {

    }

    /**
     * Default destructor
     */
    NLPHYSICS_API
    ~Node( void ) { }

    /**
     * Method that returns the node id
     * @return the node id
     */
    NLPHYSICS_API
    unsigned int& id( void ) { return _id; }

    NLPHYSICS_API
    unsigned int id( void ) const { return _id; }

    /**
     * Method that returns the node initial position
     * @return the node initial position
     */
    NLPHYSICS_API
    Eigen::Vector3f& initialPosition( void ) { return _initialPosition; }

    NLPHYSICS_API
    const Eigen::Vector3f& initialPosition( void )  const
    { return _initialPosition; }

    /**
     * Method that returns the node displacement
     * @return the node displacement
     */
    NLPHYSICS_API
    Eigen::Vector3f& displacement( void ) { return _displacement; }

    NLPHYSICS_API
    const Eigen::Vector3f& displacement( void )  const { return _displacement; }

    /**
     * Method that returns the node position
     * @return the node position
     */
    NLPHYSICS_API
    Eigen::Vector3f& position( void ) { return _position; }

    NLPHYSICS_API
    const Eigen::Vector3f& position( void )  const { return _position; }

    /**
     * Method that returns the node center
     * @return the node center
     */
    NLPHYSICS_API
    Eigen::Vector3f& center( void ) { return _center; }

    NLPHYSICS_API
    const Eigen::Vector3f& center( void )  const { return _center; }

    /**
     * Method that returns the node tangent
     * @return the node tangent
     */
    NLPHYSICS_API
    Eigen::Vector3f& tangent( void ) { return _tangent; }

    NLPHYSICS_API
    const Eigen::Vector3f& tangent( void )  const { return _tangent; }

    /**
     * Method that returns the node force
     * @return the node force
     */
    NLPHYSICS_API
    Eigen::Vector3f& force( void ) { return _force; }

    NLPHYSICS_API
    const Eigen::Vector3f& force( void )  const { return _force; }

    /**
     * Method that returns the node fixed condition
     * @return the node fixed condition
     */
    NLPHYSICS_API
    bool& fixed( void ) { return _fixed; }

    NLPHYSICS_API
    bool fixed( void )  const { return _fixed; }

    /**
     * Method that returns the node contour condition
     * @return the node contour condition
     */
    NLPHYSICS_API
    bool& contour( void ) { return _contour; }

    NLPHYSICS_API
    bool contour( void )  const { return _contour; }

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
