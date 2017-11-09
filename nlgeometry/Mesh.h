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
#ifndef __NLGEOMETRY_MESH__
#define __NLGEOMETRY_MESH__

#include "Facet.h"
#include "AxisAlignedBoundingBox.h"

#include <nlgeometry/api.h>

namespace nlgeometry
{

  class Mesh;
  typedef Mesh* MeshPtr;
  typedef std::vector< MeshPtr > Meshes;

  /* \class Mesh */
  class Mesh
  {

  public:

    /**
    * Default constructor
    */
    NLGEOMETRY_API
    Mesh( void );

    /**
     * Default destructor
     */
    NLGEOMETRY_API
    virtual ~Mesh( void );

    /**
     * Method to initialize the geometric data
     */
    NLGEOMETRY_API
    virtual void init( void );

    /**
     * Method that return the mesh vertices
     * @return the mesh vertices
     */
    NLGEOMETRY_API
    Vertices& vertices( void );

    /**
     * Method that return the mesh triangles
     * @return the mesh triangles
     */
    NLGEOMETRY_API
    Facets& triangles( void );

    /**
     * Method that return the returns the mesh quads
     * @return the mesh quads
     */
    NLGEOMETRY_API
    Facets& quads( void );

    /**
     * Method that return the returns the mesh axis aligned bounding box
     * @return the mesh axis aligned bounding box
     */
    NLGEOMETRY_API
    AxisAlignedBoundingBox& aaBoundingBox( void );

    /**
     * Method that return the returns the mesh model matrix
     * @return the mesh model matrix
     */
    NLGEOMETRY_API
    Eigen::Matrix4f& modelMatrix( void );

    NLGEOMETRY_API
    float*  modelMatrixVectorized( void );

    /**
     * Method that free the cpu geometric information of the mesh
     */
    NLGEOMETRY_API
    void clearCPUData( void );

    /**
     * Method that free the gpu geometric information of the mesh
     */
    NLGEOMETRY_API
    void clearGPUData( void );

    /**
     * Method that upload the geometric information of the mesh to the gpu
     * @param format_ format of the gpu buffers
     */
    NLGEOMETRY_API
    virtual void uploadGPU( AttribsFormat format_,
                            Facet::TFacetType facetType_ = Facet::TRIANGLES );

    /**
     * Method that computes the axis aligned bounding box of the mesh geometry
     */
    NLGEOMETRY_API
    void computeBoundingBox( void );

    /**
     * Method that computes the normals of the mesh geometry
     */
    NLGEOMETRY_API
    void computeNormals( void );

    /**
     * Method that render the mesh triangles
     */
    NLGEOMETRY_API
    virtual void renderTriangles(
      Facet::TFacetType facetType_ = Facet::TRIANGLES );

    /**
     * Method that render the mesh quads
     */
    NLGEOMETRY_API
    virtual void renderQuads(
      Facet::TFacetType facetType_ = Facet::TRIANGLES );

    /**
     * Method that render the all mesh
     */
    NLGEOMETRY_API
    void render( void );

  private:

    void _uploadBuffer( std::vector< float >& buffer_, TAttribType type_,
                        unsigned int vaoPosition_ );

  protected:

    //! Mesh vertices
    Vertices _vertices;

    //! Mesh triangles
    Facets _triangles;

    //! Mesh quads
    Facets _quads;

    //! Mesh axis aligned bounding box
    AxisAlignedBoundingBox _aaBoundingBox;

    //! Index to the Vertex Array Object of the mesh in the gpu
    unsigned int _vao;

    //! Vector of indices of the Vertex Buffer Objects of the mesh in the gpu
    std::vector< unsigned int > _vbos;

    //! Size of the mesh triangles
    unsigned int _trianglesSize;

    //! Size of the mesh quads
    unsigned int _quadsSize;

    //! Model matrix of the mesh
    Eigen::Matrix4f _modelMatrix;

  }; // class Mesh

} // namespace nlgeometry

#endif
