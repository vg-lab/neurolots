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
#ifndef __NLRENDER_RENDERER__
#define __NLRENDER_RENDERER__

#include "../nlgeometry/Mesh.h"

#include <reto/reto.h>

#include <nlrender/api.h>

namespace nlrender
{

  /* \class Renderer */
  class Renderer
  {

  public:

    typedef enum
    {
      HOMOGENEOUS = 0,
      LINEAR
    }TTessCriteria;

    typedef enum
    {
      PERVERTEX = 2,
      GLOBAL
    }TColorFunc;

    typedef enum
    {
      DISABLE = 0,
      ENABLE
    }TTransparencyStatus;

    /**
     * Default constructor
     */
    NLRENDER_API
    Renderer( bool keepOpenGLServerStack_ = false );

    /**
     * Default destructors
     */
    NLRENDER_API
    ~Renderer( void );

    /**
     * Method that return the scene camera view matrix
     * @return the scene camera view matrix
     */
    NLRENDER_API
    Eigen::Matrix4f& viewMatrix( void );

    /**
     * Method that return the scene camera view matrix
     * @return the scene camera view matrix
     */
    NLRENDER_API
    Eigen::Matrix4f& projectionMatrix( void );

    /**
     * Method that return the level of detail
     * @return the level of detail
     */
    NLRENDER_API
    float& lod( void );

    /**
     * Method that return the tangent modulus
     * @return the tanget modulus
     */
    NLRENDER_API
    float& tangentModulus( void );

    /**
     * Method that return the maximum distance
     * @return the maximum distance
     */
    NLRENDER_API
    float& maximumDistance( void );

    /**
     * Method that return the transparency factor
     * @return the transparency factor
     */
    NLRENDER_API
    float& alpha( void );

    /**
     * Method that return the tessellation criteria
     * @return the tessellation criteria
     */
    NLRENDER_API
    TTessCriteria& tessCriteria( void );

    /**
     * Method that return the type of color function
     * @return the color funciton type
     */
    NLRENDER_API
    TColorFunc& colorFunc( void );

    /**
     * Method that return the transparency status
     * @return the transparency status
     */
    NLRENDER_API
    TTransparencyStatus& transparencyStatus( void );

    /**
     * Method that renderize the given mesh
     * @param mesh_ mesh to renderize
     */
    NLRENDER_API
    void render(
      nlgeometry::MeshPtr mesh_,
      const Eigen::Matrix4f& modelMatrix_ = Eigen::Matrix4f::Identity( ),
      const Eigen::Vector3f& color_ = Eigen::Vector3f( 0.5f, 0.5f, 0.5f ),
      bool renderTriangles_ = true, bool renderQuads_ = true,
      bool renderLines = true ) const;
    /**
     * Method that renderize the given meshes
     * @param mesh_ meshes to renderize
     */
    NLRENDER_API
    void render(
      nlgeometry::Meshes meshes_,
      const std::vector< Eigen::Matrix4f >& modelMatrices_,
      const Eigen::Vector3f& color_ = Eigen::Vector3f( 0.5f, 0.5f, 0.5f ),
      bool renderLines_ = true, bool renderTriangles_ = true,
      bool renderQuads_ = true) const;

    /**
     * Method that renderize the given meshes
     * @param mesh_ meshes to renderize
     */
    NLRENDER_API
    void render(
      nlgeometry::Meshes meshes_,
      const std::vector< Eigen::Matrix4f >& modelMatrices_,
      const std::vector< Eigen::Vector3f >& colors_,
      bool renderLines_ = true, bool renderTriangles_ = true,
      bool renderQuads_ = true) const;

    /**
     * Method that extract the given mesh
     * @param mesh_ mesh to extract
     * @return the extracted mesh
     */
    NLRENDER_API
    nlgeometry::MeshPtr extract(
      nlgeometry::MeshPtr mesh_,
      const Eigen::Matrix4f& modelMatrix_ = Eigen::Matrix4f::Identity( ),
      bool extractTriangles_ = true, bool extractQuads_ = true ) const;

    /**
     * Method that extract the given meshes
     * @param mesh_ meshes to extract
     * @return the extracted meshesh
     */
    NLRENDER_API
    nlgeometry::Meshes& extract(
      nlgeometry::Meshes meshes_,
      const std::vector< Eigen::Matrix4f >& modelMatrices_,
      bool extractTriangles_ = true, bool extractQuads_ = true ) const;

    NLRENDER_API
    void initTransparencySystem( unsigned int width_, unsigned int height_ );

    NLRENDER_API
    void setUpOpaqueTransparencyScene( Eigen::Vector3f backgroundColor_,
                                       unsigned int width_,
                                       unsigned int height_ );

    NLRENDER_API
    void setUpTransparentTransparencyScene( void );

    NLRENDER_API
    void composeTransparencyScene( unsigned int finalFbo_ );

  protected:

    nlgeometry::MeshPtr _vectorToMesh( std::vector< float > positions_,
                                       std::vector< float > normals_  ) const;
    void _uploadQuad( void );

    //! Variable to determine if keep the OpenGL server status
    bool _keepOpenGLServerStack;

    //! Program to render lines
    reto::ShaderProgram* _programLines;

    //! Program to render tessellated triangles
    reto::ShaderProgram* _programTriangles;

    //! Program to render tessellated quads
    reto::ShaderProgram* _programQuads;

    //! Program to extract tessellated triangles
    reto::ShaderProgram* _programTrianglesFB;

    //! Program to extract tessellated quads
    reto::ShaderProgram* _programQuadsFB;

    //! Program to compose the transparency scene
    reto::ShaderProgram* _programTransCompose;

    //! Scene camera view matrix
    Eigen::Matrix4f _viewMatrix;

    //! Scene camera projection matrix
    Eigen::Matrix4f _projectionMatrix;

    //! Level of detail
    float _lod;

    //! Tangent modulus
    float _tng;

    //! Maximum tessellation distance
    float _maximumDistance;

    //! Transparency factor
    float _alpha;

    //! Tessellation level of detail criteria
    TTessCriteria _tessCriteria;

    //! Type of render color function
    TColorFunc _colorFunc;

    //! Status of transparency render
    TTransparencyStatus _transparencyStatus;

    //! Vertex array object index to mesh extraction
    unsigned int _tfo;

    //! Vertex buffers object indices to mesh extraction
    std::vector< unsigned int > _tbos;

    //! Vertex array object index to quad
    unsigned int _quadVao;

    //! Variable to indicates if the transparency system is initialized
    bool _transSystemInit;

    //! FBO used in the transparency composition to save opaque objects
    unsigned int _opaqueFbo;

    //! FBO used in the transparency composition to save transparent objects
    unsigned int _transFbo;

    //! Texture to save the opaque objects color
    reto::Texture2D* _opaqueTexture;

    //! Texture to save the tranparent accumulated objects color
    reto::Texture2D* _accumTexture;

    //! Texture to save the traparent revealage objects color
    reto::Texture2D* _revealageTexture;

    //! Texture to save the opaque objects depth
    reto::Texture2D* _depthTexture;

    //! Width of the screen for the transparency system
    unsigned int _transSystemWidth;

    //! height of the screen for the transparency system
    unsigned int _transSystemHeight;

  }; // class Renderer

} // namespace nlrender

#endif
