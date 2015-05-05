#include "Camera.h"

using Eigen::Vector3f;
using Eigen::Matrix3f;
using Eigen::Matrix4f;

namespace neurolots
{

  Camera::Camera( float fov_, float ratio_, float nearPlane_, float farPlane_,
      float x_, float y_, float z_, float yaw_, float pitch_, float roll_ )
    : _fov( fov_ )
    , _ratio( ratio_ )
    , _nearPlane( nearPlane_ )
    , _farPlane( farPlane_ )
    , _zeqConnection( false )

  {
    _position = Vector3f( x_, y_, z_ );
    _rotation = Vector3f( yaw_, pitch_, roll_ );

    _BuildProjectionMatrix( );
    _BuildRotationMatrix( );
    _BuildViewMatrix( );
  }

  Camera::Camera( const char * uri_, float fov_, float ratio_, float nearPlane_,
      float farPlane_, float x_, float y_, float z_, float yaw_, float pitch_,
      float roll_ )
    : _fov( fov_ )
    , _ratio( ratio_ )
    , _nearPlane( nearPlane_ )
    , _farPlane( farPlane_ )
    , _zeqConnection( true )

  {
    _uri = lunchbox::URI(uri_);
    _position = Vector3f( x_, y_, z_ );
    _rotation = Vector3f( yaw_, pitch_, roll_ );

    _BuildProjectionMatrix( );
    _BuildRotationMatrix( );
    _BuildViewMatrix( );
  }

  Camera::~Camera( void )
  {
  }

  // PUBLIC

  void Camera::LocalDisplace( float x_, float y_, float z_ )
  {
    Vector3f localX, localY, localZ;

    Eigen::Matrix3f rotT = _rot.transpose();

    localX = rotT * Vector3f::UnitX();
    localY = rotT * Vector3f::UnitY();
    localZ = rotT * Vector3f::UnitZ();

    _position += x_ * localX + y_ * localY + z_ * localZ;

    _BuildViewMatrix( );
  }

  void Camera::IncrementRotation( float yaw_, float pitch_ )
    {
      _rotation.x( ) += yaw_;
      _rotation.y( ) += pitch_;

      _BuildRotationMatrix( );
      _BuildViewMatrix( );
    }

  void Camera::IncrementRotation( float roll_ )
  {
    _rotation.z( ) += roll_;
    _BuildRotationMatrix( );
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

  // SETTERS

  void Camera::Ratio( float ratio_ )
  {
    _ratio = ratio_;
    _BuildProjectionMatrix( );
  }

  void Camera::Rotation( float yaw_, float pitch_ )
  {
    _rotation.x( ) = yaw_;
    _rotation.y( ) = pitch_;

    _BuildRotationMatrix( );
    _BuildViewMatrix( );
  }

  void Camera::Rotation( float roll_ )
  {
    _rotation.z( ) = roll_;

    _BuildRotationMatrix( );
    _BuildViewMatrix( );
  }

  void Camera::Rotation( float yaw_, float pitch_, float roll_ )
  {
    _rotation.x( ) = yaw_;
    _rotation.y( ) = pitch_;
    _rotation.z( ) = roll_;

    _BuildRotationMatrix();
    _BuildViewMatrix();
  }

  void Camera::Position( float x_, float y_, float z_ )
  {
    _position.x() = x_;
    _position.y() = y_;
    _position.z() = z_;

    _BuildViewMatrix();
  }

  // PRIVATE

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

  void Camera::_BuildRotationMatrix( void )
  {
    Matrix3f rYaw;
    Matrix3f rPitch;
    Matrix3f rRoll;

    float sy, cy, sp, cp, sr, cr;
    sy = sin( _rotation.x( ));
    cy = cos( _rotation.x( ));

    sp = sin( _rotation.y( ));
    cp = cos( _rotation.y( ));

    sr = sin( _rotation.z( ));
    cr = cos( _rotation.z( ));

    rYaw << cy, 0.0f, sy,
            0.0f, 1.0f, 0.0f,
            -sy, 0.0f, cy;

    rPitch << 1.0f, 0.0f, 0.0f,
              0.0f, cp, -sp,
              0.0f, sp, cp;

    rRoll << cr, -sr, 0.0f,
             sr, cr, 0.0f,
             0.0f, 0.0f, 1.0f;

    _rot= rRoll * rPitch * rYaw;
  }

  void Camera::_BuildViewMatrix( void )
  {
    Vector3f desp = _rot * -_position;

    std::vector<float> viewVec(16);

    // row 1
    viewVec[0] = _rot( 0, 0 );
    viewVec[1] = _rot( 1, 0 );
    viewVec[2] = _rot( 2, 0 );
    viewVec[3] = .0f;
    // row 2
    viewVec[4] = _rot( 0, 1 );
    viewVec[5] = _rot( 1, 1 );
    viewVec[6] = _rot( 2, 1 );
    viewVec[7] = .0f;
    // row 3
    viewVec[8] = _rot( 0, 2 );
    viewVec[9] = _rot( 1, 2 );
    viewVec[10] = _rot( 2, 2 );
    viewVec[11] = .0f;
    // row 4
    viewVec[12] = desp.x( );
    viewVec[13] = desp.y( );
    viewVec[14] = desp.z( );
    viewVec[15] = 1.0f;

    _ViewMatrixVectorized( viewVec );
  }

  void Camera::_ViewMatrixVectorized( std::vector<float>& viewVec_ )
  {
    _viewMatrixMutex.lock( );
    _viewVec = viewVec_;
    _viewMatrixMutex.unlock( );
  }


} // end namespace neurolots

// EOF
