/**
 * @file    Fem.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */


#ifndef __NLFEM_FEM__
#define __NLFEM_FEM__

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>
#include <iostream>
#include <cstdio>

#include <nlfem/api.h>

#include "Node.h"
#include "Tetrahedron.h"

namespace nlfem
{


  class Fem
  {
    public:

      NLFEM_API
      Fem( std::vector< NodePtr > nodes_,
          std::vector< TetrahedronPtr > tetrahedra_,
           double v_ = 0.3, double E_ = 1 );

      NLFEM_API
      ~Fem( void );

      NLFEM_API
      void Solve( void );

    private:
      void _AddTokMatrix( unsigned int id0, unsigned int id1,
                          Eigen::Matrix3f sum );
      void _AddToB( unsigned int id, Eigen::Vector3f sum);
      void _ComputeTetrahedra( void );
      void _ConformMatrixSystem( void );


      std::vector< NodePtr > _nodes;
      std::vector< TetrahedronPtr > _tetrahedra;
      std::vector< unsigned int > _ids;

      double _v;
      double _E;
      Eigen::MatrixXf _Material;
      std::vector< Eigen::Triplet< float >> _triplets;
      Eigen::SparseMatrix< float > _kMatrix;
      Eigen::VectorXf _b;
      Eigen::VectorXf _u;
      Eigen::ConjugateGradient< Eigen::SparseMatrix< float >> _solver;
      unsigned int _dim;
  };

}// end namespace nlfem

#endif // __NLFEM_FEM__

//EOF
