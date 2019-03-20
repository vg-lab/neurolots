#include <iostream>

#include <nlrender/nlrender.h>
#include <nlgeometry/nlgeometry.h>
#include <reto/reto.h>

//OpenGL
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

void initContext( int argc, char* argv[ ]);


int main( int argc, char* argv[ ])
{
  std::string sceneFile( argv[1] );

  initContext( argc, argv );

  auto startTime = std::chrono::system_clock::now( );
  auto vdmCollec = nlgeometry::VDMapReader::readVDMapCollection( sceneFile );
  auto endTime = std::chrono::system_clock::now( );

  auto loadDuration =
    (float)std::chrono::duration_cast< std::chrono::milliseconds >
    ( endTime - startTime ).count( );

  if ( vdmCollec->macroMap( ))
    std::cout << "NumberOfMaps: " << vdmCollec->vdmapsNum( ) << std::endl;
  else
    std::cout << "NumberOfMaps: " << vdmCollec->vdmaps( ).size( ) << std::endl;
  std::cout << "MapSize: " << vdmCollec->vdmapsSize( ) << std::endl;
  std::cout << "\tMeshesLoadTime: " << loadDuration << " millisecond"
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
  glutCreateWindow( "Neurolots example: vdm render reader" );

  glewExperimental = GL_TRUE;
  glewInit( );

}
