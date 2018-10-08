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

namespace nlrender
{

  Renderer::Renderer( bool keepOpenGLServerStack_ )
    : _keepOpenGLServerStack( keepOpenGLServerStack_ )
    , _lod( 10.0f )
    , _tng( 0.2f )
    , _maximumDistance( 100.0f )
    , _alpha( 0.5f )
    , _tessCriteria( HOMOGENEOUS )
    , _colorFunc( GLOBAL )
    , _transparencyStatus( DISABLE )
    , _transSystemInit( false )
    , _transSystemWidth( 0 )
    , _transSystemHeight( 0 )
  {
    _viewMatrix = Eigen::Matrix4f::Identity( );
    _projectionMatrix = Eigen::Matrix4f::Identity( );
    _programLines = new reto::ShaderProgram( );
    _programQuads = new reto::ShaderProgram( );
    _programQuadsFB = new reto::ShaderProgram( );
    _programTriangles = new reto::ShaderProgram( );
    _programTrianglesFB = new reto::ShaderProgram( );
    _programTransCompose = new reto::ShaderProgram( );

    _programLines->loadVertexShaderFromText( nlrender::line_vert );
    _programLines->loadFragmentShaderFromText( nlrender::line_frag );
    _programLines->compileAndLink( );
    _programLines->autocatching( );

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

    _programTransCompose->loadVertexShaderFromText(
      nlrender::transparency_vert );
    _programTransCompose->loadFragmentShaderFromText(
      nlrender::transparency_frag );
    _programTransCompose->compileAndLink( );
    _programTransCompose->autocatching( );
    _programTransCompose->use( );
    glUniform1i( glGetUniformLocation( _programTransCompose->program( ),
                                       "opaqueTexture" ), 0);
    glUniform1i( glGetUniformLocation( _programTransCompose->program( ),
                                     "accumTexture" ), 1);
    glUniform1i( glGetUniformLocation( _programTransCompose->program( ),
                                     "revealageTexture" ), 2);

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
    delete _programLines;
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

  float& Renderer::alpha( void )
  {
    return _alpha;
  }

  Renderer::TTessCriteria& Renderer::tessCriteria( void )
  {
    return _tessCriteria;
  }

  Renderer::TColorFunc& Renderer::colorFunc( void )
  {
    return _colorFunc;
  }

  Renderer::TTransparencyStatus& Renderer::transparencyStatus( void )
  {
    return _transparencyStatus;
  }

  void Renderer::render( nlgeometry::MeshPtr mesh_,
                         const Eigen::Matrix4f& modelMatrix_,
                         const Eigen::Vector3f& color_,
                         bool renderLines_,
                         bool renderTriangles_,
                         bool renderQuads_ ) const
  {
    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    Eigen::Matrix4f viewModel = _viewMatrix * modelMatrix_;
    std::vector< unsigned int > vertexSubroutines( 1 );
    vertexSubroutines[0] = _tessCriteria;
    std::vector< unsigned int > fragmentSubroutines( 2 );
    fragmentSubroutines[0] = _colorFunc;
    fragmentSubroutines[1] = _transparencyStatus;

    if ( renderLines_ )
    {
      _programLines->use( );
      _programLines->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programLines->sendUniform4m( "viewModel", viewModel.data( ));
      _programLines->sendUniform3v( "color", color_.data( ));
      _programLines->sendUniformf( "alpha", _alpha );
      glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                               fragmentSubroutines.data( ));
      mesh_->renderLines( );
    }
    if( renderTriangles_ )
    {
      _programTriangles->use( );
      _programTriangles->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programTriangles->sendUniform4m( "viewModel", viewModel.data( ));
      _programTriangles->sendUniform3v( "color", color_.data( ));
      _programTriangles->sendUniformf( "lod", _lod );
      _programTriangles->sendUniformf( "maxDist", _maximumDistance );
      _programTriangles->sendUniformf( "tng", _tng );
      _programTriangles->sendUniformf( "alpha", _alpha );
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, vertexSubroutines.data( ) );
      glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                               fragmentSubroutines.data( ));
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
      _programQuads->sendUniformf( "alpha", _alpha );
      glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, vertexSubroutines.data( ) );
      glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                               fragmentSubroutines.data( ));
      mesh_->renderQuads( );
    }
    if ( _keepOpenGLServerStack )
      glPopAttrib( );
  }

  void Renderer::render(
      nlgeometry::Meshes meshes_,
      const std::vector< Eigen::Matrix4f >& modelMatrices_,
      const Eigen::Vector3f& color_,
      bool renderLines_,
      bool renderTriangles_,
      bool renderQuads_ ) const
  {
    if ( meshes_.size( ) != modelMatrices_.size( ))
      throw std::runtime_error(
        "Meshes and model matrices have differents size" );

    if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    Eigen::Matrix4f viewModel;
    std::vector< unsigned int > vertexSubroutines( 1 );
    vertexSubroutines[0] = _tessCriteria;
    std::vector< unsigned int > fragmentSubroutines( 2 );
    fragmentSubroutines[0] = _colorFunc;
    fragmentSubroutines[1] = _transparencyStatus;

    if ( renderLines_ )
    {
      _programLines->use( );
      _programLines->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programLines->sendUniform3v( "color", color_.data( ));
      _programLines->sendUniformf( "alpha", _alpha );
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programLines->sendUniform4m( "viewModel", viewModel.data( ));
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                               fragmentSubroutines.data( ));
        meshes_[i]->renderLines( );
      }
    }
    if( renderTriangles_ )
    {
      _programTriangles->use( );
      _programTriangles->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programTriangles->sendUniform3v( "color", color_.data( ));
      _programTriangles->sendUniformf( "lod", _lod );
      _programTriangles->sendUniformf( "maxDist", _maximumDistance );
      _programTriangles->sendUniformf( "tng", _tng );
      _programTriangles->sendUniformf( "alpha", _alpha );
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programTriangles->sendUniform4m( "viewModel", viewModel.data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1,
                                 vertexSubroutines.data( ) );
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                                 fragmentSubroutines.data( ));
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
      _programQuads->sendUniformf( "alpha", _alpha );

      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {

        viewModel = _viewMatrix * modelMatrices_[i];
        _programQuads->sendUniform4m( "viewModel", viewModel.data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1,
                                 vertexSubroutines.data( ) );
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                                 fragmentSubroutines.data( ));
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
    bool renderLines_,
    bool renderTriangles_,
    bool renderQuads_ ) const
  {
    if ( meshes_.size( ) != modelMatrices_.size( ) ||
         modelMatrices_.size( ) != colors_.size( ))
      throw std::runtime_error(
        "Meshes, model matrices and colors have differents size" );
 if ( _keepOpenGLServerStack )
      glPushAttrib( GL_ALL_ATTRIB_BITS );

    Eigen::Matrix4f viewModel;
    std::vector< unsigned int > vertexSubroutines( 1 );
    vertexSubroutines[0] = _tessCriteria;
    std::vector< unsigned int > fragmentSubroutines( 2 );
    fragmentSubroutines[0] = _colorFunc;
    fragmentSubroutines[1] = _transparencyStatus;

    if ( renderLines_ )
    {
      _programLines->use( );
      _programLines->sendUniform4m( "proy", _projectionMatrix.data( ));
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programLines->sendUniform4m( "viewModel", viewModel.data( ));
        _programLines->sendUniform3v( "color", colors_[i].data( ));
        _programLines->sendUniformf( "alpha", _alpha );
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                               fragmentSubroutines.data( ));
        meshes_[i]->renderLines( );
      }
    }
    if( renderTriangles_ )
    {
      _programTriangles->use( );
      _programTriangles->sendUniform4m( "proy", _projectionMatrix.data( ));
      _programTriangles->sendUniformf( "lod", _lod );
      _programTriangles->sendUniformf( "maxDist", _maximumDistance );
      _programTriangles->sendUniformf( "tng", _tng );
      _programTriangles->sendUniformf( "alpha", _alpha );
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programTriangles->sendUniform4m( "viewModel", viewModel.data( ));
        _programTriangles->sendUniform3v( "color", colors_[i].data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1,
                                 vertexSubroutines.data( ) );
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                                 fragmentSubroutines.data( ));
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
      _programQuads->sendUniformf( "alpha", _alpha );
      for ( unsigned int i = 0; i < ( unsigned int )meshes_.size( ); i++ )
      {
        viewModel = _viewMatrix * modelMatrices_[i];
        _programQuads->sendUniform4m( "viewModel", viewModel.data( ));
        _programQuads->sendUniform3v( "color", colors_[i].data( ));
        glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1,
                                 vertexSubroutines.data( ) );
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 2,
                                 fragmentSubroutines.data( ));
        meshes_[i]->renderQuads( );
      }
    }

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

  void Renderer::initTransparencySystem( unsigned int width_,
                                         unsigned int height_ )
  {
    if ( !_transSystemInit )
    {
      _transSystemWidth = width_;
      _transSystemHeight = height_;

      auto texConfig = reto::TextureConfig( );
      texConfig.internalFormat = GL_RGBA32F;
      texConfig.format = GL_RGBA;
      texConfig.type = GL_FLOAT;
      texConfig.wrapS = GL_LINEAR;
      texConfig.wrapT = GL_LINEAR;
      _opaqueTexture =
        new reto::Texture2D( texConfig, _transSystemWidth, _transSystemHeight );
      _accumTexture =
        new reto::Texture2D( texConfig, _transSystemWidth, _transSystemHeight );
      _revealageTexture =
        new reto::Texture2D( texConfig, _transSystemWidth, _transSystemHeight );

      texConfig.internalFormat = GL_DEPTH_COMPONENT;
      texConfig.format = GL_DEPTH_COMPONENT;
      texConfig.type = GL_UNSIGNED_INT;
      _depthTexture =
        new reto::Texture2D( texConfig, _transSystemWidth, _transSystemHeight );

      glGenFramebuffers( 1, &_opaqueFbo );
      glBindFramebuffer( GL_FRAMEBUFFER, _opaqueFbo );
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, _opaqueTexture->handler( ), 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             GL_TEXTURE_2D, _depthTexture->handler( ), 0);

      glGenFramebuffers( 1, &_transFbo );
      glBindFramebuffer( GL_FRAMEBUFFER, _transFbo );
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, _accumTexture->handler( ), 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                             GL_TEXTURE_2D, _revealageTexture->handler( ), 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             GL_TEXTURE_2D, _depthTexture->handler( ), 0);
      GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
      glDrawBuffers( 2, drawBuffers );

      _uploadQuad( );

      _transSystemInit = true;
    }
  }


  void Renderer::setUpOpaqueTransparencyScene(
    Eigen::Vector3f backgroundColor_, unsigned int width_,
    unsigned int height_ )
  {
    if ( _transSystemInit )
    {
      if ( _transSystemWidth != width_ || _transSystemHeight != height_ )
      {
        _opaqueTexture->resize( width_, height_ );
        _accumTexture->resize( width_, height_ );
        _revealageTexture->resize( width_, height_ );
        _depthTexture->resize( width_, height_ );
        _transSystemWidth = width_;
        _transSystemHeight = height_;
      }

      _transparencyStatus = TTransparencyStatus::DISABLE;
      glBindFramebuffer( GL_FRAMEBUFFER, _opaqueFbo );
      glClearColor( backgroundColor_.x( ), backgroundColor_.y( ),
                    backgroundColor_.z( ), 1.0f );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      glDisable( GL_BLEND );
      glEnable( GL_DEPTH_TEST );
      glDepthMask(GL_TRUE);

    }
  }

  void Renderer::setUpTransparentTransparencyScene( void )
  {
    if ( _transSystemInit )
    {
      _transparencyStatus = TTransparencyStatus::ENABLE;
      glBindFramebuffer( GL_FRAMEBUFFER, _transFbo );
      float zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
      float ones[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      glClearBufferfv( GL_COLOR, 0, zeros );
      glClearBufferfv( GL_COLOR, 1, ones );
      glEnable( GL_BLEND );
      glBlendFunci(0, GL_ONE, GL_ONE);
      glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask( GL_FALSE );
    }
  }

  void Renderer::composeTransparencyScene( unsigned int finalFbo_ )
  {
    glBindFramebuffer( GL_FRAMEBUFFER, finalFbo_ );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glDepthMask(GL_TRUE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    _programTransCompose->use( );
    glBindVertexArray( _quadVao );
    _opaqueTexture->bind( 0 );
    _accumTexture->bind( 1 );
    _revealageTexture->bind( 2 );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                    (void*) 0 );
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

  void Renderer::_uploadQuad( void )
  {
    std::vector< float > positions( 8 );
    positions[0] = -1.0; positions[1] = 1.0;
    positions[2] = -1.0; positions[3] = -1.0;
    positions[4] = 1.0; positions[5] = -1.0;
    positions[6] = 1.0; positions[7] = 1.0;
    std::vector< unsigned int > indices( 6 );
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;

    glGenVertexArrays( 1, &_quadVao );
    glBindVertexArray( _quadVao );

    std::vector< unsigned int > vbos(2);
    glGenBuffers( 2, vbos.data( ));

    glBindBuffer( GL_ARRAY_BUFFER, vbos[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * positions.size( ),
                  positions.data( ), GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbos[1] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int) *
                  indices.size( ), indices.data( ), GL_STATIC_DRAW );
    glBindVertexArray( 0 );
  }

}
