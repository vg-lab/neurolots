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

#include <zeq/zeq.h>
#include <zeq/hbp/hbp.h>
#include <lunchbox/uri.h>

#include <pthread.h>
#include <mutex>
#include <boost/bind.hpp>

namespace neurolots
{

  class Camera
  {

    public:

      Camera( float fov_ = 45.0f, float ratio_ = ((float)16)/9,
        float nearPlane_ = 0.1f, float farPlane_ = 10000.0f, float x_ = 0.0f,
        float y_ = 0.0f, float z_ = 100.0f, float yaw_ = 0.0f,
        float pitch_ = 0.0f );

      Camera( const char * uri_, float fov_ = 45.0f,
        float ratio_ = ((float)16)/9, float nearPlane_ = 0.1f,
        float farPlane_ = 10000.0f, float x_ = 0.0f, float y_ = 0.0f,
        float z_ = 100.0f, float yaw_ = 0.0f, float pitch_ = 0.0f );

      ~Camera(void);

      void LocalDisplace( Eigen::Vector3f displace_ );
      void LocalRotation( float yaw_, float pitch_ );

      // GETTERS

      float* ProjectionMatrix( void );
      float* ViewMatrix( void );
      float* Position( void );
      zeq::Subscriber* Subscriber( void );

      // SETTERS

      void Ratio( float ratio_ );
      void Position( Eigen::Vector3f position_ );
      void Rotation( float yaw_, float pitch_ );

    private:

      void _Position( Eigen::Vector3f position_ );
      void _Rotation( Eigen::Matrix3f rotation_ );
      void _ViewMatrixVectorized( std::vector<float>& viewVec_ );

      void _BuildProjectionMatrix( void );
      void _BuildViewMatrix( void );

      void _OnCameraEvent( const zeq::Event& event_ );
      static void* _Subscriber( void* camera_ );

      Eigen::Matrix3f _RotationFromPY( float yaw_, float pitch_ );

      float _f;
      float _fov;
      float _ratio;
      float _nearPlane;
      float _farPlane;

      Eigen::Vector3f _position;
      Eigen::Matrix3f _rotation;

      std::vector<float> _positionVec;
      std::vector<float> _projVec;
      std::vector<float> _viewVec;

      bool _zeqConnection;
      lunchbox::URI _uri;
      zeq::Publisher* _publisher;
      zeq::Subscriber* _subscriber;

      std::mutex _positionMutex;
      std::mutex _rotationMutex;
      std::mutex _viewMatrixMutex;

      pthread_t _subscriberThread;

  };


} //end namespace neurolots

#endif // _NEUROLOTS_CAMERA__

// EOF
