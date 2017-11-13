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
#ifndef __NLGEOMETRY_SECTION_QUAD__
#define __NLGEOMETRY_SECTION_QUAD__

#include "OrbitalVertex.h"
#include "Facet.h"

#include <nlgeometry/api.h>

namespace nlgeometry
{

  class SectionQuad;
  typedef SectionQuad* SectionQuadPtr;

  /*! \class SectionQuad */
  class SectionQuad
  {

  public:

    /**
     * Constructor
     * @param vertex0_ first vertex of the section quad
     * @param vertex1_ second vertex of the section quad
     * @param vertex2_ third vertex of the section quad
     * @param vertex3_ fourth vertex of the section quad
     */
    NLGEOMETRY_API
    SectionQuad( OrbitalVertexPtr vertex0_, OrbitalVertexPtr vertex1_,
                 OrbitalVertexPtr vertex2_, OrbitalVertexPtr vertex3_ );

    /**
     * Default destructor
     */
    virtual ~SectionQuad( void );

    /**
     * Method that return the first section quad vertex
     * @return the first section quad vertex
     */
    NLGEOMETRY_API
    OrbitalVertexPtr& vertex0( void );

    /**
     * Method that return the second section quad vertex
     * @return the second section quad vertex
     */
    NLGEOMETRY_API
    OrbitalVertexPtr& vertex1( void );

    /**
     * Method that return the third section quad vertex
     * @return the third section quad vertex
     */
    NLGEOMETRY_API
    OrbitalVertexPtr& vertex2( void );

    /**
     * Method that return the fourth section quad vertex
     * @return the fourth section quad vertex
     */
    NLGEOMETRY_API
    OrbitalVertexPtr& vertex3( void );

    /**
     * Method that inverse the section quad
     */
    NLGEOMETRY_API
    void inverse( void );

    /**
     * Method return a pointer to a inversed section quad
     * @return a pointer to a inversed section quad
     */
    NLGEOMETRY_API
    SectionQuadPtr inversed( void ) const;

    /**
     * Method that displace the section quad
     * @param displacement_ vector to displace the section quad
     */
    NLGEOMETRY_API
    virtual void displace( const Eigen::Vector3f& displacement_ );

    /**
     * Method that place the section quad
     * @param placement_ vector to place the section quad
     */
    NLGEOMETRY_API
    virtual void place( const Eigen::Vector3f& placement_ );

    /**
     * Method to rotate the section quad
     * @param rotation_ matrix with the rotation to be applied
     */
    NLGEOMETRY_API
    virtual void rotate( const Eigen::Matrix3f& rotation_ );

    /**
     * Method to rotate the section quad
     * @param rotation_ quad with the rotation to be applied
     */
    NLGEOMETRY_API
    virtual void rotate( const Eigen::Quaternion< float >& rotation_ );

    /**
     * Method to orient the section quad
     * @param orientation_ to be applied to the section quad
     */
    NLGEOMETRY_API
    virtual void orient( const Eigen::Vector3f& orientation_ );

    /**
     * Method that return the 0 section quad axis
     * @return the 0 section quad axis
     */
    NLGEOMETRY_API
    Eigen::Vector3f axis0( void ) const;

    /**
     * Method that return the 1 section quad axis
     * @return the 1 section quad axis
     */
    NLGEOMETRY_API
    Eigen::Vector3f axis1( void ) const;

    /**
     * Method to change the modulus of the section quad vertices
     * @param norm_ new section quad vertices position vector modulus
     */
    NLGEOMETRY_API
    virtual void norm( float norm_ );

    /**
     * Method to normalize the section quad vertices
     */
    NLGEOMETRY_API
    virtual void normalize( void );

    /**
     * Method that return the section quad normal
     * @return the section quad normal
     */
    NLGEOMETRY_API
    Eigen::Vector3f normal( void ) const;

    /**
     * Method that return a cloned section quad
     * @return a cloned section quad
     */
    NLGEOMETRY_API
    SectionQuadPtr clone( void ) const;

    /**
     * Method that delete the section quad vertices
     */
    NLGEOMETRY_API
    void deleteVertices( void );

    /**
     * Method that returns a quaternion with the minimum z rotation between this
     * and the given section quads
     * @param otherQuad_ first section quad
     * @return a quaterion with the minimum z rotation between this and the
     * given section quads
     */
    NLGEOMETRY_API
    Eigen::Quaternion< float > getZRotation(
      SectionQuadPtr otherQuad_ );

    /**
     * Method that returns the minimum z rotation between this and the given
     * section quads
     * @param otherQuad_ first section quad
     * @return the minimum z rotation between this and the given section quads
     */
    NLGEOMETRY_API
    float getZAngle(  SectionQuadPtr otherQuad_ );

    /**
     * Static method that return a identity section quad
     * @return a identity section quad
     */
    NLGEOMETRY_API
    static SectionQuadPtr identity( void );

    /**
     * Static method that added facets to the referenced facets vector forming a
     * pipe
     * @param startQuad_ start pipe section quad
     * @param endQuad_ end pipe section quad
     */
    NLGEOMETRY_API
    static void createPipe( SectionQuadPtr startQuad_, SectionQuadPtr endQuad_,
                            Facets& facets_, bool checkDirection_ = false );

    /**
     * Static method that returns a quaternion with the minimum z rotation
     * between two section quads
     * @param quad0_ first section quad
     * @param quad1_ second section quad
     * @return a quaterion with the minimum z rotation between the to given
     * section quads
     */
    NLGEOMETRY_API
    static Eigen::Quaternion< float > getZRotation(
      SectionQuadPtr quad0_, SectionQuadPtr quad1_ );

    /**
     * Static method that returns the minimum z rotation between two section
     * quads
     * @param quad0_ first section quad
     * @param quad1_ second section quad
     * @return the minimum z rotation between the to given section quads
     */
    NLGEOMETRY_API
    static float getZAngle(
      SectionQuadPtr quad0_, SectionQuadPtr quad1_ );

  protected:

    //! First section quad vertex
    OrbitalVertexPtr _vertex0;

    //! Second section quad vertex
    OrbitalVertexPtr _vertex1;

    //! Third section quad vertex
    OrbitalVertexPtr _vertex2;

    //! Fourth section quad vertex
    OrbitalVertexPtr _vertex3;

  };

} // namespace nlgeometry

#endif
