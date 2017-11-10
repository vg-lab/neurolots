/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of nsol <https://github.com/gmrvvis/neurolots>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef __NLPHYSICS_FEM__
#define __NLPHYSICS_FEM__

#include <Eigen/Sparse>
#include <cstdio>

#include "Node.h"
#include "Tetrahedron.h"

#include <nlphysics/api.h>

namespace nlphysics
{

  /* \class Fem */
  class Fem
  {

  public:

    /**
     * Constructor
     * @param nodes_ geometry nodes of the FEM system
     * @param tetrahedra_ geometry tetrahedra of the FEM system
     * @param v_ Cauchy coefficient
     * @param E_ Young modulus
     */
    NLPHYSICS_API
    Fem( Nodes& nodes_, Tetrahedra& tetrahedra_,
         float v_ = 0.3f, float E_ = 1.0f );

    /*
     * Default destructor
     */
    NLPHYSICS_API
    ~Fem( void );

    /*
     * Method that solve the system
     */
    NLPHYSICS_API
    void solve( void );

  private:

    void _addTokMatrix( unsigned int id0, unsigned int id1,
                        Eigen::Matrix3f sum );

    void _addToB( unsigned int id, Eigen::Vector3f sum);

    void _computeTetrahedra( void );

    void _conformMatrixSystem( void );


    //! Geometry nodes
    std::vector< NodePtr > _nodes;
    //! Geometry tetrahedra
    Tetrahedra _tetrahedra;

    //! Vector of indices
    std::vector< unsigned int > _indices;

    //! Cauchy coefficient
    float _v;

    //! Young modulus
    float _E;

    //! Material matrix
    Eigen::MatrixXf _material;

    //! Vector of triplets that forms the system
    std::vector< Eigen::Triplet< float >> _triplets;

    //! System stiffness matrix
    Eigen::SparseMatrix< float > _kMatrix;

    //! System solutions matrix
    Eigen::VectorXf _b;

    //! System rigth vector
    Eigen::VectorXf _u;

    //! Conjugate Gradient solver
    Eigen::ConjugateGradient< Eigen::SparseMatrix< float >> _solver;

    //! System size
    unsigned int _size;

  }; // class Fem

} // namespace nlphysics

#endif
