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
#include "AxisAlignedBoundingBox.h"

namespace nlgeometry
{

  AxisAlignedBoundingBox::AxisAlignedBoundingBox(
    const Eigen::Vector3f& minimum_,
    const Eigen::Vector3f& maximum_ )
    : _minimum( minimum_ )
    , _maximum( maximum_ )
  {

  }

  AxisAlignedBoundingBox::AxisAlignedBoundingBox( void )
  {
    float minLimit = std::numeric_limits<float>::min( );
    float maxLimit = std::numeric_limits<float>::max( );
    _minimum = Eigen::Vector3f( maxLimit, maxLimit, maxLimit );
    _maximum = Eigen::Vector3f( minLimit, minLimit, minLimit );
  }

  AxisAlignedBoundingBox::~AxisAlignedBoundingBox( void )
  {

  }

  Eigen::Vector3f& AxisAlignedBoundingBox::minimum( void )
  {
    return _minimum;
  }

  const Eigen::Vector3f& AxisAlignedBoundingBox::minimum( void ) const
  {
    return _minimum;
  }

  Eigen::Vector3f& AxisAlignedBoundingBox::maximum( void )
  {
    return _maximum;
  }

  const Eigen::Vector3f& AxisAlignedBoundingBox::maximum( void ) const
  {
    return _maximum;
  }

  Eigen::Vector3f AxisAlignedBoundingBox::center( void ) const
  {
    return ( _maximum - _minimum ) * 0.5f + _minimum;
  }

  float AxisAlignedBoundingBox::radius( void ) const
  {
    return ( _maximum - _minimum ).norm( ) * 0.5f;
  }

} // namespace nlgeometry
