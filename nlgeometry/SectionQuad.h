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
#ifndef __NLGEOMETRY_SECTION_QUAD__
#define __NLGEOMETRY_SECTION_QUAD__

#include "OrbitalVertex.h"
#include "Facet.h"

#include <nlgeometry/nlgeometry_export.h>

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
    NLGEOMETRY_EXPORT
    SectionQuad( OrbitalVertexPtr vertex0_, OrbitalVertexPtr vertex1_,
                 OrbitalVertexPtr vertex2_, OrbitalVertexPtr vertex3_ );

    /**
     * Default destructor
     */
    NLGEOMETRY_EXPORT
    virtual ~SectionQuad( void );

    /**
     * Method that return the first section quad vertex
     * @return the first section quad vertex
     */
    NLGEOMETRY_EXPORT
    OrbitalVertexPtr& vertex0( void );

    NLGEOMETRY_EXPORT
    const OrbitalVertexPtr& vertex0( void ) const;

    /**
     * Method that return the second section quad vertex
     * @return the second section quad vertex
     */
    NLGEOMETRY_EXPORT
    OrbitalVertexPtr& vertex1( void );

    NLGEOMETRY_EXPORT
    const OrbitalVertexPtr& vertex1( void ) const;

    /**
     * Method that return the third section quad vertex
     * @return the third section quad vertex
     */
    NLGEOMETRY_EXPORT
    OrbitalVertexPtr& vertex2( void );

    NLGEOMETRY_EXPORT
    const OrbitalVertexPtr& vertex2( void ) const;

    /**
     * Method that return the fourth section quad vertex
     * @return the fourth section quad vertex
     */
    NLGEOMETRY_EXPORT
    OrbitalVertexPtr& vertex3( void );

    NLGEOMETRY_EXPORT
    const OrbitalVertexPtr& vertex3( void ) const;

    /**
     * Method that inverse the section quad
     */
    NLGEOMETRY_EXPORT
    void inverse( void );

    /**
     * Method return a pointer to a inversed section quad
     * @return a pointer to a inversed section quad
     */
    NLGEOMETRY_EXPORT
    SectionQuadPtr inversed( void ) const;

    /**
     * Method that displace the section quad
     * @param displacement_ vector to displace the section quad
     */
    NLGEOMETRY_EXPORT
    virtual void displace( const Eigen::Vector3f& displacement_ );

    /**
     * Method that place the section quad
     * @param placement_ vector to place the section quad
     */
    NLGEOMETRY_EXPORT
    virtual void place( const Eigen::Vector3f& placement_ );

    /**
     * Method to rotate the section quad
     * @param rotation_ matrix with the rotation to be applied
     */
    NLGEOMETRY_EXPORT
    virtual void rotate( const Eigen::Matrix3f& rotation_ );

    /**
     * Method to rotate the section quad
     * @param rotation_ quad with the rotation to be applied
     */
    NLGEOMETRY_EXPORT
    virtual void rotate( const Eigen::Quaternion< float >& rotation_ );

    /**
     * Method to orient the section quad
     * @param orientation_ to be applied to the section quad
     */
    NLGEOMETRY_EXPORT
    virtual void orient( const Eigen::Vector3f& orientation_ );

    /**
     * Method that return the 0 section quad axis
     * @return the 0 section quad axis
     */
    NLGEOMETRY_EXPORT
    Eigen::Vector3f axis0( void ) const;

    /**
     * Method that return the 1 section quad axis
     * @return the 1 section quad axis
     */
    NLGEOMETRY_EXPORT
    Eigen::Vector3f axis1( void ) const;

    /**
     * Method to change the modulus of the section quad vertices
     * @param norm_ new section quad vertices position vector modulus
     */
    NLGEOMETRY_EXPORT
    virtual void norm( float norm_ );

    /**
     * Method to normalize the section quad vertices
     */
    NLGEOMETRY_EXPORT
    virtual void normalize( void );

    /**
     * Method that return the section quad normal
     * @return the section quad normal
     */
    NLGEOMETRY_EXPORT
    Eigen::Vector3f normal( void ) const;

    /**
     * Method that return a cloned section quad
     * @return a cloned section quad
     */
    NLGEOMETRY_EXPORT
    SectionQuadPtr clone( void ) const;

    /**
     * Method that delete the section quad vertices
     */
    NLGEOMETRY_EXPORT
    void deleteVertices( void );

    /**
     * Method that returns a quaternion with the minimum z rotation between this
     * and the given section quads
     * @param otherQuad_ first section quad
     * @return a quaterion with the minimum z rotation between this and the
     * given section quads
     */
    NLGEOMETRY_EXPORT
    Eigen::Quaternion< float > getZRotation(
      SectionQuadPtr otherQuad_ );

    /**
     * Method that returns the minimum z rotation between this and the given
     * section quads
     * @param otherQuad_ first section quad
     * @return the minimum z rotation between this and the given section quads
     */
    NLGEOMETRY_EXPORT
    float getZAngle(  SectionQuadPtr otherQuad_ );

    /**
     * Static method that return a identity section quad
     * @return a identity section quad
     */
    NLGEOMETRY_EXPORT
    static SectionQuadPtr identity( void );

    /**
     * Static method that added facets to the referenced facets vector forming a
     * pipe
     * @param startQuad_ start pipe section quad
     * @param endQuad_ end pipe section quad
     */
    NLGEOMETRY_EXPORT
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
    NLGEOMETRY_EXPORT
    static Eigen::Quaternion< float > getZRotation(
      SectionQuadPtr quad0_, SectionQuadPtr quad1_ );

    /**
     * Static method that returns the minimum z rotation between two section
     * quads
     * @param quad0_ first section quad
     * @param quad1_ second section quad
     * @return the minimum z rotation between the to given section quads
     */
    NLGEOMETRY_EXPORT
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
