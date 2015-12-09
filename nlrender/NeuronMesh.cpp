/**
 * @file    NeuronMesh.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>
 * @date
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "NeuronMesh.h"

using namespace std;

namespace nlrender
{

  NeuronMesh::NeuronMesh( const nsol::NeuronMorphologyPtr& morpho_ )
    : _morpho( morpho_ )
    , _isInit( false )
  {

  }

  NeuronMesh::~NeuronMesh( void )
  {
  }

  void NeuronMesh::Init( void )
  {
    if( !_isInit )
    {
      std::vector< float > vertices;
      std::vector< float > centers;
      std::vector< float > tangents;
      std::vector< unsigned int > mesh;

      nlgenerator::NeuronMeshGenerator::GenerateMesh( _morpho, vertices,
                                                      centers, tangents,
                                                      mesh, _somaEnd );
      // VAO Generation
      glGenVertexArrays( 1, &vao_ );
      glBindVertexArray( vao_ );

      _size = ( unsigned int ) mesh.size( );


      vbo_ = ( GLuint * )malloc( sizeof( GLuint ) * 4 );
      glGenBuffers(4,vbo_);

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[0]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertices.size( ),
                    vertices.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( 0 );

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 1 ]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * centers.size( ),
                    centers.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( 1 );

      glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 2 ]);
      glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * tangents.size( ),
                    tangents.data( ), GL_STATIC_DRAW );
      glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, 0 );
      glEnableVertexAttribArray( 2 );

      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo_[ 3 ]);
      glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( int ) * _size,
                    mesh.data( ), GL_STATIC_DRAW );


      glBindVertexArray( 0 );

      vertices.clear( );
      centers.clear( );
      tangents.clear( );
      mesh.clear( );
      _isInit= true;
    }
  }

  void NeuronMesh::Regenerate( const float& alphaRadius_,
                               const std::vector< float >& alphaNeurites_ )
  {
    std::vector< float > vertices;
    std::vector< float > centers;
    std::vector< float > tangents;
    std::vector< unsigned int > mesh;

    nlgenerator::NeuronMeshGenerator::GenerateMesh( _morpho, alphaRadius_,
                                                    alphaNeurites_,
                                                    vertices, centers,
                                                    tangents, mesh,
                                                    _somaEnd );
    // VAO Generation
    glBindVertexArray( vao_ );

    _size = ( unsigned int ) mesh.size( );

    glBindBuffer( GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertices.size( ),
                  vertices.data( ), GL_STATIC_DRAW );


    glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 1 ]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * centers.size( ),
                  centers.data( ), GL_STATIC_DRAW );


    glBindBuffer( GL_ARRAY_BUFFER, vbo_[ 2 ]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * tangents.size( ),
                  tangents.data( ), GL_STATIC_DRAW );


    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo_[ 3 ]);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( int ) * _size,
                  mesh.data( ), GL_STATIC_DRAW );


    glBindVertexArray( 0 );

    vertices.clear( );
    centers.clear( );
    tangents.clear( );
    mesh.clear( );
  }

  void NeuronMesh::PaintSoma( void ) const
  {
    glBindVertexArray(vao_);
    glPatchParameteri( GL_PATCH_VERTICES, 3 );
    glDrawElements( GL_PATCHES, _somaEnd, GL_UNSIGNED_INT, 0 );
  }

  void NeuronMesh::PaintNeurites( void ) const
  {
    glBindVertexArray(vao_);
    glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawElements( GL_PATCHES,  _size -_somaEnd, GL_UNSIGNED_INT,
                 (void *) (_somaEnd * sizeof(unsigned int)));
  }


  void NeuronMesh::WriteOBJ( const std::string& fileName_,
                             nlgeometry::Vertices& vertices_,
                             const nlgeometry::Facets& facets_ ) const
  {
    ofstream outStream(fileName_.c_str());
    if(!outStream.is_open())
    {
      std::cerr <<  fileName_ << ": Error creating the file" << std::endl;
      return;
    }

    for ( unsigned int i = 0; i < ( unsigned int ) vertices_.size( ); i++ )
    {
      vertices_[i]->id( ) = i + 1;
      outStream << "v " << vertices_[i]->position( ).x( ) << " "
                << vertices_[i]->position( ).y( ) << " "
                << vertices_[i]->position( ).z( ) << std::endl;
    }
    for( nlgeometry::VertexPtr vertex: vertices_ )
    {
      outStream << "vn " << vertex->normal( ).x( ) << " "
                << vertex->normal( ).y( ) << " "
                << vertex->normal( ).z( ) << std::endl;
    }
    for( nlgeometry::FacetPtr facet: facets_ )
    {
      outStream << "f " << facet->v0( )->id( ) << "//" <<  facet->v0( )->id( )
                << " " << facet->v1( )->id( ) << "//" <<  facet->v1( )->id( )
                << " " << facet->v2( )->id( ) << "//" <<  facet->v2( )->id( )
                << std::endl;
    }

    outStream.close();
    std::cout << "File " << fileName_ << " saved" << " with "
              << vertices_.size() << " vertices and " << facets_.size()
              << " facets." << std::endl;

  }

} // end namespace nlrender

// EOF
