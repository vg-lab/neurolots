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
#ifndef __NLGEOMETRY_AXIS_ALIGNED_BOUNDING_BOX__
#define __NLGEOMETRY_AXIS_ALIGNED_BOUNDING_BOX__

#include <Eigen/Dense>

#include <nlgeometry/api.h>

namespace nlgeometry
{

  class AxisAlignedBoundingBox;
  typedef AxisAlignedBoundingBox* AxisAlignedBoundingBoxPtr;

  /*! \Class Axisalignedboundingbox */
  class AxisAlignedBoundingBox
  {

  public:

    /**
     * Constructor
     * @param minimum_ minimum axis aligned bounding box position
     * @param maximum_ maximum axis aligned bounding box position
     */
    NLGEOMETRY_API
    AxisAlignedBoundingBox( const Eigen::Vector3f& minimum_,
                            const Eigen::Vector3f& maximum_ );

    /**
     * Default constructor
     */
    NLGEOMETRY_API
    AxisAlignedBoundingBox( void );

    /**
     * Default destructor
     */
    NLGEOMETRY_API
    ~AxisAlignedBoundingBox( void );

    /**
     * Method that returns the minimum axis aligned bounding box position
     * @return the minimum axis aligned bounding box position
     */
    NLGEOMETRY_API
    Eigen::Vector3f& minimum( void );

    NLGEOMETRY_API
    const Eigen::Vector3f& minimum( void ) const;

    /**
     * Method that returns the maximum axis aligned bounding box position
     * @return the maximum axis aligned bounding box position
     */
    NLGEOMETRY_API
    Eigen::Vector3f& maximum( void );

    NLGEOMETRY_API
    const Eigen::Vector3f& maximum( void ) const;

    /**
     * Method that computes and return the axis aligned bounding box center
     * @return the axis aligned bounding box center
     */
    NLGEOMETRY_API
    Eigen::Vector3f center( void ) const;

    /**
     * Method that computes and return the axis aligned bounding box radius
     * @return the axis aligned bounding box radius
     */
    NLGEOMETRY_API
    float radius( void ) const;

    /**
     *
     * Method that expands the current bounds with the given point
     * @param point given point to expand current bounds
     *
     */
    NLGEOMETRY_API
    void expand( const Eigen::Vector3f& point );

    /**
     *
     * Method that expands the current bounds with the given bounding box
     * @param other given bounding box to expand current bounds
     *
     */
    NLGEOMETRY_API
    void expand( const AxisAlignedBoundingBox& other );

    NLGEOMETRY_API
    void clear( void );

  protected:

    //! Left bottom back axis aligned bounding box position
    Eigen::Vector3f _minimum;

    //! Rigth top front axis aligned bounding box position
    Eigen::Vector3f _maximum;

  }; // class Axisalignedboundingbox

} // namespace nlgeometry

#endif
