/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/gmrvvis/neurolots>
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
#include "Fem.h"

namespace nlphysics
{

  Fem::Fem( Nodes& nodes_, Tetrahedra& tetrahedra_,
            float v_, float E_ )
    : _nodes( nodes_ )
    , _tetrahedra( tetrahedra_ )
    , _v( v_ )
    , _E( E_ )
    , _size( 0 )
  {
    _material = Eigen::MatrixXf( 6, 6 );
    float a = _E * ( 1 - _v ) / (( 1 + _v ) * ( 1 - 2 * _v ));
    float b = _E * _v / (( 1 + _v ) * ( 1 - 2 * _v ));
    float c = _E / ( 2 * ( 1 + _v ));
    _material <<  a,    b,    b,    0.0f, 0.0f, 0.0f,
      b,    a,    b,    0.0f, 0.0f, 0.0f,
      b,    b,    a,    0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, c,    0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f, c,    0.0f,
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, c;
  }

  Fem::~Fem(void)
  {
  }

  void Fem::solve( void )
  {
    _computeTetrahedra( );
    for ( unsigned int i=0; i < _nodes.size( ); i++ )
    {
      if ( _nodes[i]->fixed( ))
        _nodes[i]->displacement( ) = _nodes[i]->position( ) -
          _nodes[i]->initialPosition( );
    }
    _conformMatrixSystem( );
    _u = _solver.solve( _b );

    for ( unsigned int i=0; i < _nodes.size(); i++)
    {
      if ( !_nodes[i]->fixed( ))
      {
        unsigned int id = _indices[_nodes[i]->id( )];
        _nodes[i]->displacement( ) =
          Eigen::Vector3f( _u[id*3], _u[id*3+1], _u[id*3+2]);
      }
    }


    for ( unsigned int i = 0; i < _nodes.size(); i++ )
    {
      _nodes[i]->position( ) = _nodes[i]->initialPosition( ) +
        _nodes[i]->displacement( );
    }
  }

  void Fem::_addTokMatrix( unsigned int id0, unsigned int id1,
                          Eigen::Matrix3f sum )
  {
    unsigned int row = _indices[id0] * 3;
    unsigned int col = _indices[id1] * 3;
    unsigned int localCol;
    for ( unsigned int i = 0; i < 3; i++ )
    {
      localCol = col;
      for( unsigned int j = 0; j < 3; j++ )
      {
        _triplets.push_back( Eigen::Triplet< float >( row + i, localCol + j,
                                                      sum( i, j )));
      }
    }

  }

  void Fem::_addToB( unsigned int id, Eigen::Vector3f sum )
  {
    unsigned int row = _indices[id] * 3;
    for( unsigned int i = 0; i < 3; i++ )
    {
      _b[row + i] += sum[i];
    }
  }

