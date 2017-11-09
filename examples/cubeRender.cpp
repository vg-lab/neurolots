#include <iostream>

#include <nlgeometry/nlgeometry.h>
#include <nlrender/nlrender.h>
#include <reto/reto.h>

//OpenGL
#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
#include <GL/glew.h>
#endif
#ifdef Darwin
#include <gl.h>
#include <glu.h>
#include <GLUT/glut.h>
#include <GL/freeglut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include "Cube.h"
#include "Shaders.h"

reto::Camera* camera;
reto::ShaderProgram* program;
nlgeometry::Meshes meshes;

// X Y mouse position.
int previousX;
int previousY;

// States.
bool wireframe = false;
bool mouseDown = false;
bool mouseScrolling = false;
bool rotation = false;
bool traslation = false;

// Constants.

const float mouseWheelFactor = 1.2f;
const float rotationScale = 0.01f;
const float traslationScale = 0.2f;


void idleFunc( void );
void renderFunc( void );
void keyboardFunc( unsigned char key, int x, int y );
void mouseFunc( int button, int state, int x, int y );
void mouseMotionFunc( int x, int y );
void resizeFunc( int width, int height );

void initContext( int argc, char* argv[ ]);
void initOGL( void );

int main( int argc, char* argv[] )
{
  std::cout << "nlgeometry example: Cube Render" << std::endl;

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( );

  nlgeometry::AxisAlignedBoundingBox aabb;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 2 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::NORMAL;

  mesh = new Cube( );
  std::cout << "Loaded cube with: "
                << mesh->vertices( ).size( ) << " vertices, "
                << mesh->triangles( ).size( ) << " triangles and "
                << mesh->quads( ).size( ) << " quads" << std::endl;
  mesh->computeNormals( );
  mesh->uploadGPU( format );
  mesh->computeBoundingBox( );
  mesh->clearCPUData( );

  aabb = mesh->aaBoundingBox( );

  meshes.push_back( mesh );

  camera->pivot( aabb.center( ));
  camera->radius( aabb.radius( ) / sin( camera->fov( )));

  // camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
  // camera->radius( 10.0f );

  glutMainLoop( );
  return 0;
}

void initContext( int argc, char* argv[ ])
{
  glutInit( &argc, argv );
  glutInitContextVersion( 4, 0 );

  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize( 600, 600 );
  glutInitWindowPosition( 0, 0 );
  glutCreateWindow( " Neurolots example: Obj Render" );

  nlrender::Config::init( );

  glutDisplayFunc( renderFunc );
  glutIdleFunc( idleFunc );
  glutKeyboardFunc( keyboardFunc );
  glutMouseFunc( mouseFunc );
  glutMotionFunc( mouseMotionFunc );
  glutReshapeFunc( resizeFunc );
}

void initOGL( void )
{
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

  program = new reto::ShaderProgram( );
  program->loadVertexShaderFromText( examples::example_vert );
  program->loadFragmentShaderFromText( examples::example_frag );
  program->compileAndLink( );
  program->autocatching( );

  glEnable( GL_DEPTH_TEST );
  // glEnable( GL_CULL_FACE );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void idleFunc( void )
{
  glutPostRedisplay( );
}

void renderFunc( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  program->use( );
  program->sendUniform4m("proj", camera->projectionMatrix( ));
  program->sendUniform4m("view", camera->viewMatrix( ));

  for ( auto mesh: meshes )
  {
    program->sendUniform4m("model", mesh->modelMatrixVectorized( ));
    mesh->renderTriangles( );
    mesh->renderQuads( );
  }

  glFlush( );
  glutSwapBuffers( );
}

void keyboardFunc( unsigned char key, int, int )
{
  switch( key )
  {
    // Camera control.
    case 'c':
    case 'C':
      camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
      camera->radius( 1000.0f );
      camera->rotation( 0.0f, 0.0f );
      std::cout << "Centering." << std::endl;
      glutPostRedisplay( );
      break;
    case 'm':
    case 'M':
      wireframe = !wireframe;
      if ( wireframe )
      {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        std::cout << "Wireframe ON." << std::endl;
      }
      else
      {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        std::cout << "Wireframe OFF." << std::endl;
      }
      glutPostRedisplay( );
      break;
  }
}

void mouseFunc( int button, int state, int x, int y )
{
  /**
   * GLUT
   * button: 0 (left), 1 (central), 2 (right), 3 (wheel up), 4 (wheel down).
   * state: GLUT_DOWN (button down), GLUT_UP (button released).
   */
  if ( state == GLUT_DOWN )
  {
    mouseDown = true;
    if( button == 0 ) rotation = true;
    if( button == 1 ) traslation = true;
    if ( (button == 3) || (button == 4) )
    {
      //std::cout << "Scrolling." << std::endl;
      mouseScrolling = true;
      float newRadius = ( button == 3 ) ?
                        camera->radius() / mouseWheelFactor :
                        camera->radius() * mouseWheelFactor;
      camera->radius( newRadius );
      glutPostRedisplay();
    }
    // We save X and Y previous positions.
    previousX = x;
    previousY = y;
  }
  else
  {
    mouseDown = false;
    if( button == 0 ) rotation = false;
    if( button == 1 ) traslation = false;
    if ( (button == 3) || (button == 4) )
    {
      mouseScrolling = false;
    }
  }
}

void mouseMotionFunc( int x, int y )
{
  if( mouseDown )
  {
    float deltaX = x - previousX;
    float deltaY = y - previousY;
    if( rotation )
    {
      camera->localRotation( deltaX * rotationScale,
                             deltaY * rotationScale );
    }
    if( traslation )
    {
      camera->localTranslation( Eigen::Vector3f ( -deltaX * traslationScale,
                                                  deltaY * traslationScale,
                                                  0.0f ) );
    }
    previousX = x;
    previousY = y;
    glutPostRedisplay();
  }
}

void resizeFunc( int width, int height )
{
  camera->ratio((( double ) width ) / height );
  glViewport( 0, 0, width, height );
}
