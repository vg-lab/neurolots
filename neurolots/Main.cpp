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

#include <iostream>

using Eigen::Vector3d;
using namespace std;
using namespace neurolots;


bool rotation = false;
bool translation = false;

int m_x, m_y;

bool mode = true;

Camera * camera;
Program * program;
NeuronMesh * mesh;

void sceneInit( void )
{
  glEnable( GL_DEPTH_TEST );
  glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
  glPolygonMode( GL_FRONT_AND_BACK , GL_FILL );
  glEnable( GL_CULL_FACE );

  mesh->Init();
}

void paintFunc(void)
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  mesh->Paint( );

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
      program->lod( program->lod( ) + 1.0f );
      std::cout << "Lod: " << program->lod() << std::endl;
      break;
    case 's':
      program->lod( program->lod( ) - 1.0f );
      if ( program->lod( ) < 1.0f )
        program->lod( 1.0f );
      std::cout << "Lod: " << program->lod() << std::endl;
      break;
    case 'e':
      program->tng( program->tng( ) + 0.1f );
      std::cout << "Tng: " << program->tng() << std::endl;
      break;
    case 'd':
      program->tng( program->tng( ) - 0.1f );
      if ( program->tng( ) < 0.0f )
        program->tng( 0.0f );
      std::cout << "Tng: " << program->tng() << std::endl;
      break;
    case 'r':
      program->maxDist( program->maxDist( ) + 1 );
      std::cout << "Max distance: " << program->maxDist() << std::endl;
      break;
    case 'f':
      program->maxDist( program->maxDist( ) - 1 );
      if( program->maxDist( ) < 2 )
        program->maxDist( 2 );
      std::cout << "Max distance: " << program->maxDist() << std::endl;
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
    camera->LocalDisplace( 0.0f, 0.0f, -2.0f );
  }
  if ( boton == 4 && state == GLUT_DOWN )
  {
    camera->LocalDisplace( 0.0f, 0.0f, 2.0f );
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
  program = new Program( );

  if( argc == 2 )
  {
    string file_name( argv[ 1 ]);
    mesh = new NeuronMesh( file_name, program, camera, Vector3d(0,0,0),
                           Vector3d( 0.2, 0.5, 0.7 ));
  }
  else
  {
    std::cout << "Usage Error" << std::endl;
    exit( 0 );
  }
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
  GLenum err = glewInit( );
  if( GLEW_OK != err )
  {
    std::cout << "Error: " << glewGetErrorString( err ) << std::endl;
  }
  const GLubyte * oglVersion = glGetString( GL_VERSION );
  std::cout << "Este sistema soporta la Versión " << oglVersion << " de OpenGL."
            << std::endl;

  program->Init( );
  sceneInit( );

  glutMainLoop( );

  return 0;
}