/**
 * @file    Fem.h
 * @brief
 * @author  Juan José García <juanjosegarciacan@gmail.com>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#ifndef __FEM_FEM__
#define __FEM_FEM__

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>
#include <iostream>
#include <cstdio>

#include "Node.h"
#include "Tetrahedron.h"

namespace fem
{


  class Fem
  {
    public:
      Fem( std::vector<Node *> _nodes, std::vector<Tetrahedron> _tetrahedra,
           double _v = 0.3, double _E = 1 );
      Fem( void );
      ~Fem( void );
      void conformkMatrix( void );
      void conformb( void );
      void addTokMatrix( int row, int col, Eigen::Matrix3d sum_ );
      void addTob( int row, Eigen::Vector3d sum_);
      void solve( void );
      void computeTetraheda( void );
      void recomputeNodes( void );
      void printMatrix(Eigen::MatrixXd _m);
      void setMaterial( double _v, double _E );

      void passToVectors( void );

      void toInitPos( void );
      void toMiddlePos( void );
      void toFinalPos( void );


      std::vector<Node *> nodes;
      std::vector<Tetrahedron> tetrahedra;
      std::vector<float> vertices;
      std::vector<float> centers;
      std::vector<int> ids;


      double v;
      double E;
      Eigen::MatrixXd Material;
      Eigen::MatrixXd kMatrix;
      Eigen::VectorXd b;
      Eigen::VectorXd u;
      Eigen::ConjugateGradient< Eigen::MatrixXd > solver;
  };

}// end namespace fem

#endif // __FEM_FEM__

//EOF
