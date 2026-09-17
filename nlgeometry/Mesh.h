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
#ifndef __NLGEOMETRY_MESH__
#define __NLGEOMETRY_MESH__

#include "Facet.h"
#include "AxisAlignedBoundingBox.h"

#include <nlgeometry/nlgeometry_export.h>

#include <unordered_map>
#include <set>

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
    NLGEOMETRY_EXPORT
    Mesh( void );

    /**
     * Default destructor
     */
    NLGEOMETRY_EXPORT
    virtual ~Mesh( void );

    /**
     * Method to initialize the geometric data
     */
    NLGEOMETRY_EXPORT
    virtual void init( void );

    /**
     * Method that return the mesh vertices
     * @return the mesh vertices
     */
    NLGEOMETRY_EXPORT
    Vertices& vertices( void );

    /**
     * Method that return the mesh lines
     * @return the mesh lines
     */
    NLGEOMETRY_EXPORT
    Facets& lines( void );

    /**
     * Method that return the mesh triangles
     * @return the mesh triangles
     */
    NLGEOMETRY_EXPORT
    Facets& triangles( void );

    /**
     * Method that return the returns the mesh quads
     * @return the mesh quads
     */
    NLGEOMETRY_EXPORT
    Facets& quads( void );

    /**
     * Method that return the uploaded vertices size
     * @return the uploades vertices size
     */
    NLGEOMETRY_EXPORT
    unsigned int verticesSize( void );

    /**
     * Method that returns the returns the mesh axis aligned bounding box
     * @return the mesh axis aligned bounding box
     */
    NLGEOMETRY_EXPORT
    AxisAlignedBoundingBox& aaBoundingBox( void );

    /**
     * Method that return the returns the mesh model matrix
     * @return the mesh model matrix
     */
    NLGEOMETRY_EXPORT
    Eigen::Matrix4f& modelMatrix( void );

    NLGEOMETRY_EXPORT
    float*  modelMatrixVectorized( void );

    /**
     * Method that free the cpu geometric information of the mesh
     */
    NLGEOMETRY_EXPORT
    void clearCPUData( void );

    /**
     * Method that free the gpu geometric information of the mesh
     */
    NLGEOMETRY_EXPORT
    void clearGPUData( void );

    /**
     * Clears the morphology_nodes->vertices map.
     */
    NLGEOMETRY_EXPORT    
    inline void clearVerticesMap()
    {
      _nodesToIndicesMap.clear();
    }

    /**
     * Method that upload the geometric information of the mesh to the gpu
     * @param format_ format of the gpu buffers
     */
    NLGEOMETRY_EXPORT
    virtual void uploadGPU( AttribsFormat format_,
                            Facet::TFacetType facetType_ = Facet::TRIANGLES );

    /**
     * Method that upload the geometric information of the mesh to the gpu
     * @param format_ format of the gpu buffer
     * @param buffer_ geometric data to upload
     */
    NLGEOMETRY_EXPORT
    void uploadDataBuffer( TAttribType format_, std::vector< float >& buffer_ );

    /**
     * Method that upload a indexes buffer to the gpu
     * @param type_ Type of indexes
     * @param buffer_ geometric data to upload
     */
    NLGEOMETRY_EXPORT
    void uploadIndexesBuffer(std::vector<unsigned int>& buffer_ );

    /**
     * Method that computes the axis aligned bounding box of the mesh geometry
     */
    NLGEOMETRY_EXPORT
    void computeBoundingBox( void );

    /**
     * Method that computes the normals of the mesh geometry
     */
    NLGEOMETRY_EXPORT
    void computeNormals( void );

    /**
     * Method that render the mesh lines
     */
    NLGEOMETRY_EXPORT
    virtual void renderLines( void );

    /**
     * Method that render the mesh triangles
     */
    NLGEOMETRY_EXPORT
    virtual void renderTriangles( void );

    /**
     * Method that render the mesh quads
     */
    NLGEOMETRY_EXPORT
    virtual void renderQuads( void );

    /** \brief Method to render the custom vertices buffer. 
     */
    NLGEOMETRY_EXPORT
    virtual void renderPart( void );

    /**
     * Method that render the all mesh
     */
    NLGEOMETRY_EXPORT
    void render( void );

    NLGEOMETRY_EXPORT
    void printVerticesMap() const;

    NLGEOMETRY_EXPORT
    std::vector<uint32_t> morphologyNodeToVertices(const uint32_t &nodeId) const;

  private:

    void _conformVertices( void );

    void _createBuffer( TAttribType type_, unsigned int vaoPosition_ );

    void _uploadAttribBuffer( std::vector< float >& buffer_,
                        unsigned int vaoPosition_ );

                            

    bool _equalFormat( AttribsFormat format0_, AttribsFormat format1_ );

  protected:
    //! Mesh vertices
    Vertices _vertices;

    //! Mesh lines
    Facets _lines;

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

    //! Size of the mesh lines
    unsigned int _linesSize;

    //! Size of the mesh triangles
    unsigned int _trianglesSize;

    //! Size of the mesh quads
    unsigned int _quadsSize;

    //! Size of uploaded vertices
    unsigned int _verticesSize;

    //! Size of vertices in custom buffer
    unsigned int _verticesBufferSize;

    //! Model matrix of the mesh
    Eigen::Matrix4f _modelMatrix;

    //! Attribs format in gpu
    AttribsFormat _format;

    //! Facet type uploaded to the gpu
    Facet::TFacetType _facetType;

    //! Morphological node->vertex map.
    std::unordered_map<uint32_t, std::vector<uint32_t>> _nodesToIndicesMap;

  }; // class Mesh

} // namespace nlgeometry

#endif
