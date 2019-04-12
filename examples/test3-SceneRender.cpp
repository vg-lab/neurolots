#include <iostream>

#include <nlgeometry/nlgeometry.h>
#include <nlrender/nlrender.h>
#include <reto/reto.h>

#include "DemoCallbacks.h"

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

#include <chrono>
#include "Shaders.h"

reto::Camera* camera;
reto::AbstractCameraController* cController;
nlrender::Renderer* renderer;
nlgeometry::VDMapCollection* vdmCollec;
bool collection = false;
bool wireMode = false;

void renderFunc( void );
void keyboardFunc( unsigned char key, int, int );
void initContext( int argc, char* argv[ ]);
void initOGL( void );
void initCameraAnimation( void );

unsigned int numFrames = 0;

std::chrono::time_point< std::chrono::system_clock > startTime;
std::chrono::time_point< std::chrono::system_clock > previousTime;

int main( int argc, char* argv[] )
{
  if( argc < 2 )
  {
    std::cerr << "Usage error: " << argv[0] << " scene[.xml]" << std::endl;
    return -1;
  }

  std::string sceneFile( argv[1] );

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( 45.0f, 1.0f, 0.1f, 10000.0f  );
  cController = new reto::OrbitalCameraController( camera );
  DemoCallbacks::camera( cController );
  renderer = new nlrender::Renderer( );

  vdmCollec = nlgeometry::VDMapReader::readVDMapCollection( sceneFile );

  if ( !vdmCollec )
  {
    std::cerr << "Error: Usage: " << argv[0] << " spine_scene[.xml]"
              << std::endl;
    return 1;
  }

  std::vector< Eigen::Matrix4f > models;
  if ( vdmCollec->macroMap( ) )
  {
    collection = true;
    std::cout << "loaded as colection" << std::endl;
    models = vdmCollec->macroModels( );
  }
  else
    models = vdmCollec->models( );

  Eigen::Array3f minimum =
    Eigen::Array3f::Constant( std::numeric_limits< float >::max( ));
  Eigen::Array3f maximum =
    Eigen::Array3f::Constant( std::numeric_limits< float >::min( ));

  for ( Eigen::Matrix4f model: models )
  {
    Eigen::Array3f pos( model.block( 0, 3, 1, 3 ));
    minimum = minimum.min( pos );
    maximum = maximum.max( pos );
  }
  minimum += Eigen::Array3f( -2.0f, -2.0f, -2.0f );
  maximum += Eigen::Array3f( 2.0f, 2.0f, 2.0f );

  Eigen::Vector3f center(( maximum + minimum ) * 0.5f );

  cController->position( center );
  cController->radius(
    ( center - Eigen::Vector3f( minimum )).norm( ) / sin( 3.1416f * 0.25f ));

  Eigen::Matrix4f projection( camera->projectionMatrix( ));
  renderer->projectionMatrix( ) = projection;
  Eigen::Matrix4f view( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view;
  renderer->tessCriteria( ) = nlrender::Renderer::LINEAR;
  renderer->maximumDistance( ) = 100.0f;
  vdmCollec->uploadGPU( );
  renderer->lod( ) = vdmCollec->vdmapsSize( ) - 1;
  startTime = std::chrono::system_clock::now( );
  previousTime = startTime;
  initCameraAnimation( );


  while ( cController->isAniming( ))
  {
    numFrames ++;

    auto currentTime = std::chrono::system_clock::now( );
    auto duration = std::chrono::duration_cast< std::chrono::microseconds >
      ( currentTime - previousTime );
    float dt = duration.count( ) * 0.000001f;
    previousTime = currentTime;
    renderFunc( );
    cController->anim( dt );
  }

  auto currentTime = std::chrono::system_clock::now( );
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >
      ( currentTime - startTime );
  std::cout << "FPS: " << numFrames / ( duration.count( )*0.001 ) << std::endl;
  return 0;
}

void initContext( int argc, char* argv[ ])
{
  glutInit( &argc, argv );
  glutInitContextVersion( 4, 0 );

  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  glutInitWindowSize( 600, 600 );
  glutInitWindowPosition( 0, 0 );
  glutCreateWindow( "Neurolots example: Obj Render" );

  glewExperimental = GL_TRUE;
  glewInit( );

  glutDisplayFunc( renderFunc );
  glutIdleFunc( DemoCallbacks::idleFunc );
  glutKeyboardFunc( keyboardFunc );
  glutMouseFunc( DemoCallbacks::mouseFunc );
  glutMotionFunc( DemoCallbacks::mouseMotionFunc );
  glutReshapeFunc( DemoCallbacks::resizeFunc );
}

void initOGL( void )
{
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

  glEnable( GL_DEPTH_TEST );
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void renderFunc( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  Eigen::Matrix4f view = Eigen::Matrix4f( camera->viewMatrix( ));
  renderer->viewMatrix( ) = view;
  Eigen::Matrix4f projection = Eigen::Matrix4f( camera->projectionMatrix( ));
  renderer->projectionMatrix( )= projection;

  if ( collection )
    renderer->render( vdmCollec );
  else
    renderer->render( vdmCollec->vdmaps( ), vdmCollec->models( ));

  glFlush( );
  glutSwapBuffers( );
}

void initCameraAnimation( void )
{
  Eigen::Vector3f initPos = cController->position( );
  Eigen::Matrix3f initRot = cController->rotation( );
  float initRadius = cController->radius( );

  auto camAnim = new reto::CameraAnimation( reto::CameraAnimation::LINEAR,
                                            reto::CameraAnimation::LINEAR,
                                            reto::CameraAnimation::LINEAR );
  camAnim->addKeyCamera(
    new reto::KeyCamera( 0.0f, initPos, initRot, initRadius ));

  Eigen::Vector3f pos = initPos + Eigen::Vector3f( 0.0f, 100.0f, 0.0f );
  Eigen::Matrix3f rot =
    cController->rotationMatrixFromAngles( Eigen::Vector3f( 3.1416, 0.0, 0.0 )) *
    initRot;
  float radius = initRadius * 0.1f;
  camAnim->addKeyCamera( new reto::KeyCamera( 10.0f, pos,rot, radius ));

  pos = initPos + Eigen::Vector3f( 0.0f, 0.0f, 0.0f );
  rot =
    cController->rotationMatrixFromAngles( Eigen::Vector3f( 0, 1.4, 0.0 )) *
    initRot;
  radius = initRadius * 1.0f;
  camAnim->addKeyCamera( new reto::KeyCamera( 20.0f, pos,rot, radius ));

  pos = initPos + Eigen::Vector3f( 0.0f, -100.0f, 0.0f );
  rot =
    cController->rotationMatrixFromAngles( Eigen::Vector3f( 0, 1.4, 0.0 )) *
    initRot;
  radius = initRadius * 0.1f;
  camAnim->addKeyCamera( new reto::KeyCamera( 30.0f, pos,rot, radius ));

  pos = initPos + Eigen::Vector3f( 0.0f, 200.0f, 0.0f );
  rot =
    cController->rotationMatrixFromAngles( Eigen::Vector3f( 0, 1.4, 0.0 )) *
    initRot;
  radius = initRadius * 0.1f;
  camAnim->addKeyCamera( new reto::KeyCamera( 40.0f, pos,rot, radius ));

  pos = initPos + Eigen::Vector3f( 10.0f, 200.0f, -20.0f );
  rot =
    cController->rotationMatrixFromAngles( Eigen::Vector3f( 0, 1.4, 0.0 )) *
    initRot;
  radius = initRadius * 0.001f;
  camAnim->addKeyCamera( new reto::KeyCamera( 50.0f, pos,rot, radius ));

  camAnim->addKeyCamera(
    new reto::KeyCamera( 60.0f, initPos,initRot, initRadius ));


  cController->startAnim( camAnim, false );
}

void keyboardFunc( unsigned char key_, int, int )
{
  switch( key_ )
  {
  case 'w':
    renderer->lod( ) += 1.f;
    std::cout << "Level of tessellation: " << renderer->lod( ) << std::endl;
    break;
  case 's':
    renderer->lod( ) -= 1.f;
    std::cout << "Level of tessellation: " << renderer->lod( ) << std::endl;
    break;
  case 'm':
    wireMode = !wireMode;
    if ( wireMode )
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    else
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }
}
