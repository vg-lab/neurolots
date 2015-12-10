/**
 * @file    VectorizedNode.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NLGENERATOR_VECTORIZED_NODE__
#define __NLGENERATOR_VECTORIZED_NODE__

#include <Eigen/Dense>

#include "GeometricPrimitive.h"
#include "Quad.h"

#include <nlgenerator/api.h>

namespace nlgenerator
{

  class VectorizedNode
  {

  public:

    NLGENERATOR_API
    VectorizedNode( void );

    NLGENERATOR_API
    ~VectorizedNode( void );

    NLGENERATOR_API
    void AddChild( VectorizedNode* child );

    //Setters

    NLGENERATOR_API
    void Id( int id_ );

    NLGENERATOR_API
    void Position( Eigen::Vector3f position_ );

    NLGENERATOR_API
    void Tangent( Eigen::Vector3f tangent_ );

    NLGENERATOR_API
    void Radius( float radius_ );

    NLGENERATOR_API
    void Primitive( GeometricPrimitivePtr primitive_ );

    NLGENERATOR_API
    void Father( VectorizedNode* father_ );

    //Getters

    NLGENERATOR_API
    int Id( void );

    NLGENERATOR_API
    Eigen::Vector3f Position( void );

    NLGENERATOR_API
    Eigen::Vector3f Tangent( void );

    NLGENERATOR_API
    float Radius( void );

    NLGENERATOR_API
    GeometricPrimitivePtr Primitive( void );

    NLGENERATOR_API
    VectorizedNode* Father( void );

    NLGENERATOR_API
    std::vector< VectorizedNode* > Childs( void );

    NLGENERATOR_API
    bool Bifurcation( void );

    NLGENERATOR_API
    bool FirstNode( void );


    private:

      int _id;
      Eigen::Vector3f _position;
      Eigen::Vector3f _tangent;
      float _radius;

      GeometricPrimitivePtr _primitive;

      VectorizedNode* _father;
      std::vector< VectorizedNode* > _childs;

      bool _bifurcation;
      bool _firstNode;


  };

  typedef VectorizedNode* VectorizedNodePtr;

}// end namespace nlgenerator

#endif // __NLGENERATOR_VECTORIZED_NODE__

//EOF