  void Fem::_computeTetrahedra( void )
  {
    for( unsigned int i = 0; i < _tetrahedra.size( ); i++ )
    {
      TetrahedronPtr tet = _tetrahedra[i];
      Eigen::Matrix3f E;
      Eigen::Matrix3f InvE;

      Eigen::MatrixXf B0( 6, 3 );
      Eigen::MatrixXf B1( 6, 3 );
      Eigen::MatrixXf B2( 6, 3 );
      Eigen::MatrixXf B3( 6, 3 );

      Eigen::Vector3f v0 = tet->node1( )->initialPosition( ) -
        tet->node0( )->initialPosition( );
      Eigen::Vector3f v1 = tet->node2( )->initialPosition( ) -
        tet->node0( )->initialPosition( );
      Eigen::Vector3f v2 = tet->node3( )->initialPosition( ) -
        tet->node0( )->initialPosition( );

      E << v0.x( ), v1.x( ), v2.x( ),
           v0.y( ), v1.y( ), v2.y( ),
           v0.z( ), v1.z( ), v2.z( );

      float volume = fabs( E.determinant( ) / 6.0f );
      tet->volume( ) = volume;
      InvE = E.inverse( );

      B1 << InvE( 0, 0 ), 0.0f,         0.0f,
            0.0f,         InvE( 0, 1 ), 0.0f,
            0.0f,         0.0f,         InvE( 0, 2 ),
            InvE( 0, 1 ), InvE( 0, 0 ), 0.0f,
            0.0f,         InvE( 0, 2 ), InvE( 0, 1 ),
            InvE( 0, 2 ), 0.0f,         InvE( 0, 0 );

      B2 << InvE( 1, 0 ), 0.0f,         0.0f,
            0.0f,         InvE( 1, 1 ), 0.0f,
            0.0f,         0.0f,         InvE( 1, 2 ),
            InvE( 1, 1 ), InvE( 1, 0 ), 0.0f,
            0.0f,         InvE( 1, 2 ), InvE( 1, 1 ),
            InvE( 1, 2 ), 0.0f,         InvE( 1, 0 );

      B3 << InvE( 2, 0 ), 0.0f,         0.0f,
            0.0f,         InvE( 2, 1 ), 0.0f,
            0.0f,         0.0f,         InvE( 2, 2 ),
            InvE( 2, 1 ), InvE( 2, 0 ), 0.0f,
            0.0f,         InvE( 2, 2 ), InvE( 2, 1 ),
            InvE( 2, 2 ), 0.0f,         InvE( 2, 0 );

      float bn = -InvE( 0, 0 ) - InvE( 1, 0 ) - InvE( 2, 0 );
      float cn = -InvE( 0, 1 ) - InvE( 1, 1 ) - InvE( 2, 1 );
      float dn = -InvE( 0, 2 ) - InvE( 1, 2 ) - InvE( 2, 2 );

      B0 << bn,   0.0f, 0.0f,
            0.0f, cn,   0.0f,
            0.0f, 0.0f, dn,
            cn,   bn,   0.0f,
            0.0f, dn,   cn,
            dn,   0.0f, bn;

      tet->b0( ) = B0;
      tet->b1( ) = B1;
      tet->b2( ) = B2;
      tet->b3( ) = B3;
    }
  }

