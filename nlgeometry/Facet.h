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
#ifndef __NLGEOMETRY_FACET__
#define __NLGEOMETRY_FACET__

#include "Vertex.h"

#include <nlgeometry/nlgeometry_export.h>

namespace nlgeometry
{

  class Facet;
  typedef Facet* FacetPtr;
  typedef std::vector< FacetPtr > Facets;

  /*! \class Facet */
  class NLGEOMETRY_EXPORT Facet
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
     * @param id_ id of the correspoding original node.
     */
    Facet( VertexPtr vertex0_ = nullptr, VertexPtr vertex1_ = nullptr,
           VertexPtr vertex2_ = nullptr, VertexPtr vertex3_ = nullptr, uint32_t id_ = 0);

    /**
     * Default destructor
     */
    ~Facet( void )
    {};

    /**
     * Method to get the first triangle/quad vertex pointer
     * @return first triangle/quad vertex pointer
     */
    inline VertexPtr& vertex0( void )
    {
      return _vertex0;
    }

    inline const VertexPtr& vertex0( void ) const
    {
      return _vertex0;
    }

    /**
     * Method to get the second triangle/quad vertex pointer
     * @return second triangle/quad vertex pointer
     */
    inline VertexPtr& vertex1( void )
    {
      return _vertex1;
    }

    inline const VertexPtr& vertex1( void ) const
    {
      return _vertex1;
    }

    /**
     * Method to get the third triangle/quad vertex pointer
     * @return first triangle/quad vertex pointer
     */
    inline VertexPtr& vertex2( void )
    {
      return _vertex2;
    }

    inline const VertexPtr& vertex2( void ) const
    {
      return _vertex2;
    }

    /**
     * Method to get the fourth triangle/quad vertex pointer
     * @return fourth quad vertex pointer
     */
    inline VertexPtr& vertex3( void )
    {
      return _vertex3;
    }

    inline const VertexPtr& vertex3( void ) const
    {
      return _vertex3;
    }

    /** \brief RMethod to get the associated original morphology node.
     * @return id of the associated morphology node.
     */
    inline uint32_t id() const
    { return _id; }

    /**
     * Method that store the facet vertices attribs and assigns the vertices ids
     * @param attribs_ reference to the vertex attribs to return
     */
    void store( Attribs& attribs_, const AttribsFormat format_ );

    /**
     * Method that returns the facet indices stored in a vector
     * @param facetType_ format to the returns indices
     * @return vector with the facet indices
     */
    std::vector< unsigned int > getIndicesAs( TFacetType facetType_ ) const;

    /**
     * Method that add the facet indices to the referenced vector
     * @param facetType_ format to the added indices
     * @param indices_ indices vector where indices are added
     */
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

    //! Id of the corresponding original morphologycal node. 
    uint32_t _id;

  }; // class Facet

} // namespace nlgeometry

#endif
