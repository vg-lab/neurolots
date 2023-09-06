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
#ifndef __NLGEOMETRY_ORBITAL_VERTEX__
#define __NLGEOMETRY_ORBITAL_VERTEX__

#include "Vertex.h"

#include <nlgeometry/api.h>

namespace nlgeometry
{

  class OrbitalVertex;
  typedef OrbitalVertex* OrbitalVertexPtr;

  class OrbitalVertex
    : public Vertex
  {

  public:

    /**
     * Constructor
     * @param position_ vertex position
     * @param pivot_ vertex pivot
     * @param tangent_ vertex tanget
     * @param color_ vertex color
     */
    NLGEOMETRY_API
    OrbitalVertex(
      const Eigen::Vector3f& position_,
      const Eigen::Vector3f& center_ = Eigen::Vector3f( 0.0f, 0.0f, 0.0f ),
      const Eigen::Vector3f& tangent_ = Eigen::Vector3f( 0.0f, 0.0f, 0.0f ),
      const Eigen::Vector3f& color_ = Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));

    /**
     * Copy constructor
     * @param other_ vertex to be copied
     */
    NLGEOMETRY_API
    OrbitalVertex( const OrbitalVertex& other_ );

    /**
     * Default constructor
     */
    NLGEOMETRY_API
    virtual ~OrbitalVertex( );

    /**
     * Method that returns the orbital vertex center
     */
    NLGEOMETRY_API
    Eigen::Vector3f& center( void );

    NLGEOMETRY_API
    const Eigen::Vector3f& center( void ) const;

    /**
     * Method that returns the orbital vertex tangent
     */
    NLGEOMETRY_API
    Eigen::Vector3f& tangent( void );

    NLGEOMETRY_API
    const Eigen::Vector3f& tangent( void ) const;

    /**
     * Method to displace the orbital vertex position
     * @param displacement_ displacement applied to the orbital vertex position
     */
    NLGEOMETRY_API
    virtual void displace( const Eigen::Vector3f& displacement_ );

    /**
     * Method to place the orbital vertex position
     * @param placement_ placement to place the orbital vertex position
     */
    NLGEOMETRY_API
    virtual void place( const Eigen::Vector3f& placement_ );

    /**
     * Method to rotate the orbital vertex position and normal
     * @param rotation_ matrix with the rotation to be applied
     */
    NLGEOMETRY_API
    virtual void rotate( const Eigen::Matrix3f& rotation_ );

    /**
     * Method to rotate the orbital vertex position and normal
     * @param rotation_ quaternion with the rotation to be applied
     */
    NLGEOMETRY_API
    virtual void rotate( const Eigen::Quaternion< float >& rotation_ );

    /**
     * Method to change the modulus of the position vector
     * @param norm_ new position vector modulus
     */
    NLGEOMETRY_API
    virtual void norm( const float norm_ );

    NLGEOMETRY_API
    virtual void normalize( const Eigen::Vector3f& axis_ );

    NLGEOMETRY_API
    virtual void store( std::vector< float >& buffer_,
                        const TAttribType attribType_ );

    /**
     * Method that return a cloned orbtial vertex
     * @return a cloned orbital vertex from the actual orbital vertex
     */
    NLGEOMETRY_API
    VertexPtr clone( void );

  protected:

    //! Orbital Vertex center
    Eigen::Vector3f _center;

    //! Orbital Vertex tangent
    Eigen::Vector3f _tangent;

  }; // class OrbitalVertex

} // namespace nlgeomtry

#endif
