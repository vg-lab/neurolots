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
#ifndef __NLGEOMETRY_FACET__
#define __NLGEOMETRY_FACET__

#include "Vertex.h"

#include <nlgeometry/api.h>

namespace nlgeometry
{

  class Facet;
  typedef Facet* FacetPtr;
  typedef std::vector< FacetPtr > Facets;

  /*! \class Facet */
  class Facet
  {

  public:

    typedef enum
    {
      TRIANGLES = 0,
      PATCHES
    } TFacetType;

    /**
     * Constructor
     * @param vertex0_ pointer to the first triangle/quad vertex
     * @param vertex1_ pointer to the second triangle/quad vertex
     * @param vertex2_ pointer to the third triangle/quad vertex
     * @param vertex3_ pointer to the fourth quad vertex
     */
    NLGEOMETRY_API
    Facet( VertexPtr vertex0_ = nullptr, VertexPtr vertex1_ = nullptr,
           VertexPtr vertex2_ = nullptr, VertexPtr vertex3_ = nullptr );

    /**
     * Default destructor
     */
    NLGEOMETRY_API
    ~Facet( void );

    /**
     * Method to get the first triangle/quad vertex pointer
     * @return first triangle/quad vertex pointer
     */
    NLGEOMETRY_API
    VertexPtr& vertex0( void );

    NLGEOMETRY_API
    const VertexPtr& vertex0( void ) const;

    /**
     * Method to get the second triangle/quad vertex pointer
     * @return second triangle/quad vertex pointer
     */
    NLGEOMETRY_API
    VertexPtr& vertex1( void );

    NLGEOMETRY_API
    const VertexPtr& vertex1( void ) const;

    /**
     * Method to get the third triangle/quad vertex pointer
     * @return first triangle/quad vertex pointer
     */
    NLGEOMETRY_API
    VertexPtr& vertex2( void );

    NLGEOMETRY_API
    const VertexPtr& vertex2( void ) const;

    /**
     * Method to get the fourth triangle/quad vertex pointer
     * @return fourth quad vertex pointer
     */
    NLGEOMETRY_API
    VertexPtr& vertex3( void );

    NLGEOMETRY_API
    const VertexPtr& vertex3( void ) const;

    /**
     * Method that store the facet vertices attribs and assigns the vertices ids
     * @param attribs_ reference to the vertex attribs to return
     */
    NLGEOMETRY_API
    void store( Attribs& attribs_, const AttribsFormat format_ );

    /**
     * Method that put the store status to nor stored for all the facet vertices
     */
    NLGEOMETRY_API
    void cleanStoreStatus( void );

    /**
     * Method that returns the facet indices stored in a vector
     * @param facetType_ format to the returns indices
     * @return vector with the facet indices
     */
    NLGEOMETRY_API
    std::vector< unsigned int > getIndicesAs( TFacetType facetType_ ) const;

    /**
     * Method that add the facet indices to the referenced vector
     * @param facetType_ format to the added indices
     * @param indices_ indices vector where indices are added
     */
    NLGEOMETRY_API
    void addIndicesAs( TFacetType facetType_,
                       std::vector< unsigned int >& indices_ ) const;
  private:

    //! First vertex triangle/quad facet
    VertexPtr _vertex0;

    //! Second vertex triangle/quad facet
    VertexPtr _vertex1;

    //! Third vertex triangle/quad facet
    VertexPtr _vertex2;

    //! Fourth vertex quad facet
    VertexPtr _vertex3;

  }; // class Facet

} // namespace nlgeometry

#endif
