#include "Camera.h"

using Eigen::Vector3f;
using Eigen::Matrix3f;
using Eigen::Matrix4f;

namespace neurolots
{

  Camera::Camera( float fov_, float ratio_, float nearPlane_, float farPlane_,
      float x_, float y_, float z_, float yaw_, float pitch_ )
    : _fov( fov_ )
    , _ratio( ratio_ )
    , _nearPlane( nearPlane_ )
    , _farPlane( farPlane_ )
    , _zeqConnection( false )
  {
    _Position( Vector3f( x_, y_, z_ ));
    _Rotation( _RotationFromPY( pitch_, yaw_ ));

    _BuildProjectionMatrix( );
    _BuildViewMatrix( );
  }

#ifdef NEUROLOTS_WITH_ZEQ
  Camera::Camera( std::string& uri_, float fov_, float ratio_, float nearPlane_,
      float farPlane_, float x_, float y_, float z_, float yaw_, float pitch_ )
    : _fov( fov_ )
    , _ratio( ratio_ )
    , _nearPlane( nearPlane_ )
    , _farPlane( farPlane_ )
    , _zeqConnection( true )
  {
    _uri = lunchbox::URI(uri_);

    _Position( Vector3f( x_, y_, z_ ));
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

  void Camera::LocalDisplace( Eigen::Vector3f displace_ )
  {
    Vector3f localX, localY, localZ;

    Eigen::Matrix3f rotT = _rotation.transpose();

    localX = rotT * Vector3f::UnitX();
    localY = rotT * Vector3f::UnitY();
    localZ = rotT * Vector3f::UnitZ();

    _Position( _position + displace_.x() * localX + displace_.y() * localY +
                 displace_.z() * localZ );

    _BuildViewMatrix( );
  }

  void Camera::LocalRotation( float yaw_, float pitch_ )
  {
    _Rotation(  _RotationFromPY( yaw_, pitch_ ) * _rotation );
    _BuildViewMatrix( );
  }

  // GETTERS

  float * Camera::ProjectionMatrix( void )
  {
    return _projVec.data( );
  }

  float *  Camera::ViewMatrix( void )
  {
    return _viewVec.data( );
  }

  float * Camera::Position(void)
  {
    _positionVec.resize(3);

    _positionVec[0] = _position.x( );
    _positionVec[1] = _position.y( );
    _positionVec[2] = _position.z( );

    return _positionVec.data( );
  }

#ifdef NEUROLOTS_WITH_ZEQ
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

  void Camera::Position( Eigen::Vector3f position_ )
  {
    _Position( position_ );
    _BuildViewMatrix();
  }

  void Camera::Rotation( float yaw_, float pitch_ )
  {
    _Rotation( _RotationFromPY( yaw_, pitch_ ));
  }

  // PRIVATE
#ifndef NEUROLOTS_WITH_ZEQ

  void Camera::_Position( Eigen::Vector3f position_ )
  {
    _position = position_;
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

  void Camera::_Position( Eigen::Vector3f position_ )
  {
    _positionMutex.lock( );
    _position = position_;
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
    _f = 1.0f / tan (_fov * ( 3.141599f / 360.0f ));

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
    Vector3f desp = _rotation * -_position;

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
    viewVec[12] = desp.x( );
    viewVec[13] = desp.y( );
    viewVec[14] = desp.z( );
    viewVec[15] = 1.0f;
#ifdef NEUROLOTS_WITH_ZEQ
    if ( _zeqConnection )
    {
      _publisher->publish( zeq::hbp::serializeCamera( viewVec ));
//      std::cout << "Evento publicado" << std::endl;
    }
#endif
    _ViewMatrixVectorized( viewVec );
  }

#ifdef NEUROLOTS_WITH_ZEQ
  void Camera::_OnCameraEvent( const zeq::Event& event_ )
  {
    std::vector<float> viewMatrixVec = zeq::hbp::deserializeCamera( event_ );
    _ViewMatrixVectorized( viewMatrixVec );

    Eigen::Matrix3f rot;
    rot << viewMatrixVec[0], viewMatrixVec[4], viewMatrixVec[8],
           viewMatrixVec[1], viewMatrixVec[5], viewMatrixVec[9],
           viewMatrixVec[2], viewMatrixVec[6], viewMatrixVec[10];

    _Rotation( rot );

    Eigen::Vector3f pos = - rot.inverse() * Eigen::Vector3f( viewMatrixVec[12],
                          viewMatrixVec[13], viewMatrixVec[14] );

    _Position( pos );
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
    Matrix3f rot;
    Matrix3f rYaw;
    Matrix3f rPitch;


    float sy, cy, sp, cp;
    sy = sin( yaw_ );
    cy = cos( yaw_ );
    sp = sin( pitch_ );
    cp = cos( pitch_ );

    rYaw << cy, 0.0f, sy,
      0.0f, 1.0f, 0.0f,
      -sy, 0.0f, cy;

    rPitch << 1.0f, 0.0f, 0.0f,
      0.0f, cp, -sp,
      0.0f, sp, cp;

    rot= rPitch * rYaw;
    return rot;
  }

} // end namespace neurolots

// EOF