  void Fem::_conformMatrixSystem( void )
  {

    _triplets.clear( );

    _indices.resize( _nodes.size( ));
    unsigned int n = 0;
    for ( unsigned int i = 0; i < _nodes.size( ); i++ )
    {
      if( !_nodes[i]->fixed( ))
      {
        _indices[i] = n;
        n ++;
      }
    }

    _size = n * 3;

    _kMatrix.resize( _size, _size );
    _b = Eigen::VectorXf( _size );
    _u = Eigen::VectorXf( _size );

    _kMatrix.setZero( );
    _b.setZero( );
    _u.setZero( );

    for ( unsigned int i = 0; i < _tetrahedra.size( ); i++ )
    {
      TetrahedronPtr tet = _tetrahedra[i];


      Eigen::MatrixXf B0 = tet->b0( );
      Eigen::MatrixXf B1 = tet->b1( );
      Eigen::MatrixXf B2 = tet->b2( );
      Eigen::MatrixXf B3 = tet->b3( );

      Eigen::MatrixXf B0_Tr = B0.transpose( );
      Eigen::MatrixXf B1_Tr = B1.transpose( );
      Eigen::MatrixXf B2_Tr = B2.transpose( );
      Eigen::MatrixXf B3_Tr = B3.transpose( );

      float volume = tet->volume( );

      Eigen::Matrix3f K00 = B0_Tr * _material * B0 * volume;
      Eigen::Matrix3f K01 = B0_Tr * _material * B1 * volume;
      Eigen::Matrix3f K02 = B0_Tr * _material * B2 * volume;
      Eigen::Matrix3f K03 = B0_Tr * _material * B3 * volume;

      Eigen::Matrix3f K10 = B1_Tr * _material * B0 * volume;
      Eigen::Matrix3f K11 = B1_Tr * _material * B1 * volume;
      Eigen::Matrix3f K12 = B1_Tr * _material * B2 * volume;
      Eigen::Matrix3f K13 = B1_Tr * _material * B3 * volume;

      Eigen::Matrix3f K20 = B2_Tr * _material * B0 * volume;
      Eigen::Matrix3f K21 = B2_Tr * _material * B1 * volume;
      Eigen::Matrix3f K22 = B2_Tr * _material * B2 * volume;
      Eigen::Matrix3f K23 = B2_Tr * _material * B3 * volume;

      Eigen::Matrix3f K30 = B3_Tr * _material * B0 * volume;
      Eigen::Matrix3f K31 = B3_Tr * _material * B1 * volume;
      Eigen::Matrix3f K32 = B3_Tr * _material * B2 * volume;
      Eigen::Matrix3f K33 = B3_Tr * _material * B3 * volume;

      unsigned int id0 = tet->node0( )->id( );
      unsigned int id1 = tet->node1( )->id( );
      unsigned int id2 = tet->node2( )->id( );
      unsigned int id3 = tet->node3( )->id( );

      // Node 0
      if ( !tet->node0( )->fixed( ))
      {
        _addTokMatrix( id0, id0, K00 );
        // with node 1
        if ( !tet->node1( )->fixed( ) )
        {
          _addTokMatrix( id0, id1, K01 );
        }
        else
        {
          _addToB( id0, -K01 * tet->node1( )->displacement( ));
        }
        // with node 2
        if ( !tet->node2( )->fixed( ) )
        {
          _addTokMatrix( id0, id2, K02 );
        }
        else
        {
          _addToB( id0, -K02 * tet->node2( )->displacement( ));
        }
        // with node 3
        if ( !tet->node3( )->fixed( ) )
        {
          _addTokMatrix( id0, id3, K03 );
        }
        else
        {
          _addToB( id0, -K03 * tet->node3( )->displacement( ));
        }
      }

      // Node 1
      if ( !tet->node1( )->fixed( ))
      {
        _addTokMatrix( id1, id1, K11 );
        // with node 0
        if ( !tet->node0( )->fixed( ) )
        {
          _addTokMatrix( id1, id0, K10 );
        }
        else
        {
          _addToB( id1, -K10 * tet->node0( )->displacement( ));
        }
        // with node 2
        if ( !tet->node2( )->fixed( ) )
        {
          _addTokMatrix( id1, id2, K12 );
        }
        else
        {
          _addToB( id1, -K12 * tet->node2( )->displacement( ));
        }
        // with node 3
        if ( !tet->node3( )->fixed( ) )
        {
          _addTokMatrix( id1, id3, K13 );
        }
        else
        {
          _addToB( id1, -K13 * tet->node3( )->displacement( ));
        }
      }

      // node 2
      if ( !tet->node2( )->fixed( ))
      {
        _addTokMatrix( id2, id2, K22 );

        // with node 0
        if ( !tet->node0( )->fixed( ) )
        {
          _addTokMatrix( id2, id0, K20 );
        }
        else
        {
          _addToB( id2, -K20 * tet->node0( )->displacement( ));
        }

        // with node 1
        if ( !tet->node1( )->fixed( ) )
        {
          _addTokMatrix( id2, id1, K21 );
        }
        else
        {
          _addToB( id2, -K21 * tet->node1( )->displacement( ));
        }

        // with node 3
        if ( !tet->node3( )->fixed( ) )
        {
          _addTokMatrix( id2, id3, K23 );
        }
        else
        {
          _addToB( id2, -K23 * tet->node3( )->displacement( ));
        }
      }

      // node 3
      if ( !tet->node3( )->fixed( ))
      {
        _addTokMatrix( id3, id3, K33 );
        // with node 0
        if ( !tet->node0( )->fixed( ) )
        {
          _addTokMatrix( id3, id0, K30 );
        }
        else
        {
          _addToB( id3, -K30 * tet->node0( )->displacement( ));
        }
        // with node 1
        if ( !tet->node1( )->fixed( ) )
        {
          _addTokMatrix( id3, id1, K31 );
        }
        else
        {
          _addToB( id3, -K31 * tet->node1( )->displacement( ));
        }
        // with node 2
        if ( !tet->node2( )->fixed( ) )
        {
          _addTokMatrix( id3, id2, K32 );
        }
        else
        {
          _addToB( id3, -K32 * tet->node2( )->displacement( ));
        }

      }

    }

    _kMatrix.resizeNonZeros( int( _triplets.size( )));
    _kMatrix.setFromTriplets( _triplets.begin( ), _triplets.end( ));

    _triplets.clear( );

    _solver.compute( _kMatrix );
  }


} // namespace nlphysics
