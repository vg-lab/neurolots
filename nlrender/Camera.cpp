/**
 * @file    Camera.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "Camera.h"

#include <cmath>

#include <thread>

namespace nlrender
{

  Camera::Camera( float fov_, float ratio_, float nearPlane_, float farPlane_,
      Eigen::Vector3f pivot_, float radius_, float yaw_, float pitch_ )
    : _ratio( ratio_ )
    , _nearPlane( nearPlane_ )
    , _farPlane( farPlane_ )
    , _pivot( pivot_ )
    , _radius( radius_ )
#ifdef NEUROLOTS_USE_ZEROEQ
    , _zeqConnection( false )
#endif
    , _isAniming( false )
    , _firstStep( false )
    , _speedPivot( 1.0f / 60.0f )
    , _speedRadius( 1.0f / 60.0f )
    , _animDuration( 2.0f )
  {
    _fov = fov_ * ( M_PI / 360.0f );
    _f = 1.0f / tan( _fov );

    _previusTime = std::chrono::system_clock::now( );

    _Rotation( _RotationFromPY( pitch_, yaw_ ));
    _BuildProjectionMatrix( );
    _BuildViewMatrix( );
  }

#ifdef NEUROLOTS_USE_ZEROEQ
  Camera::Camera( const std::string& uri_, float fov_, float ratio_,
      float nearPlane_, float farPlane_, Eigen::Vector3f pivot_, float radius_,
      float yaw_, float pitch_ )
    : _ratio( ratio_ )
    , _nearPlane( nearPlane_ )
    , _farPlane( farPlane_ )
    , _pivot( pivot_ )
    , _radius( radius_ )
    , _zeqConnection( true )
    , _isAniming( false )
    , _firstStep( false )
    , _speedPivot( 0.0f )
    , _speedRadius( 0.0f )
    , _animDuration( 2.0f )
  {
    _fov = fov_ * ( M_PI / 360.0f );
    _f = 1.0f / tan( _fov );

    _uri = servus::URI(uri_);

    _previusTime = std::chrono::system_clock::now( );

    _Rotation( _RotationFromPY( pitch_, yaw_ ));

    _publisher = new zeroeq::Publisher( uri_ );
    _subscriber = new zeroeq::Subscriber( uri_ );

    _subscriber->subscribe(
        lexis::render::LookOut::ZEROBUF_TYPE_IDENTIFIER( ),
        [ & ]( const void* data, size_t size )
        { _OnCameraEvent( lexis::render::LookOut::create( data, size ));});

    std::thread subsThread( [&]() { while( true) _subscriber->receive( 10000 );});

    _BuildProjectionMatrix( );
    _BuildViewMatrix( );
    _BuildViewProjectionMatrix( );
  }
#endif

  Camera::~Camera( void )
  {
  }

  // PUBLIC

  void Camera::LocalRotation( float yaw_, float pitch_ )
  {
    _Rotation( _RotationFromPY( yaw_, pitch_) * _rotation );
    _BuildViewMatrix( );
    _BuildViewProjectionMatrix( );
  }

  bool Camera::Anim( void )
  {
    std::chrono::time_point< std::chrono::system_clock > actualTime =
      std::chrono::system_clock::now( );

    if ( _isAniming )
    {
      auto duration = std::chrono::duration_cast< std::chrono::milliseconds >
        ( actualTime - _previusTime );
      float dt = (( float ) duration.count( )) * 0.001f;

      Eigen::Vector3f actualPivot = _pivot;
      Eigen::Vector3f diffPivot = _targetPivot - actualPivot;

      float actualRadius = _radius;
      float diffRadius = _targetRadius - actualRadius;

      if ( _firstStep )
      {
        _speedPivot = diffPivot.norm( ) / _animDuration;
        _speedRadius = fabs( diffRadius ) / _animDuration;
        _firstStep = false;
      }

      float distancePivot = dt * _speedPivot;
      float distanceRadius = dt * _speedRadius;

      bool pivotInPlace = false;
      bool radiusInPlace = false;

      if (( pivotInPlace = ( diffPivot.norm() <= distancePivot )))
        _pivot = _targetPivot;
      else
        _pivot = actualPivot + diffPivot.normalized() * distancePivot;

      _BuildViewMatrix( );
      _BuildViewProjectionMatrix( );

      if (( radiusInPlace = ( fabs( diffRadius ) <= distanceRadius )))
        _radius = _targetRadius;
      else
        _radius = actualRadius + diffRadius / fabs( diffRadius ) *
            distanceRadius;

      _BuildViewMatrix( );
      _BuildViewProjectionMatrix( );

      _isAniming = !( pivotInPlace && radiusInPlace );
      _previusTime = actualTime;
      return true;
    }
    _previusTime = actualTime;
    return false;
  }

  // GETTERS

  float Camera::Fov( void )
  {
    return _fov;
  }

  Eigen::Vector3f Camera::Pivot( void )
  {
    return _pivot;
  }

  float Camera::Radius( void )
  {
    return _radius;
  }

  float* Camera::ProjectionMatrix( void )
  {
    return _projVec.data( );
  }

  float* Camera::ViewMatrix( void )
  {
    return _viewVec.data( );
  }

  float* Camera::ViewProjectionMatrix( void )
  {
    return _viewProjVec.data( );
  }

  float* Camera::Position( void )
  {
    return _positionVec.data( );
  }

#ifdef NEUROLOTS_USE_ZEROEQ
  zeroeq::Subscriber* Camera::Subscriber( void )
  {
    return _subscriber;
  }
#endif


  // SETTERS

  void Camera::Ratio( float ratio_ )
  {
    _ratio = ratio_;
    _BuildProjectionMatrix( );
    _BuildViewProjectionMatrix( );
  }

  void Camera::Pivot( Eigen::Vector3f pivot_ )
  {
    if ( !_isAniming )
    {
      _pivot = pivot_;
      _BuildViewMatrix( );
      _BuildViewProjectionMatrix( );
    }
  }

  void Camera::Radius( float radius_ )
  {
    if ( !_isAniming )
    {
      _radius = radius_;
      _BuildViewMatrix( );
      _BuildViewProjectionMatrix( );
    }
  }

  void Camera::Rotation( float yaw_, float pitch_ )
  {
    if ( !_isAniming )
    {
      _Rotation( _RotationFromPY( yaw_, pitch_ ));
      _BuildViewMatrix( );
      _BuildViewProjectionMatrix( );
    }
  }

  void Camera::TargetPivot( Eigen::Vector3f targetPivot_ )
  {
    _isAniming = true;
    _firstStep = true;
    _targetPivot = targetPivot_;
  }

  void Camera::TargetRadius( float targetRadius_ )
  {
    _isAniming = true;
    _firstStep = true;
    _targetRadius = targetRadius_;
  }

  void Camera::TargetPivotRadius( Eigen::Vector3f targetPivot_,
      float targetRadius_ )
  {
    TargetPivot( targetPivot_ );
    TargetRadius( targetRadius_ );
  }

  void Camera::AnimDuration( float animDuration_ )
  {
    _animDuration = animDuration_;
  }


  // PRIVATE

#ifndef NEUROLOTS_USE_ZEROEQ

  void Camera::_PositionVectorized( const std::vector<float>& positionVec_ )
  {
    _positionVec = positionVec_;
  }

  void Camera::_Rotation( Eigen::Matrix3f rotation_ )
  {
    _rotation = rotation_;
  }

  void Camera::_ViewMatrixVectorized( std::vector<float>& viewVec_ )
  {
    _viewVec = viewVec_;
  }

#else

  void Camera::_PositionVectorized( const std::vector<float>& positionVec_ )
  {
    _positionMutex.lock( );
    _positionVec = positionVec_;
    _positionMutex.unlock( );
  }

  void Camera::_Rotation( const Eigen::Matrix3f& rotation_ )
  {
    _rotationMutex.lock( );
    _rotation = rotation_;
    _rotationMutex.unlock( );
  }

  void Camera::_ViewMatrixVectorized( const std::vector<float>& viewVec_ )
  {
    _viewMatrixMutex.lock( );
    _viewVec = viewVec_;
    _viewMatrixMutex.unlock( );
  }

#endif

  void Camera::_BuildProjectionMatrix( void )
  {
    _projVec.resize(16);

    // row 1
    _projVec[0] = _f / _ratio;
    _projVec[1] = .0f;
    _projVec[2] = .0f;
    _projVec[3] = .0f;
    // row 2
    _projVec[4] = .0f;
    _projVec[5] = _f;
    _projVec[6] = .0f;
    _projVec[7] = .0f;
    // row 3
    _projVec[8] = .0f;
    _projVec[9] = .0f;
    _projVec[10] = ( _farPlane + _nearPlane ) / ( _nearPlane - _farPlane );
    _projVec[11] = -1.0f;
    // row 4
    _projVec[12] = .0f;
    _projVec[13] = .0f;
    _projVec[14] = ( 2.0f * _farPlane * _nearPlane ) / ( _nearPlane - _farPlane );
    _projVec[15] = .0f;
  }

  void Camera::_BuildViewMatrix( void )
  {
    Eigen::Vector3f position = _rotation.transpose( ) *
    Eigen::Vector3f( 0.0f, 0.0f, 1.0f ) * _radius + _pivot;

    Eigen::Vector3f pivot = _rotation * _pivot;

    std::vector<float> positionVec;
    positionVec.resize( 3 );
    positionVec[ 0 ] =  position.x( );
    positionVec[ 1 ] =  position.y( );
    positionVec[ 2 ] =  position.z( );

    _PositionVectorized( positionVec );

    std::vector<float> viewVec(16);

    // row 1
    viewVec[0] = _rotation( 0, 0 );
    viewVec[1] = _rotation( 1, 0 );
    viewVec[2] = _rotation( 2, 0 );
    viewVec[3] = .0f;
    // row 2
    viewVec[4] = _rotation( 0, 1 );
    viewVec[5] = _rotation( 1, 1 );
    viewVec[6] = _rotation( 2, 1 );
    viewVec[7] = .0f;
    // row 3
    viewVec[8] = _rotation( 0, 2 );
    viewVec[9] = _rotation( 1, 2 );
    viewVec[10] = _rotation( 2, 2 );
    viewVec[11] = .0f;
    // row 4
    viewVec[12] = - pivot.x( );
    viewVec[13] = - pivot.y( );
    viewVec[14] = - pivot.z( ) - _radius;
    viewVec[15] = 1.0f;

    _ViewMatrixVectorized( viewVec );

#ifdef NEUROLOTS_USE_ZEROEQ
    if ( _zeqConnection )
    {
      std::vector< double > viewm ( viewVec.begin( ), viewVec.end( ));

      lexis::render::LookOut lookout;
      lookout.setMatrix( viewm );
      _publisher->publish( lookout );
//      std::cout << "Evento publicado" << std::endl;
    }
#endif
    _ViewMatrixVectorized( viewVec );
  }

  void Camera::_BuildViewProjectionMatrix( void )
  {
    Eigen::Matrix4f v, p, vp;
    v <<  _viewVec[0], _viewVec[4], _viewVec[8], _viewVec[12],
           _viewVec[1], _viewVec[5], _viewVec[9], _viewVec[13],
           _viewVec[2], _viewVec[6], _viewVec[10], _viewVec[14],
           _viewVec[3], _viewVec[7], _viewVec[11], _viewVec[15];

    p  <<  _projVec[0], _projVec[4], _projVec[8], _projVec[12],
           _projVec[1], _projVec[5], _projVec[9], _projVec[13],
           _projVec[2], _projVec[6], _projVec[10], _projVec[14],
           _projVec[3], _projVec[7], _projVec[11], _projVec[15];

    vp = p * v;

    _viewProjVec.resize( 16 );

    _viewProjVec[0] = vp( 0, 0 );
    _viewProjVec[1] = vp( 1, 0 );
    _viewProjVec[2] = vp( 2, 0 );
    _viewProjVec[3] = vp( 3, 0 );

    _viewProjVec[4] = vp( 0, 1 );
    _viewProjVec[5] = vp( 1, 1 );
    _viewProjVec[6] = vp( 2, 1 );
    _viewProjVec[7] = vp( 3, 1 );

    _viewProjVec[8] = vp( 0, 2 );
    _viewProjVec[9] = vp( 1, 2 );
    _viewProjVec[10] = vp( 2, 2 );
    _viewProjVec[11] = vp( 3, 2 );

    _viewProjVec[12] = vp( 0, 3 );
    _viewProjVec[13] = vp( 1, 3 );
    _viewProjVec[14] = vp( 2, 3 );
    _viewProjVec[15] = vp( 3, 3 );

  }

#ifdef NEUROLOTS_USE_ZEROEQ
  void Camera::_OnCameraEvent( lexis::render::ConstLookOutPtr lookoutPtr_ )
  {
    std::vector< double > aux = std::move( lookoutPtr_->getMatrixVector( ));
    std::vector<float> viewMatrixVec( aux.begin( ), aux.end( ));
    _ViewMatrixVectorized( viewMatrixVec );

    Eigen::Matrix3f rot;
    rot << viewMatrixVec[0], viewMatrixVec[4], viewMatrixVec[8],
           viewMatrixVec[1], viewMatrixVec[5], viewMatrixVec[9],
           viewMatrixVec[2], viewMatrixVec[6], viewMatrixVec[10];

    Eigen::Vector3f pos = - rot.inverse() * Eigen::Vector3f( viewMatrixVec[12],
                          viewMatrixVec[13], viewMatrixVec[14] );

    std::vector<float> posVec( 3 );
    posVec[ 0 ] = pos.x( );
    posVec[ 1 ] = pos.y( );
    posVec[ 2 ] = pos.z( );

    _PositionVectorized( posVec );
  }

#endif

  Eigen::Matrix3f Camera::_RotationFromPY( float yaw_, float pitch_ )
  {
    Eigen::Matrix3f rot;
    Eigen::Matrix3f rYaw;
    Eigen::Matrix3f rPitch;

    float sinYaw, cosYaw, sinPitch, cosPitch;

    sinYaw = sin( yaw_ );
    cosYaw = cos( yaw_ );
    sinPitch = sin( pitch_ );
    cosPitch = cos( pitch_ );

    rYaw << cosYaw, 0.0f, sinYaw,
            0.0f,   1.0f, 0.0f,
            -sinYaw, 0.0f, cosYaw;

    rPitch << 1.0f, 0.0f,     0.0f,
              0.0f, cosPitch, -sinPitch,
              0.0f, sinPitch, cosPitch;

    rot = rPitch * rYaw;
    return rot;
  }

} // end namespace nlrender

// EOF
