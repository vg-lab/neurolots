/**
 * @file    VectorizedNode.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __NEUROLOTS_VECTORIZED_NODE__
#define __NEUROLOTS_VECTORIZED_NODE__

#include <Eigen/Dense>

#include "GeometricPrimitive.h"

namespace neurolots
{

  class VectorizedNode
  {

    public:

      VectorizedNode( void );
      ~VectorizedNode( void );

      void AddChild( VectorizedNode* child );

      //Setters
      void Id( int id_ );
      void Position( Eigen::Vector3f position_ );
      void Tangent( Eigen::Vector3f tangent_ );
      void Radius( float radius_ );
      void Primitive( GeometricPrimitivePtr primitive_ );
      void Father( VectorizedNode* father_ );

      //Getters
      int Id( void );
      Eigen::Vector3f Position( void );
      Eigen::Vector3f Tangent( void );
      float Radius( void );
      GeometricPrimitivePtr Primitive( void );
      VectorizedNode* Father( void );
      std::vector< VectorizedNode* > Childs( void );
      bool Bifurcation( void );
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

}// end namespace neurolots

#endif // __NEUROLOTS_VECTORIZED_NODE__

//EOF
