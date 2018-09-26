/*
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
#ifndef __NLGEOMETRY_VERTEX__
#define __NLGEOMETRY_VERTEX__

#include <Eigen/Dense>
#include <vector>

#include <nlgeometry/api.h>

namespace nlgeometry
{

  class Vertex;
  typedef Vertex* VertexPtr;
  typedef std::vector< VertexPtr > Vertices;

  typedef enum
    {
      NONE = 0,
      POSITION,
      NORMAL,
      COLOR,
      CENTER,
      TANGENT,
      UV
    } TAttribType;

    typedef std::vector< std::vector< float >> Attribs;

    typedef std::vector< TAttribType > AttribsFormat;

  /*! \class Vertex */
  class Vertex
  {

  public:

    /**
     * Constructor
     * @param position_ vertex position
     * @param normal_ vertex normal
     * @param color_ vertex color
     * @param uv_ vertex uv coordinates
     */
    NLGEOMETRY_API
    Vertex(
      const Eigen::Vector3f& position_,
      const Eigen::Vector3f& normal_ = Eigen::Vector3f( 0.0f, 0.0f, 0.0f ),
      const Eigen::Vector3f& color_ = Eigen::Vector3f( 0.0f, 0.0f, 0.0f ),
      const Eigen::Vector2f& uv_ = Eigen::Vector2f( 0.0f, 0.0f ));

    /**
     * Copy constructor
     * @param other_ vertex to be copied
     */
    NLGEOMETRY_API
    Vertex( const Vertex& other_ );

    /**
     * Default destructor
     */
    NLGEOMETRY_API
    virtual ~Vertex( void );

    /**
     * Method to get the vertex id
     * @return vertex id
     */
    NLGEOMETRY_API
    const unsigned int& id( void ) const;

    /**
     * Method to get the vertex position
     * @return vertex position
     */
    NLGEOMETRY_API
    Eigen::Vector3f& position( void );

    NLGEOMETRY_API
    const Eigen::Vector3f& position( void ) const;

    /**
     * Method to get the vertex normal
     * @return vertex normal
     */
    NLGEOMETRY_API
    Eigen::Vector3f& normal( void );

    NLGEOMETRY_API
    const Eigen::Vector3f& normal( void ) const;

    /**
     * Method to get the vertex color
     * @return vertex color
     */
    NLGEOMETRY_API
    Eigen::Vector3f& color( void );

    NLGEOMETRY_API
    const Eigen::Vector3f& color( void ) const;

    /**
     * Method to get the vertex uv coordinates
     * @return vertex uv coordinates
     */
    NLGEOMETRY_API
    Eigen::Vector2f& uv( void );

    NLGEOMETRY_API
    const Eigen::Vector2f& uv( void ) const;

    /**
     * Method to displace the vertex position
     * @param displacement_ displacement applied to the vertex position
     */
    NLGEOMETRY_API
    virtual void displace( const Eigen::Vector3f& displacement_ );

    /**
     * Method to place the vertex position
     * @param placement_ placement to place the vertex position
     */
    NLGEOMETRY_API
    virtual void place( const Eigen::Vector3f& placement_ );

    /**
     * Method to rotate the vertex position and normal
     * @param rotation_ matrix with the rotation to be applied
     */
    NLGEOMETRY_API
    virtual void rotate( const Eigen::Matrix3f& rotation_ );

    /**
     * Method to rotate the vertex position and normal
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
    virtual void normalize( const Eigen::Vector3f& /*axis_*/ ){ }

    NLGEOMETRY_API
    virtual void store( std::vector< float >& buffer_,
                        const TAttribType attribType_ );

    /**
     * Method that store the vertex attribs (position, normal, color, uv
     * coordinates) and assigns the vertex id
     * @param attribs_ reference to the vertex attribs to return
     * @param fomat_ vector with the attribs format
     */
    NLGEOMETRY_API
    void store( Attribs& attribs_, const AttribsFormat format_ );

    /**
     * Method that return a cloned vertex
     * @return a cloned vertex from the actual vertex
     */
    NLGEOMETRY_API
    virtual VertexPtr clone( void );

  protected:

    //! Vertex id
    unsigned int _id;

    //! Vertex position
    Eigen::Vector3f _position;

    //! Vertex normal
    Eigen::Vector3f _normal;

    //! Vertex color
    Eigen::Vector3f _color;

    //! Vertex uv coordinates
    Eigen::Vector2f _uv;

  }; // class Vertex

} // namespace nlgeometry

#endif
