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
#include "Renderer.h"

#include "Shaders.h"

#include "../nlgeometry/SpatialHashTable.h"

#define MAX_TESS_LEVEL 65.0f

namespace nlrender
{

  Renderer::Renderer( bool keepOpenGLServerStack_)
    : _keepOpenGLServerStack( keepOpenGLServerStack_ )
    , _lod( 10.0f )
    , _tng( 0.2f )
    , _maximumDistance( 100.0f )
    , _tessCriteria( HOMOGENEOUS )
  {
    _viewMatrix = Eigen::Matrix4f::Identity( );
    _projectionMatrix = Eigen::Matrix4f::Identity( );
    _programQuads = new reto::ShaderProgram(  );
    _programQuadsFB = new reto::ShaderProgram(  );
    _programTriangles = new reto::ShaderProgram(  );
    _programTrianglesFB = new reto::ShaderProgram(  );
    _programVDM = new reto::ShaderProgram( );
    _programVDMFB = new reto::ShaderProgram( );
    _programVDMCollec = new reto::ShaderProgram( );
    _programVDMCollecFB = new reto::ShaderProgram( );

    _programPCA = new reto::ShaderProgram( );


    _programQuads->loadVertexShaderFromText( nlrender::quad_vert );
    _programQuads->loadTesselationControlShaderFromText(
      nlrender::quad_tcs );
    _programQuads->loadTesselationEvaluationShaderFromText(
      nlrender::quad_tes );
    _programQuads->loadFragmentShaderFromText( nlrender::quad_frag );
    _programQuads->compileAndLink( );
    _programQuads->autocatching( );


    const char* fbVaryings[ ] = { "outValue0", "outValue1" };
    _programQuadsFB->loadVertexShaderFromText( nlrender::quad_vert );
    _programQuadsFB->loadTesselationControlShaderFromText(
      nlrender::quad_tcs );
    _programQuadsFB->loadTesselationEvaluationShaderFromText(
      nlrender::quad_tes );
    _programQuadsFB->loadGeometryShaderFromText( nlrender::quad_geom );

    _programQuadsFB->create( );
    _programQuadsFB->feedbackVarying( fbVaryings, 2, GL_SEPARATE_ATTRIBS );
    _programQuadsFB->link( );
    _programQuadsFB->autocatching( );


    _programTriangles->loadVertexShaderFromText( nlrender::triangle_vert );
    _programTriangles->loadTesselationControlShaderFromText(
      nlrender::triangle_tcs );
    _programTriangles->loadTesselationEvaluationShaderFromText(
      nlrender::triangle_tes );
    _programTriangles->loadFragmentShaderFromText( nlrender::triangle_frag );
    _programTriangles->compileAndLink( );
    _programTriangles->autocatching( );


    _programTrianglesFB->loadVertexShaderFromText( nlrender::triangle_vert );
    _programTrianglesFB->loadTesselationControlShaderFromText(
      nlrender::triangle_tcs );
    _programTrianglesFB->loadTesselationEvaluationShaderFromText(
      nlrender::triangle_tes );
    _programTrianglesFB->loadGeometryShaderFromText( nlrender::triangle_geom );

    _programTrianglesFB->create( );
    _programTrianglesFB->feedbackVarying( fbVaryings, 2, GL_SEPARATE_ATTRIBS );
    _programTrianglesFB->link( );
    _programTrianglesFB->autocatching( );

    _programVDM->loadVertexShaderFromText( nlrender::vdm_vert );
    _programVDM->loadTesselationControlShaderFromText( nlrender::vdm_tcs );
    _programVDM->loadTesselationEvaluationShaderFromText( nlrender::vdm_tes );
    _programVDM->loadFragmentShaderFromText( nlrender::vdm_frag );
    _programVDM->compileAndLink( );
    _programVDM->autocatching( );
    _programVDM->use( );
    _programVDM->sendUniformi( "vdmTex", 0 );
    _programVDM->sendUniformi( "normalTex", 1 );


    _programPCA->loadVertexShaderFromText( nlrender::pca_vert);
    _programPCA->loadTesselationControlShaderFromText( nlrender::pca_tcs);
    _programPCA->loadTesselationEvaluationShaderFromText( nlrender::pca_tes);
    _programPCA->loadGeometryShaderFromText( nlrender::quad_geom);

    _programPCA->create( );
    _programPCA->feedbackVarying( fbVaryings, 2, GL_SEPARATE_ATTRIBS );
    _programPCA->link( );
    _programPCA->autocatching( );
    _programPCA->use( );
    //_programPCA->sendUniformi( "spineInfo", 1 );
    _programPCA->sendUniformi( "normalTex", 0 );
    _programPCA->sendUniformi( "macroTexture", 1 );
    _programPCA->sendUniformi( "spineInfo", 2 );



    _programVDMFB->loadVertexShaderFromText( nlrender::vdm_vert );
    _programVDMFB->loadTesselationControlShaderFromText( nlrender::vdm_tcs );
    _programVDMFB->loadTesselationEvaluationShaderFromText( nlrender::vdm_tes );
    _programVDMFB->loadGeometryShaderFromText( nlrender::quad_geom );

    _programVDMFB->create( );
    _programVDMFB->feedbackVarying( fbVaryings, 2, GL_SEPARATE_ATTRIBS );
    _programVDMFB->link( );
    _programVDMFB->autocatching( );
    _programVDMFB->use( );
    _programVDMFB->sendUniformi( "vdmTex", 0 );
    _programVDMFB->sendUniformi( "normalTex", 1 );
/*
#Link Error
Link log: error: tessellation evaluation shader input `tcSpineCoord' has no matching output in the previous stage
error: tessellation evaluation shader input `tcModel' has no matching output in the previous stage

    _programVDMCollec->loadVertexShaderFromText( nlrender::vdmCollec_vert );
    _programVDMCollec->loadTesselationControlShaderFromText(
      nlrender::vdmCollec_tcs );
    _programVDMCollec->loadTesselationEvaluationShaderFromText(
      nlrender::vdmCollec_tes );
    _programVDMCollec->loadFragmentShaderFromText( nlrender::vdmCollec_frag );
    _programVDMCollec->compileAndLink( );
    _programVDMCollec->autocatching( );
    _programVDMCollec->use( );
    _programVDMCollec->sendUniformi( "vdmTex", 0 );
    _programVDMCollec->sendUniformi( "normalTex", 1 );

    _programVDMCollecFB->loadVertexShaderFromText( nlrender::vdm_vert );
    _programVDMCollecFB->loadTesselationControlShaderFromText(
      nlrender::vdm_tcs );
    _programVDMCollecFB->loadTesselationEvaluationShaderFromText(
      nlrender::vdmCollec_tes );
    _programVDMCollecFB->loadGeometryShaderFromText( nlrender::quad_geom );

    _programVDMCollecFB->create( );
    _programVDMCollecFB->feedbackVarying( fbVaryings, 2, GL_SEPARATE_ATTRIBS );
    _programVDMCollecFB->link( );
    _programVDMCollecFB->autocatching( );
    _programVDMCollecFB->use( );
    _programVDMCollecFB->sendUniformi( "vdmTex", 0 );
    _programVDMCollecFB->sendUniformi( "normalTex", 1 );
*/
    _tbos.resize( 2 );
    glGenBuffers( 2, _tbos.data( ));

    glGenTransformFeedbacks( 1, &_tfo );
    glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, _tfo );

    glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, _tbos[0] );
    glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 1, _tbos[1] );

    glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
  }

  Renderer::~Renderer( void )
  {
    delete _programTriangles;
    delete _programQuads;
    delete _programTrianglesFB;
    delete _programQuadsFB;

    if ( _tfo != GL_INVALID_VALUE )
      glDeleteVertexArrays( 1, &_tfo );
    if ( _tbos.size( ) > 0 )
      glDeleteBuffers( (GLsizei)_tbos.size( ), _tbos.data( ));
  }

  Eigen::Matrix4f& Renderer::viewMatrix( void )
  {
    return _viewMatrix;
  }

  Eigen::Matrix4f& Renderer::projectionMatrix( void )
  {
    return _projectionMatrix;
  }

  float& Renderer::lod( void )
  {
    return _lod;
  }

  float& Renderer::tangentModulus( void )
  {
    return _tng;
  }

  float& Renderer::maximumDistance( void )
  {
    return _maximumDistance;
  }

  Renderer::TTessCriteria& Renderer::tessCriteria( void )
  {
    return _tessCriteria;
  }

  void Renderer::render( nlgeometry::MeshPtr mesh_,
                         const Eigen::Matrix4f& modelMatrix_,
                         const Eigen::Vector3f& color_,
                         bool renderTriangles_,
                         bool renderQuads_ )
  {
    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    Eigen::Matrix4f viewModel = _viewMatrix * modelMatrix_;
    unsigned int criteria = _tessCriteria;

    if( renderTriangles_ )
    {
      _programTriangles->use( );
      _programTriangles->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programTriangles->sendUniform4m( "viewModel", viewModel.data( ));
      _programTriangles->sendUniform3v( "color", color_.data( ));
      _programTriangles->sendUniformf( "lod", _lod );
      _programTriangles->sendUniformf( "maxDist", _maximumDistance );
      _programTriangles->sendUniformf( "tng", _tng );
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
      mesh_->renderTriangles( );
    }
    if ( renderQuads_ )
    {
      _programQuads->use( );
      _programQuads->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programQuads->sendUniform4m( "viewModel", viewModel.data( ));
      _programQuads->sendUniform3v( "color", color_.data( ));
      _programQuads->sendUniformf( "lod", _lod);
      _programQuads->sendUniformf( "maxDist", _maximumDistance);
      _programQuads->sendUniformf( "tng", _tng);
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
      mesh_->renderQuads( );
    }
    if ( _keepOpenGLServerStack )
      glPopAttrib( );
  }

  void Renderer::render(
      nlgeometry::Meshes meshes_,
      const std::vector< Eigen::Matrix4f >& modelMatrices_,
      const Eigen::Vector3f& color_,
      bool renderTriangles_,
      bool renderQuads_ )
  {
    if ( meshes_.size( ) != modelMatrices_.size( ))
      throw std::runtime_error(
        "Meshes and model matrices have differents size" );

    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    Eigen::Matrix4f viewModel;
    unsigned int criteria = _tessCriteria;

    if( renderTriangles_ )
    {
      _programTriangles->use( );
      _programTriangles->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programTriangles->sendUniform3v( "color", color_.data( ));
      _programTriangles->sendUniformf( "lod", _lod );
      _programTriangles->sendUniformf( "maxDist", _maximumDistance );
      _programTriangles->sendUniformf( "tng", _tng );
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programTriangles->sendUniform4m( "viewModel", viewModel.data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
        meshes_[i]->renderTriangles( );
      }
    }
    if ( renderQuads_ )
    {
      _programQuads->use( );
      _programQuads->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programQuads->sendUniform3v( "color", color_.data( ));
      _programQuads->sendUniformf( "lod", _lod);
      _programQuads->sendUniformf( "maxDist", _maximumDistance);
      _programQuads->sendUniformf( "tng", _tng);
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programQuads->sendUniform4m( "viewModel", viewModel.data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
        meshes_[i]->renderQuads( );
      }
    }

    if ( _keepOpenGLServerStack )
      glPopAttrib( );
  }


  void Renderer::render(
    nlgeometry::Meshes meshes_,
    const std::vector< Eigen::Matrix4f >& modelMatrices_,
    const std::vector< Eigen::Vector3f >& colors_,
    bool renderTriangles_,
    bool renderQuads_ )
  {
    if ( meshes_.size( ) != modelMatrices_.size( ) ||
         modelMatrices_.size( ) != colors_.size( ))
      throw std::runtime_error(
        "Meshes, model matrices and colors have differents size" );
    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    Eigen::Matrix4f viewModel;
    unsigned int criteria = _tessCriteria;

    if( renderTriangles_ )
    {
      _programTriangles->use( );
      _programTriangles->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programTriangles->sendUniformf( "lod", _lod );
      _programTriangles->sendUniformf( "maxDist", _maximumDistance );
      _programTriangles->sendUniformf( "tng", _tng );
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programTriangles->sendUniform4m( "viewModel", viewModel.data( ));
        _programTriangles->sendUniform3v( "color", colors_[i].data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
        meshes_[i]->renderTriangles( );
      }
    }
    if ( renderQuads_ )
    {
      _programQuads->use( );
      _programQuads->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programQuads->sendUniformf( "lod", _lod);
      _programQuads->sendUniformf( "maxDist", _maximumDistance);
      _programQuads->sendUniformf( "tng", _tng);
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programQuads->sendUniform4m( "viewModel", viewModel.data( ));
        _programQuads->sendUniform3v( "color", colors_[i].data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
        meshes_[i]->renderQuads( );
      }
    }

    if ( _keepOpenGLServerStack )
      glPopAttrib( );
  }

  void Renderer::render( nlgeometry::VDMapPtr vdmap_,
                         const Eigen::Matrix4f& modelMatrix_,
                         const Eigen::Vector3f& color_ )
  {
    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    _programVDM->use( );
    _programVDM->sendUniform4m( "proy", _projectionMatrix.data( ));

    Eigen::Matrix4f viewModel = _viewMatrix * modelMatrix_;
    _programVDM->sendUniform4m( "viewModel", viewModel.data( ));
    _programVDM->sendUniform4m( "model", modelMatrix_.data( ));
    _programVDM->sendUniform3v( "color", color_.data( ));
    float maxTexel = vdmap_->size( ) - 1;
    float invTexel = 1.0f / maxTexel;
    unsigned int numSegments = ceil( vdmap_->size( ) / MAX_TESS_LEVEL );
    unsigned int numVertices = numSegments * numSegments * 4;
    unsigned int criteria = _tessCriteria;

    _programVDM->sendUniformf( "lod", _lod / numSegments );
    _programVDM->sendUniformf( "maxTexel", maxTexel );
    _programVDM->sendUniformf( "invTexel", invTexel );
    _programVDM->sendUniformf( "maxDist", _maximumDistance);


    vdmap_->vdmTexture( )->bind( 0 );
    vdmap_->normalTexture( )->bind( 1 );

    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
    glBindVertexArray( _getQuadVao( numSegments ));
    glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawElements( GL_PATCHES, numVertices, GL_UNSIGNED_INT, 0 );

    if ( _keepOpenGLServerStack )
      glPopAttrib( );
  }

  void Renderer::render( const std::vector< nlgeometry::VDMapPtr >& vdmaps_,
                         const std::vector< Eigen::Matrix4f >& modelMatrices_,
                         const Eigen::Vector3f& color_ )
  {
    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    _programVDM->use( );
    _programVDM->sendUniform4m( "proy", _projectionMatrix.data( ));
    _programVDM->sendUniform3v( "color", color_.data( ));

    for ( unsigned int i = 0; i < vdmaps_.size( ); i++ )
    {
      Eigen::Matrix4f viewModel = _viewMatrix * modelMatrices_[i];
      _programVDM->sendUniform4m( "viewModel", viewModel.data( ));
      _programVDM->sendUniform4m( "model", modelMatrices_[i].data( ));
      float maxTexel = vdmaps_[i]->size( ) - 1;
      float invTexel = 1.0f / maxTexel;
      unsigned int numSegments = ceil( vdmaps_[i]->size( ) / MAX_TESS_LEVEL );
      unsigned int numVertices = numSegments * numSegments * 4;
      unsigned int criteria = _tessCriteria;

      _programVDM->sendUniformf( "lod", _lod / numSegments );
      _programVDM->sendUniformf( "maxTexel", maxTexel );
      _programVDM->sendUniformf( "invTexel", invTexel );
      _programVDM->sendUniformf( "maxDist", _maximumDistance);


      vdmaps_[i]->vdmTexture( )->bind( 0 );
      vdmaps_[i]->normalTexture( )->bind( 1 );

      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
      glBindVertexArray( _getQuadVao( numSegments ));
      glPatchParameteri( GL_PATCH_VERTICES, 4 );
      glDrawElements( GL_PATCHES, numVertices, GL_UNSIGNED_INT, 0 );
    }

    if ( _keepOpenGLServerStack )
      glPopAttrib( );
  }

  void Renderer::render( nlgeometry::VDMapCollectionPtr vdmapCollection_,
                         const Eigen::Vector3f& color_ )
  {
    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    _programVDMCollec->use( );
    _programVDMCollec->sendUniform4m( "proy", _projectionMatrix.data( ));
    _programVDMCollec->sendUniform3v( "color", color_.data( ));

    auto modelMatrices = vdmapCollection_->models( );
    float maxTexel = vdmapCollection_->vdmapsSize( ) - 1;
    float invTexel = 1.0f / maxTexel;
    unsigned int criteria = _tessCriteria;

    _programVDMCollec->sendUniformf( "lod", _lod );
    _programVDMCollec->sendUniformf( "maxTexel", maxTexel );
    _programVDMCollec->sendUniformf( "invTexel", invTexel );
    _programVDMCollec->sendUniform4m( "viewModel", _viewMatrix.data( ));
    _programVDMCollec->sendUniformf( "maxDist", _maximumDistance);

    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
    vdmapCollection_->render( );

    if ( _keepOpenGLServerStack )
      glPopAttrib( );
  }

  nlgeometry::MeshPtr Renderer::extract(
    nlgeometry::MeshPtr mesh_, const Eigen::Matrix4f& modelMatrix_,
    bool extractTriangles_, bool extractQuads_ ) const
  {
    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    Eigen::Matrix4f viewModel = ( _viewMatrix * modelMatrix_ ).transpose( );
    Eigen::Matrix4f projection = _projectionMatrix.transpose( );
    unsigned int criteria = _tessCriteria;

    glDisable( GL_CULL_FACE );
    glEnable( GL_RASTERIZER_DISCARD );

    unsigned int query = 0;
    unsigned int numPrimitives = 0;
    unsigned int trianglesSize = 0;
    unsigned int quadsSize = 0;

    std::vector< float > _extractedVertices;
    std::vector< float > _extractedNormals;

    glGenQueries( 1, &query );

    if( extractTriangles_ )
    {
      glBeginQuery( GL_PRIMITIVES_GENERATED, query );
      _programTrianglesFB->use( );
      _programTrianglesFB->sendUniform4m( "proy", projection.data( ));
      _programTrianglesFB->sendUniform4m( "viewModel", viewModel.data( ));
      _programTrianglesFB->sendUniformf( "lod", _lod );
      _programTrianglesFB->sendUniformf( "maxDist", _maximumDistance );
      _programTrianglesFB->sendUniformf( "tng", _tng );
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
      mesh_->renderTriangles( );

      glEndQuery( GL_PRIMITIVES_GENERATED );
      glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );
      trianglesSize = numPrimitives * 9;
      if ( trianglesSize > 0 )
      {
        glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * trianglesSize, nullptr,
                      GL_STATIC_READ );
        glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * trianglesSize, nullptr,
                      GL_STATIC_READ );

        glBeginQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query );
        glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, _tfo );
        glBeginTransformFeedback( GL_TRIANGLES );

        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
        mesh_->renderTriangles( );

        glEndTransformFeedback( );
        glFlush( );

        glEndQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN );
        glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );
        trianglesSize = numPrimitives * 9;

        glBindVertexArray( 0 );
        glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
        _extractedVertices.resize( trianglesSize );
        _extractedNormals.resize( trianglesSize );

        glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
        glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * trianglesSize,
                            _extractedVertices.data( ));
        glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
        glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * trianglesSize,
                            _extractedNormals.data( ));
      }
    }
    if ( extractQuads_ )
    {
      glBeginQuery( GL_PRIMITIVES_GENERATED, query );
      _programQuadsFB->use( );
      _programQuadsFB->sendUniform4m( "proy", projection.data() );
      _programQuadsFB->sendUniform4m( "viewModel", viewModel.data( ));
      _programQuadsFB->sendUniformf( "lod", _lod);
      _programQuadsFB->sendUniformf( "maxDist", _maximumDistance);
      _programQuadsFB->sendUniformf( "tng", _tng);
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
      mesh_->renderQuads( );

      glEndQuery( GL_PRIMITIVES_GENERATED );
      glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );
      quadsSize = numPrimitives * 9;
      if ( quadsSize > 0 )
      {
        glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * quadsSize, nullptr,
                      GL_STATIC_READ );
        glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * quadsSize, nullptr,
                      GL_STATIC_READ );

        glBeginQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query );
        glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, _tfo );
        glBeginTransformFeedback( GL_TRIANGLES );

        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
        mesh_->renderQuads( );

        glEndTransformFeedback( );
        glFlush( );

        glEndQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN );
        glGetQueryObjectuiv( query, GL_QUERY_RESULT, &numPrimitives );
        quadsSize = numPrimitives * 9;

        glBindVertexArray( 0 );
        glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
        _extractedVertices.resize( quadsSize + trianglesSize );
        _extractedNormals.resize( quadsSize + trianglesSize );

        glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
        glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * quadsSize,
                            &_extractedVertices[trianglesSize]);
        glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
        glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * quadsSize,
                            &_extractedNormals[trianglesSize]);
      }
    }
    glDisable( GL_RASTERIZER_DISCARD );
    glDeleteQueries( 1, &query );

    auto mesh = _vectorToMesh( _extractedVertices, _extractedNormals );

    if ( _keepOpenGLServerStack )
      glPopAttrib( );

    return mesh;
  }

  nlgeometry::MeshPtr Renderer::extract( nlgeometry::VDMapPtr vdmap_,
                                         const Eigen::Matrix4f& modelMatrix_ )
  {
    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable( GL_CULL_FACE );
    glEnable( GL_RASTERIZER_DISCARD );

    unsigned int query = 0;
    unsigned int trianglesSize = 0;

    std::vector< float > _extractedVertices;
    std::vector< float > _extractedNormals;

    glGenQueries( 1, &query );

    glBeginQuery( GL_PRIMITIVES_GENERATED, query );
    _programVDMFB->use( );
    _programVDMFB->sendUniform4m( "proy", _projectionMatrix.data( ));
    _programVDMFB->sendUniform4m( "model", modelMatrix_.data( ));
    Eigen::Matrix4f viewModel = _viewMatrix * modelMatrix_;
    _programVDM->sendUniform4m( "viewModel", viewModel.data( ));
    float maxTexel = vdmap_->size( ) - 1;
    float invTexel = 1.0f / maxTexel;
    unsigned int numSegments = ceil( vdmap_->size( ) / MAX_TESS_LEVEL );
    unsigned int numVertices = numSegments * numSegments * 4;
    unsigned int criteria = _tessCriteria;
    _programVDMFB->sendUniformf( "lod", _lod / numSegments );
    _programVDMFB->sendUniformf( "maxTexel", maxTexel );
    _programVDMFB->sendUniformf( "invTexel", invTexel );
    _programVDMFB->sendUniformf( "maxDist", _maximumDistance);

    vdmap_->vdmTexture( )->bind( 0 );
    vdmap_->normalTexture( )->bind( 1 );

    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
    glBindVertexArray( _getQuadVao( numSegments ));
    glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawElements( GL_PATCHES, numVertices, GL_UNSIGNED_INT, 0 );

    glEndQuery( GL_PRIMITIVES_GENERATED );
    glGetQueryObjectuiv( query, GL_QUERY_RESULT, &trianglesSize );
    trianglesSize *= 9;

    if ( trianglesSize > 0 )
    {
      glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * trianglesSize, nullptr,
                    GL_STATIC_READ );
      glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * trianglesSize, nullptr,
                    GL_STATIC_READ );

      glBeginQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query );
      glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, _tfo );
      glBeginTransformFeedback( GL_TRIANGLES );

      glPatchParameteri( GL_PATCH_VERTICES, 4 );
      glDrawElements( GL_PATCHES, numVertices, GL_UNSIGNED_INT, 0 );

      glEndTransformFeedback( );
      glFlush( );

      glEndQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN );
      glGetQueryObjectuiv( query, GL_QUERY_RESULT, &trianglesSize );
      trianglesSize *= 9;

      glBindVertexArray( 0 );
      glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
      _extractedVertices.resize( trianglesSize );
      _extractedNormals.resize( trianglesSize );

      glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
      glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * trianglesSize,
                          _extractedVertices.data( ));
      glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
      glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * trianglesSize,
                          _extractedNormals.data( ));
    }

    glDisable( GL_RASTERIZER_DISCARD );
    glDeleteQueries( 1, &query );

    auto mesh = _vectorToMesh( _extractedVertices, _extractedNormals );

    if ( _keepOpenGLServerStack )
      glPopAttrib( );

    return mesh;
  }


  nlgeometry::MeshPtr  Renderer::PCARender( nlgeometry::PCACompMapPtr macroTexture,
                            reto::Texture1D* spineInfo,
                            nlgeometry::VDMapPtr normalVDM,
                            const int& numComponentes,
                            const Eigen::Matrix4f& modelMatrix_ )
  {
 if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable( GL_CULL_FACE );
    glEnable( GL_RASTERIZER_DISCARD );

    unsigned int query = 0;
    unsigned int trianglesSize = 0;

    std::vector< float > _extractedVertices;
    std::vector< float > _extractedNormals;

    glGenQueries( 1, &query );

    glBeginQuery( GL_PRIMITIVES_GENERATED, query );
    _programPCA->use( );
    _programPCA->sendUniform4m( "proy", _projectionMatrix.data( ));
    _programPCA->sendUniform4m( "model", modelMatrix_.data( ));
    Eigen::Matrix4f viewModel = _viewMatrix * modelMatrix_;
    _programVDM->sendUniform4m( "viewModel", viewModel.data( ));
    float maxTexel = normalVDM->size( ) - 1;
    float invTexel = 1.0f / maxTexel;
    unsigned int numSegments = ceil( normalVDM->size( ) / MAX_TESS_LEVEL );
    unsigned int numVertices = numSegments * numSegments * 4;
    unsigned int criteria = _tessCriteria;
    _programPCA->sendUniformf( "lod", _lod / numSegments );
    _programPCA->sendUniformf( "maxTexel", maxTexel );
    _programPCA->sendUniformf( "invTexel", invTexel );
    _programPCA->sendUniformf( "maxDist", _maximumDistance);

    std::cout<< "numComponents "  << std::to_string(numComponentes) << std::endl;

    _programPCA->sendUniformi("numComponents", numComponentes);

    normalVDM->normalTexture()->bind( 0 );
    macroTexture->textureComponents()->bind(1);
    spineInfo->bind(2);

    glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &criteria );
    glBindVertexArray( _getQuadVao( numSegments ));
    glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawElements( GL_PATCHES, numVertices, GL_UNSIGNED_INT, 0 );

    glEndQuery( GL_PRIMITIVES_GENERATED );
    glGetQueryObjectuiv( query, GL_QUERY_RESULT, &trianglesSize );
    trianglesSize *= 9;

    if ( trianglesSize > 0 )
    {
      glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * trianglesSize, nullptr,
                    GL_STATIC_READ );
      glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * trianglesSize, nullptr,
                    GL_STATIC_READ );

      glBeginQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query );
      glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, _tfo );
      glBeginTransformFeedback( GL_TRIANGLES );

      glPatchParameteri( GL_PATCH_VERTICES, 4 );
      glDrawElements( GL_PATCHES, numVertices, GL_UNSIGNED_INT, 0 );

      glEndTransformFeedback( );
      glFlush( );

      glEndQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN );
      glGetQueryObjectuiv( query, GL_QUERY_RESULT, &trianglesSize );
      trianglesSize *= 9;

      glBindVertexArray( 0 );
      glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
      _extractedVertices.resize( trianglesSize );
      _extractedNormals.resize( trianglesSize );

      glBindBuffer( GL_ARRAY_BUFFER, _tbos[0] );
      glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * trianglesSize,
                          _extractedVertices.data( ));
      glBindBuffer( GL_ARRAY_BUFFER, _tbos[1] );
      glGetBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * trianglesSize,
                          _extractedNormals.data( ));
    }

    glDisable( GL_RASTERIZER_DISCARD );
    glDeleteQueries( 1, &query );

    auto mesh = _vectorToMesh( _extractedVertices, _extractedNormals );

    if ( _keepOpenGLServerStack )
      glPopAttrib( );

    return mesh;

  }

  nlgeometry::MeshPtr Renderer::_vectorToMesh(
    std::vector< float > positions_, std::vector< float > normals_ ) const
  {
    nlgeometry::Vertices vertices;
    nlgeometry::Facets facets;

    Eigen::Vector3f position;
    Eigen::Vector3f normal;

    nlgeometry::SpatialHashTable spht;

    for ( unsigned int i = 0; i < ( unsigned int ) positions_.size( ) / 9;
          i ++ )
    {
      position = Eigen::Vector3f( positions_[i*9], positions_[i*9+1],
                                  positions_[i*9+2] );
      normal = Eigen::Vector3f( normals_[i*9], normals_[i*9+1],
                                  normals_[i*9+2] );
      auto vertex0 = spht.insert( new nlgeometry::Vertex( position, normal ));
      position = Eigen::Vector3f( positions_[i*9+3], positions_[i*9+4],
                                  positions_[i*9+5] );
      normal = Eigen::Vector3f( normals_[i*9+3], normals_[i*9+4],
                                  normals_[i*9+5] );
      auto vertex1 = spht.insert( new nlgeometry::Vertex( position, normal ));
      position = Eigen::Vector3f( positions_[i*9+6], positions_[i*9+7],
                                  positions_[i*9+8] );
      normal = Eigen::Vector3f( normals_[i*9+6], normals_[i*9+7],
                                  normals_[i*9+8] );
      auto vertex2 = spht.insert( new nlgeometry::Vertex( position, normal ));

      if ( vertex0 && vertex1 && vertex2 &&
           vertex0 != vertex1 && vertex0 != vertex2 && vertex1 != vertex2 )
        facets.push_back( new nlgeometry::Facet( vertex0, vertex1, vertex2 ));
    }

    auto mesh = new nlgeometry::Mesh( );
    spht.vertices( vertices );
    mesh->vertices( ) = vertices;
    mesh->triangles( ) = facets;
    return mesh;
  }

  unsigned int  Renderer::_generateQuadVao( unsigned int numSegments_ )
  {
    unsigned int quadVao;
    glGenVertexArrays( 1, &quadVao );
    glBindVertexArray( quadVao );
    std::vector< unsigned int > quadvbos( 2 );
    glGenBuffers( 2, quadvbos.data( ));

    std::vector< float > positions;
    float increment = 2.0f / numSegments_;
    unsigned int numVertices = numSegments_ + 1;
    for ( unsigned int i = 0; i < numVertices; i++ )
    {
      for ( unsigned int j = 0; j < numVertices; j++ )
      {
        positions.push_back( j * increment - 1.0f );
        positions.push_back( i * increment - 1.0f );
        positions.push_back( 0.0f );
      }
    }

    std::vector< unsigned int > indices;
    for ( unsigned int i = 0; i < numSegments_; i++ )
    {
      for ( unsigned int j = 0; j < numSegments_; j++ )
      {
        unsigned int id0 = j + i * numVertices;
        unsigned int id1 = j+1 + i * numVertices;
        unsigned int id2 = j + (i+1) * numVertices;
        unsigned int id3 = j+1 + (i+1) * numVertices;
        indices.push_back( id0 );
        indices.push_back( id2 );
        indices.push_back( id1 );
        indices.push_back( id3 );
      }
    }

    glBindBuffer( GL_ARRAY_BUFFER, quadvbos[0]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * positions.size( ),
                  positions.data( ), GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, quadvbos[1] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size( ),
                  indices.data( ), GL_STATIC_DRAW );

    positions.clear( );
    indices.clear( );

    glBindVertexArray( 0 );
    return quadVao;
  }

  unsigned int Renderer::_getQuadVao( unsigned int numSegments_ )
  {
    auto vaosIt = _quadVaos.find( numSegments_ );
    if ( vaosIt != _quadVaos.end( ))
    {
      return vaosIt->second;
    }
    else
    {
      unsigned int newQuadVao = _generateQuadVao( numSegments_ );
      _quadVaos[numSegments_] = newQuadVao;
      return newQuadVao;
    }
  }
}
