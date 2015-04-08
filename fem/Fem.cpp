#include "Fem.h"

namespace fem
{

  Fem::Fem( std::vector< NodePtr > nodes_,
            std::vector< TetrahedronPtr > tetrahedra_,
            double v_, double E_ )
    : _nodes( nodes_ )
    , _tetrahedra( tetrahedra_ )
    , _v( v_ )
    , _E( E_ )
    , _dim( 0 )
  {
    _Material = Eigen::MatrixXf( 6, 6 );
    float a = _E * ( 1 - _v ) / (( 1 + _v ) * ( 1 - 2 * _v ));
    float b = _E * _v / (( 1 + _v ) * ( 1 - 2 * _v ));
    float c = _E / ( 2 * ( 1 + _v ));
    _Material <<  a,    b,    b,    0.0f, 0.0f, 0.0f,
                  b,    a,    b,    0.0f, 0.0f, 0.0f,
                  b,    b,    a,    0.0f, 0.0f, 0.0f,
                  0.0f, 0.0f, 0.0f, c,    0.0f, 0.0f,
                  0.0f, 0.0f, 0.0f, 0.0f, c,    0.0f,
                  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, c;
  }

  Fem::~Fem(void)
  {
  }

  // Public

  void Fem::Solve( void )
  {
    _ComputeTetrahedra( );
    _ConformMatrixSystem( );
     _u = _solver.solve( _b );

//     std::cout << "K:\n" << _kMatrix << std::endl;
//     std::cout << "b:\n" << _b << std::endl;
//     std::cout << "u:\n" << _u << std::endl;

     //COMPORBACION DE U

    for ( unsigned int i=0; i < _nodes.size(); i++)
    {
      if ( !_nodes[i]->Fixed( ))
      {
        unsigned int id = _ids[_nodes[i]->Id( )];
        _nodes[i]->U( Eigen::Vector3f( _u[id*3], _u[id*3+1], _u[id*3+2]) );
      }
    }


    for ( unsigned int i = 0; i < _nodes.size(); i++ )
    {
      _nodes[i]->Pos( _nodes[i]->Pos0( ) + _nodes[i]->U( ));
    }


  }

  // Private

  void Fem::_AddTokMatrix( unsigned int id0, unsigned int id1,
                          Eigen::Matrix3f sum )
  {
    unsigned int row = _ids[id0] * 3;
    unsigned int col = _ids[id1] * 3;
    unsigned int localCol;
    for ( unsigned int i = 0; i < 3; i++ )
    {
      localCol = col;
      for( unsigned int j = 0; j < 3; j++ )
      {
//        _kMatrix( row + i, localCol + j ) = _kMatrix( row + i, localCol + j )
//                                            + sum( i, j );
        _triplets.push_back( Eigen::Triplet< float >( row + i, localCol + j,
                             sum( i, j )));
      }
    }

  }

  void Fem::_AddToB( unsigned int id, Eigen::Vector3f sum )
  {
    unsigned int row = _ids[id] * 3;
    for( unsigned int i = 0; i < 3; i++ )
    {
      _b[row + i] += sum[i];
    }
  }

  void Fem::_ComputeTetrahedra( void )
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

      Eigen::Vector3f v0 = tet->Node1( )->Pos0( ) - tet->Node0( )->Pos0( );
      Eigen::Vector3f v1 = tet->Node2( )->Pos0( ) - tet->Node0( )->Pos0( );
      Eigen::Vector3f v2 = tet->Node3( )->Pos0( ) - tet->Node0( )->Pos0( );

      E << v0.x( ), v1.x( ), v2.x( ),
           v0.y( ), v1.y( ), v2.y( ),
           v0.z( ), v1.z( ), v2.z( );
//      std::cout << "Tetraedro " << i << " E\n " << E << std::endl;

      float volume = fabs( E.determinant( ) / 6.0f );
      tet->Volume( volume );
      InvE = E.inverse( );
//      std::cout << "tet " << i << " Volumen " << tet->Volume( ) << std::endl;
//      std::cout << "Tetraedro " << i << " inversa \n"<< InvE << std::endl;
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

