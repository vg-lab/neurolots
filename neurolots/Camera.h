/**
 * @file    Camera.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_CAMERA__
#define __NEUROLOTS_CAMERA__


// Eigen
#include <Eigen/Dense>

#include <vector>
#include <iostream>


namespace neurolots
{

  class Camera
  {

    public:

      Camera( float _fov = 45.0f, float _ratio = 16/9, float _nearPlane = 0.1f,
              float _farPlane = 10000.0f, float _x = 0.0f, float _y = 0.0f,
              float _z = 20.0f, float _yaw = 0.0f, float _pitch = 0.0f,
              float _roll = 0.0f );
      ~Camera(void);

      void UpdateRatio( float _ratio );
      void UpdateRotation( float _yaw, float _pitch, float _roll );
      void UpdateRotation( float _yaw, float _pitch );
      void UpdateRotation( float _roll );
      void UpdatePosition( float _x, float _y, float _z );
      void LocalDisplace( float _x, float _y, float _z );
      void IncrementRotation( float _yaw, float _pitch );
      void IncrementRotation( float _roll );
      float * GetProjectionMatrix( void );
      float * GetViewMatrix( void );
      float * GetCameraPos( void );

    private:

      void BuildProjectionMatrix( void );
      void BuildRotationMatrix( void );
      void BuildViewMatrix( void );

      Eigen::Vector3f position_;
      Eigen::Vector3f rotation_;

      float f_;
      float fov_;
      float ratio_;
      float nearPlane_;
      float farPlane_;

      Eigen::Matrix4f proy_;
      Eigen::Matrix4f view_;
      Eigen::Matrix3f rot_;

      std::vector<float> positionVec_;
      std::vector<float> proyVec_;
      std::vector<float> viewVec_;
  };


} //end namespace neurolots

#endif // _NEUROLOTS_CAMERA__

// EOF
