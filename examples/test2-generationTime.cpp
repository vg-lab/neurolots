#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>

#include <nlrender/nlrender.h>
#include <nlgeometry/nlgeometry.h>
#include <reto/reto.h>

#include <chrono>

#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
  #include <GL/glew.h>
#endif
#ifdef Darwin
#define __gl_h_
  #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GL/freeglut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/freeglut.h>
#endif

std::vector< nlgeometry::VDMapPtr > vdmaps;
unsigned int textureSize;

void initContext( int argc, char* argv[ ]);
void initOGL( void );

int main( int argc, char* argv[ ])
{

  textureSize = 65;

  for ( int i = 1; i < argc; i++ )
  {
    std::string option( argv[i] );
    try
    {
      if ( option.compare( "-size") == 0 )
      {
        i++;
        textureSize = atof( argv[i] );
      }
    }
    catch( ... )
    {
      return 1;
    }
  }

  initContext( argc, argv );
  initOGL( );

  auto paraMethod0 = nlgeometry::Parametrizer::MEAN_VALUE;
  auto paraMethod1 = nlgeometry::Parametrizer::UNDEFINED;
  nlgenerator::VDMGenerator::Instance( )->vdmapSize( textureSize );
  nlgeometry::VDMapCollectionPtr vdmCollec = new nlgeometry::VDMapCollection( );

  auto startTime = std::chrono::system_clock::now( );

  nlgeometry::Meshes meshes;
  for ( int i = 1; i < argc; i++ )
  {
    try
    {
      auto inFile = std::string( argv[i] );
      nlgeometry::ObjReader objr;
      nlgeometry::MeshPtr mesh = objr.readMesh( inFile, false );
      if ( mesh->triangles( ).size( ) > 0 )
        meshes.push_back( mesh );
    }
    catch( ... )
    {
    }
  }
  auto loadTime = std::chrono::system_clock::now( );

  auto loadDuration =
    (float)std::chrono::duration_cast< std::chrono::milliseconds >
    ( loadTime - startTime ).count( );
  std::cout << "Textures size: " << textureSize << std::endl;
  std::cout << "\tMapsNumber: " << meshes.size( ) << std::endl;
  std::cout << "\tLoadTime: " << loadDuration << " millisecond " << std::endl;

  for ( auto mesh: meshes )
  {
    try
    {
      nlgeometry::VDMapPtr vdmap =
        nlgenerator::VDMGenerator::Instance( )->vectorDisplacementMapTexture(
          mesh, paraMethod0, paraMethod1, 1.0f, 0.5f, 3.0f );

      vdmCollec->addVDMap( vdmap, mesh->modelMatrix( ) );
      // delete mesh;
    }
    catch( ... )
    {
    }
  }
  auto mapsTime = std::chrono::system_clock::now( );

  vdmCollec->computeMacroMap( );

  auto macroTime = std::chrono::system_clock::now( );

  auto mapsDuration =
    (float)std::chrono::duration_cast< std::chrono::milliseconds >
    ( mapsTime - loadTime ).count( );
  auto macroDuration =
    (float)std::chrono::duration_cast< std::chrono::milliseconds >
    ( macroTime - mapsTime ).count( );

  std::cout << "\tMapsGenerationTime: " << mapsDuration << " millisecond"
            << std::endl;
  std::cout << "\tGenerationTimePerMap: "
            << mapsDuration / meshes.size( ) << " milliseconds" << std::endl;
  std::cout << "\tMacroMapGenerationTime: " << macroDuration << " milliseconds"
            << std::endl;
  return 0;
}

void initContext( int argc, char* argv[ ])
{
  glutInit( &argc, argv );
  glutInitContextVersion( 4, 0 );

  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize( 600, 600 );
  glutInitWindowPosition( 0, 0 );
  glutCreateWindow( "test2" );

  glewExperimental = GL_TRUE;
  glewInit( );
}

void initOGL( void )
{
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );


  glEnable( GL_DEPTH_TEST );
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

}
