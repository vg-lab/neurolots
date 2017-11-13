#include <iostream>

#include <nlgeometry/nlgeometry.h>
#include <nlgenerator/nlgenerator.h>
#include <nlrender/nlrender.h>
#include <reto/reto.h>

//OpenGL
#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
#include <GL/glew.h>
#endif
#ifdef Darwin
  #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GL/freeglut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/freeglut.h>
#endif

#include "Shaders.h"

reto::Camera* camera;
nlgeometry::Meshes meshes;
nlrender::Renderer* renderer;
std::vector< Eigen::Matrix4f > models;
nlgeometry::ObjWriter objw;

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
  renderer = new nlrender::Renderer( );

  renderer->lod( ) = 0;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 3 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::CENTER;
  format[2] = nlgeometry::TAttribType::TANGENT;

  nsol::SwcReader swcr;

  for ( int i = 1; i < argc; i++ )
  {
    nsol::NeuronMorphologyPtr morphology =
      swcr.readMorphology( std::string( argv[i] ));
    mesh = nlgenerator::MeshGenerator::generateMesh( morphology );
    std::cout << "Loaded morphology with: "
              << mesh->vertices( ).size( ) << " vertices, "
              << mesh->triangles( ).size( ) << " triangles and "
              << mesh->quads( ).size( ) << " quads" << std::endl;

    mesh->uploadGPU( format, nlgeometry::Facet::PATCHES );
    meshes.push_back( mesh );
    models.push_back( Eigen::Matrix4f::Identity( ));
  }

  camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
  camera->radius( 100.0f );
  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection.transpose( );
  Eigen::Matrix4f view( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view.transpose( );

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

  glewExperimental = GL_TRUE;
  glewInit( );

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

  for ( auto m: meshes )
    renderer->render( m, m->modelMatrix( ), Eigen::Vector3f( 1.0f, 0.0f, 0.0f ));
  // renderer->render( meshes, models );

  glFlush( );
  glutSwapBuffers( );
}

void keyboardFunc( unsigned char key, int, int )
{
  Eigen::Matrix4f view;
  switch( key )
  {
    // Camera control.
    case 'c':
    case 'C':
      camera->pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
      camera->radius( 1000.0f );
      camera->rotation( 0.0f, 0.0f );
      std::cout << "Centering." << std::endl;
      view = Eigen::Matrix4f( camera->viewMatrix( ));
      renderer->viewMatrix( ) = view.transpose( );
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
  case 'w':
    renderer->lod( ) += 0.1f;
    break;
  case 's':
    renderer->lod( ) -= 0.1f;
    break;
  case 'x':
    for ( auto mesh: meshes )
    {

      objw.writeMesh( renderer->extract( mesh,  mesh->modelMatrix( )),
                      "test.obj" );
    }
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
      Eigen::Matrix4f view( camera->viewMatrix( ));
      renderer->viewMatrix( ) = view.transpose( );
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
    Eigen::Matrix4f view( camera->viewMatrix( ));
    renderer->viewMatrix( ) = view.transpose( );
    previousX = x;
    previousY = y;
    glutPostRedisplay();
  }
}

void resizeFunc( int width, int height )
{
  camera->ratio((( double ) width ) / height );
  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection.transpose( );
  glViewport( 0, 0, width, height );
}