      tet->B0( B0 );
      tet->B1( B1 );
      tet->B2( B2 );
      tet->B3( B3 );

//      std::cout << "tet " << i << std::endl;
//      std::cout << " B0\n " << tet->B0( ) << std::endl;
//      std::cout << " B1\n " << tet->B1( ) << std::endl;
//      std::cout << " B2\n " << tet->B2( ) << std::endl;
//      std::cout << " B3\n " << tet->B3( ) << std::endl;

    }
  }

  void Fem::_ConformMatrixSystem( void )
  {

    _triplets.clear( );

    _ids.resize( _nodes.size( ));
    unsigned int n = 0;
    for ( unsigned int i = 0; i < _nodes.size( ); i++ )
    {
      if( !_nodes[i]->Fixed( ))
      {
        _ids[i] = n;
        n ++;
      }
    }

    _dim = n * 3;
//    std::cout << "dimensiones de la matriz " << _dim << std::endl;

    _kMatrix.resize( _dim, _dim );
    _b = Eigen::VectorXf( _dim );
    _u = Eigen::VectorXf( _dim );

    _kMatrix.setZero( );
    _b.setZero( );
    _u.setZero( );

    for ( unsigned int i = 0; i < _tetrahedra.size( ); i++ )
    {
      TetrahedronPtr tet = _tetrahedra[i];


      Eigen::MatrixXf B0 = tet->B0( );
      Eigen::MatrixXf B1 = tet->B1( );
      Eigen::MatrixXf B2 = tet->B2( );
      Eigen::MatrixXf B3 = tet->B3( );

      Eigen::MatrixXf B0_Tr = B0.transpose( );
      Eigen::MatrixXf B1_Tr = B1.transpose( );
      Eigen::MatrixXf B2_Tr = B2.transpose( );
      Eigen::MatrixXf B3_Tr = B3.transpose( );

      float volume = tet->Volume( );

//      std::cout << "Volumen de tetraedro " << volume << std::endl;

      Eigen::Matrix3f K00 = B0_Tr * _Material * B0 * volume;
      Eigen::Matrix3f K01 = B0_Tr * _Material * B1 * volume;
      Eigen::Matrix3f K02 = B0_Tr * _Material * B2 * volume;
      Eigen::Matrix3f K03 = B0_Tr * _Material * B3 * volume;

      Eigen::Matrix3f K10 = B1_Tr * _Material * B0 * volume;
      Eigen::Matrix3f K11 = B1_Tr * _Material * B1 * volume;
      Eigen::Matrix3f K12 = B1_Tr * _Material * B2 * volume;
      Eigen::Matrix3f K13 = B1_Tr * _Material * B3 * volume;

      Eigen::Matrix3f K20 = B2_Tr * _Material * B0 * volume;
      Eigen::Matrix3f K21 = B2_Tr * _Material * B1 * volume;
      Eigen::Matrix3f K22 = B2_Tr * _Material * B2 * volume;
      Eigen::Matrix3f K23 = B2_Tr * _Material * B3 * volume;

      Eigen::Matrix3f K30 = B3_Tr * _Material * B0 * volume;
      Eigen::Matrix3f K31 = B3_Tr * _Material * B1 * volume;
      Eigen::Matrix3f K32 = B3_Tr * _Material * B2 * volume;
      Eigen::Matrix3f K33 = B3_Tr * _Material * B3 * volume;
//
//      std::cout << "tet " << i << std::endl;
//
//      std::cout << "B0\n" << B0 << std::endl;
//      std::cout << "B1\n" << B1 << std::endl;
//      std::cout << "B2\n" << B2 << std::endl;
//      std::cout << "B3\n" << B3 << std::endl;
//
//      std::cout << "B0_Tr\n" << B0_Tr << std::endl;
//      std::cout << "B1_Tr\n" << B1_Tr << std::endl;
//      std::cout << "B2_Tr\n" << B2_Tr << std::endl;
//      std::cout << "B3_Tr\n" << B3_Tr << std::endl;
//
//      std::cout << "K00\n" << K00 << std::endl;
//      std::cout << "K01\n" << K01 << std::endl;
//      std::cout << "K02\n" << K02 << std::endl;
//      std::cout << "K03\n" << K03 << std::endl;
//
//      std::cout << "K10\n" << K10 << std::endl;
//      std::cout << "K11\n" << K11 << std::endl;
//      std::cout << "K12\n" << K12 << std::endl;
//      std::cout << "K13\n" << K13 << std::endl;
//
//      std::cout << "K20\n" << K20 << std::endl;
//      std::cout << "K21\n" << K21 << std::endl;
//      std::cout << "K22\n" << K22 << std::endl;
//      std::cout << "K23\n" << K23 << std::endl;
//
//      std::cout << "K30\n" << K30 << std::endl;
//      std::cout << "K31\n" << K31 << std::endl;
//      std::cout << "K32\n" << K32 << std::endl;
//      std::cout << "K33\n" << K33 << std::endl;
//
//      std::cout << "u0 \n" << tet->Node0( )->U( ) << std::endl;
//      std::cout << "u1 \n" << tet->Node1( )->U( ) << std::endl;
//      std::cout << "u2 \n" << tet->Node2( )->U( ) << std::endl;
//      std::cout << "u3 \n" << tet->Node3( )->U( ) << std::endl;

      unsigned int id0 = tet->Node0( )->Id( );
      unsigned int id1 = tet->Node1( )->Id( );
      unsigned int id2 = tet->Node2( )->Id( );
      unsigned int id3 = tet->Node3( )->Id( );

      // Node 0
      if ( !tet->Node0( )->Fixed( ))
      {
        _AddTokMatrix( id0, id0, K00 );
        // with node 1
        if ( !tet->Node1( )->Fixed( ) )
        {
          _AddTokMatrix( id0, id1, K01 );
        }
        else
        {
          _AddToB( id0, -K01 * tet->Node1( )->U( ));
        }
        // with node 2
        if ( !tet->Node2( )->Fixed( ) )
        {
          _AddTokMatrix( id0, id2, K02 );
        }
        else
        {
          _AddToB( id0, -K02 * tet->Node2( )->U( ));
        }
        // with node 3
        if ( !tet->Node3( )->Fixed( ) )
        {
          _AddTokMatrix( id0, id3, K03 );
        }
        else
        {
          _AddToB( id0, -K03 * tet->Node3( )->U( ));
        }
      }

      // Node 1
      if ( !tet->Node1( )->Fixed( ))
      {
        _AddTokMatrix( id1, id1, K11 );
        // with node 0
        if ( !tet->Node0( )->Fixed( ) )
        {
          _AddTokMatrix( id1, id0, K10 );
        }
        else
        {
          _AddToB( id1, -K10 * tet->Node0( )->U( ));
        }
        // with node 2
        if ( !tet->Node2( )->Fixed( ) )
        {
          _AddTokMatrix( id1, id2, K12 );
        }
        else
        {
          _AddToB( id1, -K12 * tet->Node2( )->U( ));
        }
        // with node 3
        if ( !tet->Node3( )->Fixed( ) )
        {
          _AddTokMatrix( id1, id3, K13 );
        }
        else
        {
          _AddToB( id1, -K13 * tet->Node3( )->U( ));
        }
      }

      // Node 2
      if ( !tet->Node2( )->Fixed( ))
      {
        _AddTokMatrix( id2, id2, K22 );

        // with node 0
        if ( !tet->Node0( )->Fixed( ) )
        {
          _AddTokMatrix( id2, id0, K20 );
        }
        else
        {
          _AddToB( id2, -K20 * tet->Node0( )->U( ));
        }

        // with node 1
        if ( !tet->Node1( )->Fixed( ) )
        {
          _AddTokMatrix( id2, id1, K21 );
        }
        else
        {
          _AddToB( id2, -K21 * tet->Node1( )->U( ));
        }

        // with node 3
        if ( !tet->Node3( )->Fixed( ) )
        {
          _AddTokMatrix( id2, id3, K23 );
        }
        else
        {
          _AddToB( id2, -K23 * tet->Node3( )->U( ));
        }
      }

      // Node 3
      if ( !tet->Node3( )->Fixed( ))
      {
        _AddTokMatrix( id3, id3, K33 );
        // with node 0
        if ( !tet->Node0( )->Fixed( ) )
        {
          _AddTokMatrix( id3, id0, K30 );
        }
        else
        {
          _AddToB( id3, -K30 * tet->Node0( )->U( ));
        }
        // with node 1
        if ( !tet->Node1( )->Fixed( ) )
        {
          _AddTokMatrix( id3, id1, K31 );
        }
        else
        {
          _AddToB( id3, -K31 * tet->Node1( )->U( ));
        }
        // with node 2
        if ( !tet->Node2( )->Fixed( ) )
        {
          _AddTokMatrix( id3, id2, K32 );
        }
        else
        {
          _AddToB( id3, -K32 * tet->Node2( )->U( ));
        }

      }

    }

    _kMatrix.resizeNonZeros( _triplets.size( ));
    _kMatrix.setFromTriplets( _triplets.begin( ), _triplets.end( ));

    _triplets.clear( );

    _solver.compute( _kMatrix );



  }


} // end namespace fem
