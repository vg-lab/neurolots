/**
 * Copyright (c) 2015-2017 VG-Lab/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/vg-lab/neurolots>
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
#ifndef __NLPHYSICS_TETRAHEDRON__
#define __NLPHYSICS_TETRAHEDRON__

#include "Node.h"

#include <nlphysics/api.h>

namespace nlphysics
{

  class Tetrahedron;
  typedef Tetrahedron* TetrahedronPtr;
  typedef std::vector< TetrahedronPtr > Tetrahedra;

  /* \class Tetrahedron */
  class Tetrahedron
  {

  public:

    /**
     * Constructor
     * @param node0_ fist tetrahedron node
     * @param node1_ second tetrahedron node
     * @param node2_ third tetrahedron node
     * @param node3_ fourth tetrahedron node
     */
    NLPHYSICS_API
    Tetrahedron( NodePtr node0_, NodePtr node1_, NodePtr node2_,
                 NodePtr node3_ )
      : _node0( node0_ )
      , _node1( node1_ )
      , _node2( node2_ )
      , _node3( node3_ )
      , _volume( 0.0f )
    {

    }

    /**
     * Default destructor
     */
    NLPHYSICS_API
    ~Tetrahedron( void ) { }
    /**
     * Method that returns the first tetrahedron node
     * @return the first tetrahedron node
     */
    NLPHYSICS_API
    NodePtr& node0( void ) {  return _node0; }

    /**
     * Method that returns the second tetrahedron node
     * @return the second tetrahedron node
     */
    NLPHYSICS_API
    NodePtr& node1( void ) { return _node1; }

    /**
     * Method that returns the third tetrahedron node
     * @return the third tetrahedron node
     */
    NLPHYSICS_API
    NodePtr& node2( void ) { return _node2; }

    /**
     * Method that returns the fourth tetrahedron node
     * @return the fourth tetrahedron node
     */
    NLPHYSICS_API
    NodePtr& node3( void ) { return _node3; }

    /**
     * Method that returns the b0 matrix
     * @return the b0 matrix
     */
    NLPHYSICS_API
    Eigen::MatrixXf& b0( void ) { return _b0; }

    /**
     * Method that returns the b1 matrix
     * @return the b1 matrix
     */
    NLPHYSICS_API
    Eigen::MatrixXf& b1( void ) { return _b1; }

    /**
     * Method that returns the b2 matrix
     * @return the b2 matrix
     */
    NLPHYSICS_API
    Eigen::MatrixXf& b2( void ) { return _b2; }

    /**
     * Method that returns the b3 matrix
     * @return the b3 matrix
     */
    NLPHYSICS_API
    Eigen::MatrixXf& b3( void ) { return _b3; }

    /**
     * Method that returns the tetrahedron volume
     * @return the tetrahedron volume
     */
    NLPHYSICS_API
    float& volume( void ) { return _volume; }

    /**
     * Method that check and return if the 0 face of the tetrahedron is a
     * contour face
     * @return true if the 0 face of the tetrahedron is a contour face
     */
    NLPHYSICS_API
    bool face0( void ) const
    {
      return _node0->contour( ) && _node1->contour( ) && _node2->contour( );
    }

    /**
     * Method that check and return if the 1 face of the tetrahedron is a
     * contour face
     * @return true if the 1 face of the tetrahedron is a contour face
     */
    NLPHYSICS_API
    bool face1( void ) const
    {
      return _node0->contour( ) && _node2->contour( ) && _node3->contour( );
    }

    /**
     * Method that check and return if the 2 face of the tetrahedron is a
     * contour face
     * @return true if the 2 face of the tetrahedron is a contour face
     */
    NLPHYSICS_API
    bool face2( void ) const
    {
      return _node0->contour( ) && _node3->contour( ) && _node1->contour( );
    }

    /**
     * Method that check and return if the 3 face of the tetrahedron is a
     * contour face
     * @return true if the 3 face of the tetrahedron is a contour face
     */
    NLPHYSICS_API
    bool face3( void ) const
    {
      return _node1->contour( ) && _node3->contour( ) && _node2->contour( );
    }

  protected:

    //! First tetrahedron node
    NodePtr _node0;

    //! Second tetrahedron node
    NodePtr _node1;

    //! Third tetrahedron node
    NodePtr _node2;

    //! Fourth tetrahedron node
    NodePtr _node3;

    //! b0 matrix
    Eigen::MatrixXf _b0;

    //! b1 matrix
    Eigen::MatrixXf _b1;

    //! b2 matrix
    Eigen::MatrixXf _b2;

    //! b3 matrix
    Eigen::MatrixXf _b3;

    //! tetraheron volume
    float _volume;

  }; // class Tetrahedron

} // namespace nlphysics

#endif
