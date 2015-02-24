#include "Camera.h"

using Eigen::Vector3f;
using Eigen::Matrix3f;
using Eigen::Matrix4f;

namespace neurolots
{

  Camera::Camera( float _fov, float _ratio, float _nearPlane, float _farPlane,
                  float _x, float _y, float _z, float _yaw, float _pitch,
                  float _roll)
    : fov_( _fov )
    , ratio_( _ratio)
    , nearPlane_( _nearPlane )
    , farPlane_( _farPlane )

  {
    position_ = Vector3f( _x, _y, _z );
    rotation_ = Vector3f( _yaw, _pitch, _roll );

    BuildProjectionMatrix( );
    BuildRotationMatrix( );
    BuildViewMatrix( );
  }

  Camera::~Camera( void )
  {
  }

  void Camera::BuildProjectionMatrix( void )
  {
    f_ = 1.0f / tan (fov_ * ( 3.141599f / 360.0f ));

    proy_ << f_ / ratio_, 0.0f, 0.0f, 0.0f,
        0.0f, f_, 0.0f, 0.0f,
        0.0f, 0.0f,( farPlane_ + nearPlane_ ) / ( nearPlane_ - farPlane_ ),
        ( 2.0f * farPlane_ * nearPlane_ ) / ( nearPlane_ - farPlane_ ),
        0.0f, 0.0f, -1.0f, 0.0f;
  }

  void Camera::UpdateRatio( float _ratio )
  {
    ratio_ = _ratio;
    proy_(0,0) = f_ / ratio_;
    BuildProjectionMatrix( );
  }

  void Camera::BuildRotationMatrix( void )
  {
    Matrix3f rYaw;
    Matrix3f rPitch;
    Matrix3f rRoll;

    float sy, cy, sp, cp, sr, cr;
    sy = sin( rotation_.x( ));
    cy = cos( rotation_.x( ));

    sp = sin( rotation_.y( ));
    cp = cos( rotation_.y( ));

    sr = sin( rotation_.z( ));
    cr = cos( rotation_.z( ));

    rYaw << cy, 0.0f, sy,
            0.0f, 1.0f, 0.0f,
            -sy, 0.0f, cy;

    rPitch << 1.0f, 0.0f, 0.0f,
              0.0f, cp, -sp,
              0.0f, sp, cp;

    rRoll << cr, -sr, 0.0f,
             sr, cr, 0.0f,
             0.0f, 0.0f, 1.0f;

    rot_= rRoll * rPitch * rYaw;
  }

  void Camera::BuildViewMatrix( void )
  {
    Vector3f desp = rot_ * -position_;

    view_ << rot_( 0,0 ), rot_( 0,1 ), rot_( 0,2 ), desp.x( ),
            rot_( 1,0 ), rot_( 1,1 ), rot_( 1,2 ), desp.y( ),
            rot_( 2,0 ), rot_( 2,1 ), rot_( 2,2 ), desp.z( ),
            0.0f, 0.0f, 0.0f, 1.0f;
  }

  void Camera::UpdateRotation( float yaw, float pitch )
  {
    rotation_.x( ) = yaw;
    rotation_.y( ) = pitch;

    BuildRotationMatrix( );
    BuildViewMatrix( );
  }

  void Camera::UpdateRotation( float roll )
  {
    rotation_.z( ) = roll;

    BuildRotationMatrix( );
    BuildViewMatrix( );
  }

  void Camera::UpdateRotation( float yaw, float pitch, float roll )
  {
    rotation_.x( ) = yaw;
    rotation_.y( ) = pitch;
    rotation_.z( ) = roll;

    BuildRotationMatrix();
    BuildViewMatrix();
  }

  void Camera::IncrementRotation( float yaw, float pitch )
  {
    rotation_.x( ) += yaw;
    rotation_.y( ) += pitch;
    BuildRotationMatrix();
    BuildViewMatrix();
  }

  void Camera::IncrementRotation( float roll )
  {
    rotation_.z() += roll;
    BuildRotationMatrix();
    BuildViewMatrix();
  }

  void Camera::UpdatePosition( float x, float y, float z )
  {
    position_.x() = x;
    position_.y() = y;
    position_.z() = z;

    BuildViewMatrix();
  }

  void Camera::LocalDisplace( float x, float y, float z )
  {
    Vector3f localX, localY, localZ;

    localX = rot_.inverse() * Vector3f::UnitX();
    localY = rot_.inverse() * Vector3f::UnitY();
    localZ = rot_.inverse() * Vector3f::UnitZ();

    position_ += x*localX + y*localY + z*localZ;

    BuildViewMatrix();
  }

  float * Camera::GetProjectionMatrix( void )
  {
    proyVec_.resize(16);

    proyVec_[0] = proy_( 0, 0 );  proyVec_[1] = proy_( 1, 0 );
    proyVec_[2] = proy_( 2, 0 ); proyVec_[3] = proy_( 3, 0 );

    proyVec_[4] = proy_( 0, 1 );  proyVec_[5] = proy_( 1, 1 );
    proyVec_[6] = proy_( 2, 1 ); proyVec_[7] = proy_( 3, 1 );

    proyVec_[8] = proy_( 0, 2 );  proyVec_[9] = proy_( 1, 2 );
    proyVec_[10] = proy_( 2, 2 ); proyVec_[11] = proy_( 3, 2 );

    proyVec_[12] = proy_( 0, 3 ); proyVec_[13] = proy_( 1, 3 );
    proyVec_[14] = proy_( 2, 3 ); proyVec_[15] = proy_( 3, 3 );

    return proyVec_.data( );
  }

  float *  Camera::GetViewMatrix( void )
  {
    viewVec_.resize( 16 );

    viewVec_[0] = view_( 0, 0 );  viewVec_[1] = view_( 1, 0 );
    viewVec_[2] = view_( 2, 0 ); viewVec_[3] = view_( 3, 0 );

    viewVec_[4] = view_( 0, 1 );  viewVec_[5] = view_( 1, 1 );
    viewVec_[6] = view_( 2, 1 ); viewVec_[7] = view_( 3, 1 );

    viewVec_[8] = view_( 0, 2 );  viewVec_[9] = view_( 1, 2 );
    viewVec_[10] = view_( 2, 2 ); viewVec_[11] = view_( 3, 2 );

    viewVec_[12] = view_( 0, 3 ); viewVec_[13] = view_( 1, 3 );
    viewVec_[14] = view_( 2, 3 ); viewVec_[15] = view_( 3, 3 );

    return viewVec_.data( );
  }

  float * Camera::GetCameraPos(void)
  {
    positionVec_.resize(3);

    positionVec_[0] = position_.x( );
    positionVec_[1] = position_.y( );
    positionVec_[2] = position_.z( );

    return positionVec_.data( );
  }

} // end namespace neurolots

// EOF
