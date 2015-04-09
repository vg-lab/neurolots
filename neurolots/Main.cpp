//OpenGL
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

//Eigen
#include "Eigen/Dense"

//Neurolots
#include "Camera.h"
#include "Program.h"
#include "NeuronMesh.h"
#include "NeuronsCollection.h"

#include <iostream>

using Eigen::Vector3d;
using namespace std;
using namespace neurolots;


bool rotation = false;
bool translation = false;

bool paintSoma = true;
bool paintNeurites = true;

int m_x, m_y;

bool mode = true;

int cont = 0;

Camera * camera;
NeuronsCollection * neuronsCollection;

void sceneInit( void )
{
  glEnable( GL_DEPTH_TEST );
  glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
  glPolygonMode( GL_FRONT_AND_BACK , GL_FILL );
  glEnable( GL_CULL_FACE );

  glLineWidth( 1.5 );

}

void paintFunc(void)
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );



//  neuronsCollection->Paint( );
  neuronsCollection->PaintMiniColum( 0, 0 );
//  neuronsCollection->PaintMiniColum( 0, 1 );
//  for ( int i = 0; i < 10; i++ )
//  {
//    for ( int j = 0; j < 10; j++ )
//    {
//      for ( int k = 0; k < 10; k++ )
//      {
//        neuronsCollection->PaintNeuron(0,0,49, i*100, j*100, k*100);
//      }
//    }
//  }

  //neuronsCollection->PaintNeuron(0,0,0);

//  cont ++;
//
//  if( cont >= 10  )
//    cont = 0;
//
//
//  neuronsCollection->PaintMiniColum( 0,cont );
  //neuronsCollection->Anim( );

  glUseProgram( 0 );
  glFlush( );
  glutSwapBuffers( );
}

void keyboardFunc( unsigned char key, int _x, int _y )
{
  switch( key )
  {
    case 'm':
      mode = !mode;
      if ( mode )
      {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glDisable( GL_POLYGON_OFFSET_LINE );
      }
      else
      {
        glEnable( GL_POLYGON_OFFSET_LINE );
        glPolygonOffset( -1, -1 );
        glLineWidth( 1.5 );
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }
      break;
    case 'c':
      camera->UpdatePosition( 0.0f, 0.0f, 20.0f );
      camera->UpdateRotation( 0.0f, 0.0f, 0.0f );
      break;
    case 'z':
      camera->IncrementRotation( 0.05f );
      break;
    case 'x':
      camera->IncrementRotation( -0.05f );
      break;
    case 'w':
      neuronsCollection->AddLod( 1.0f );
      break;
    case 's':
      neuronsCollection->AddLod( -1.0f );
      break;
    case 'e':
      neuronsCollection->AddTng( 0.1f );
      break;
    case 'd':
      neuronsCollection->AddTng( -0.1f );
      break;
    case 'r':
      neuronsCollection->AddMaxDist( 1 );
      break;
    case 'f':
      neuronsCollection->AddMaxDist( -1 );
      break;
    case 'o':
      paintSoma = !paintSoma;
      neuronsCollection->PaintSoma( paintSoma );
      break;
    case 'p':
      paintNeurites = !paintNeurites;
      neuronsCollection->PaintNeurites( paintNeurites );
      break;
  }
}

void mouseFunc( int boton, int state, int _x, int _y )
{
  if ( boton == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
  {
    rotation = true;
    m_x = _x;
    m_y = _y;
  }
  if ( boton == GLUT_LEFT_BUTTON && state == GLUT_UP )
  {
    rotation=false;
  }
  if ( boton == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN )
  {
    translation = true;
    m_x = _x;
    m_y = _y;
  }
  if ( boton == GLUT_MIDDLE_BUTTON && state == GLUT_UP )
  {
    translation = false;
  }
  if ( boton == 3 && state == GLUT_DOWN )
  {
    camera->LocalDisplace( 0.0f, 0.0f, -10.0f );
  }
  if ( boton == 4 && state == GLUT_DOWN )
  {
    camera->LocalDisplace( 0.0f, 0.0f, 10.0f );
  }
}

void mouseMoveFunc( int _x, int _y )
{
  if( rotation )
  {
    camera->IncrementRotation( -( m_x - _x ) * 0.01, -( m_y - _y ) * 0.01 );
    m_x = _x;
    m_y = _y;
  }
  if( translation )
  {
    camera->LocalDisplace(( m_x - _x ) * 0.1, ( _y - m_y ) * 0.1, 0.0f );
    m_x = _x;
    m_y = _y;
  }
}

void idleFunc( )
{
  glutPostRedisplay( );
}

void resizeFunc( int w, int h )
{
  camera->UpdateRatio((( double ) w ) / h );
  glViewport( 0, 0, w, h );
}

int main( int argc, char * argv[ ])
{
  camera = new Camera( );




  glutInit( &argc, argv );
  glutInitContextVersion( 4, 4 );

  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GL_DEPTH_BUFFER );
  glutInitWindowPosition( 0, 0 );
  glutInitWindowSize( 600, 600 );
  glutCreateWindow( "NeuroLOT" );

  glutDisplayFunc( paintFunc );
  glutKeyboardFunc( keyboardFunc );
  glutMouseFunc( mouseFunc );
  glutMotionFunc( mouseMoveFunc );
  glutIdleFunc( idleFunc );
  glutReshapeFunc( resizeFunc );

  glewExperimental = GL_TRUE;
/*  GLenum err =*/glewInit( );

  if( argc == 2 )
  {
    neuronsCollection = new NeuronsCollection( argv[1],
                                             "/home/jjgarcia/shaders/quads",
                                             "/home/jjgarcia/shaders/triangles",
                                             camera );
    neuronsCollection->NeuritesColor( Eigen::Vector3f( 0.3, 0.5, 0.7 ));
    neuronsCollection->SomaColor( Eigen::Vector3f( 0.7, 0.5, 0.3 ));
  }
  else
  {
    std::cout << "Usage Error" << std::endl;
    exit( 0 );
  }

/*
  if( GLEW_OK != err )
  {
    std::cout << "Error: " << glewGetErrorString( err ) << std::endl;
  }
  const GLubyte * oglVersion = glGetString( GL_VERSION );
  std::cout << "Este sistema soporta la Versión " << oglVersion << " de OpenGL."
            << std::endl;
*/

  sceneInit( );

  glutMainLoop( );

  return 0;
}
