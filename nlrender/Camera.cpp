#include "Camera.h"

#include <cmath>

namespace neurolots
{

  Camera::Camera( float fov_, float ratio_, float nearPlane_, float farPlane_,
      Eigen::Vector3f pivot_, float radius_, float yaw_, float pitch_ )
    : _ratio( ratio_ )
    , _nearPlane( nearPlane_ )
    , _farPlane( farPlane_ )
    , _pivot( pivot_ )
    , _radius( radius_ )
#ifdef NEUROLOTS_USE_ZEQ
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

    _previusTime = std::clock( );

    _Rotation( _RotationFromPY( pitch_, yaw_ ));
    _BuildProjectionMatrix( );
    _BuildViewMatrix( );
  }

#ifdef NEUROLOTS_USE_ZEQ
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
    , _speedPivot( 1.0f / 60 )
    , _speedRadius( 1.0f / 60 )
    , _animDuration( 2.0f )
  {
    _fov = fov_ * ( M_PI / 360.0f );
    _f = 1.0f / tan( _fov );

    _uri = servus::URI(uri_);

    _previusTime = std::clock( );

    _Rotation( _RotationFromPY( pitch_, yaw_ ));

    _publisher = new zeq::Publisher( _uri );
    _subscriber = new zeq::Subscriber( _uri );

    _subscriber->registerHandler( zeq::hbp::EVENT_CAMERA,
      boost::bind( &Camera::_OnCameraEvent , this, _1 ));

    pthread_create( &_subscriberThread, NULL, _Subscriber, this );

    _BuildProjectionMatrix( );
    _BuildViewMatrix( );
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
  }

  void Camera::Anim( void )
  {
    if ( _isAniming )
    {
      float dt = ( std::clock( ) - _previusTime ) / (double) CLOCKS_PER_SEC;

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
      {
        _pivot = _targetPivot;
        _BuildViewMatrix( );
      }
      else
      {
        _pivot = actualPivot + diffPivot.normalized() * distancePivot;
        _BuildViewMatrix( );
      }

      if (( radiusInPlace = ( fabs( diffRadius ) <= distanceRadius )))
      {
        _radius = _targetRadius;
        _BuildViewMatrix( );
      }
      else
      {
        _radius = actualRadius + diffRadius / fabs( diffRadius ) *
            distanceRadius;
        _BuildViewMatrix( );
      }
      _isAniming = !( pivotInPlace && radiusInPlace );
    }
    _previusTime = std::clock( );
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

  float* Camera::Position( void )
  {
    return _positionVec.data( );
  }

#ifdef NEUROLOTS_USE_ZEQ
  zeq::Subscriber* Camera::Subscriber( void )
  {
    return _subscriber;
  }
#endif


  // SETTERS

  void Camera::Ratio( float ratio_ )
  {
    _ratio = ratio_;
    _BuildProjectionMatrix( );
  }

  void Camera::Pivot( Eigen::Vector3f pivot_ )
  {
    if ( !_isAniming )
    {
      _pivot = pivot_;
      _BuildViewMatrix( );
    }
  }

  void Camera::Radius( float radius_ )
  {
    if ( !_isAniming )
    {
      _radius = radius_;
      _BuildViewMatrix( );
    }
  }

  void Camera::Rotation( float yaw_, float pitch_ )
  {
    if ( !_isAniming )
    {
      _Rotation( _RotationFromPY( yaw_, pitch_ ));
      _BuildViewMatrix( );
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

#ifndef NEUROLOTS_USE_ZEQ

  void Camera::_PositionVectorized( std::vector<float>& positionVec_ )
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

  void Camera::_PositionVectorized( std::vector<float>& positionVec_ )
  {
    _positionMutex.lock( );
    _positionVec = positionVec_;
    _positionMutex.unlock( );
  }

  void Camera::_Rotation( Eigen::Matrix3f rotation_ )
  {
    _rotationMutex.lock( );
    _rotation = rotation_;
    _rotationMutex.unlock( );
  }

  void Camera::_ViewMatrixVectorized( std::vector<float>& viewVec_ )
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
#ifdef NEUROLOTS_USE_ZEQ
    if ( _zeqConnection )
    {
      _publisher->publish( zeq::hbp::serializeCamera( viewVec ));
//      std::cout << "Evento publicado" << std::endl;
    }
#endif
    _ViewMatrixVectorized( viewVec );
  }

#ifdef NEUROLOTS_USE_ZEQ
  void Camera::_OnCameraEvent( const zeq::Event& event_ )
  {
    std::vector<float> viewMatrixVec = zeq::hbp::deserializeCamera( event_ );
    _ViewMatrixVectorized( viewMatrixVec );

    Eigen::Matrix3f rot;
    rot << viewMatrixVec[0], viewMatrixVec[4], viewMatrixVec[8],
           viewMatrixVec[1], viewMatrixVec[5], viewMatrixVec[9],
           viewMatrixVec[2], viewMatrixVec[6], viewMatrixVec[10];

    Eigen::Vector3f pos = - rot.inverse() * Eigen::Vector3f( viewMatrixVec[12],
                          viewMatrixVec[13], viewMatrixVec[14] );
    std::vector<float> posVec;
    posVec.resize( 3 );
    posVec[ 0 ] = pos.x( );
    posVec[ 1 ] = pos.y( );
    posVec[ 2 ] = pos.z( );

    _PositionVectorized( posVec );
  }

  void* Camera::_Subscriber( void* camera_ )
  {
    Camera* camera = ( Camera* )camera_;
    zeq::Subscriber* subscriber = camera->Subscriber();
    std::cout << "Waiting Camera Events..." << std::endl;
    while ( true )
    {
      subscriber->receive( 10000 );
    }

    pthread_exit( NULL );
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

} // end namespace neurolots

// EOF
