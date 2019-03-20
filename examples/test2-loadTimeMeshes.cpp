 #include <iostream>

#include <nlgeometry/nlgeometry.h>
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
reto::ShaderProgram* program;
nlgeometry::MeshPtr globalMesh;

void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );


int main( int argc, char* argv[] )
{
  std::cout << "neurolots example: Obj Render" << std::endl;
  if( argc < 2 )
  {
    std::cerr << "Usage error: " << argv[0] << " file[.obj]" << std::endl;
    return -1;
  }

  initContext( argc, argv );
  initOGL( );

  camera = new reto::Camera( );
  cController = new reto::OrbitalCameraController( camera );
  DemoCallbacks::camera( cController );

  nlgeometry::AxisAlignedBoundingBox aabb;
  nlgeometry::MeshPtr mesh;
  nlgeometry::AttribsFormat format( 2 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::NORMAL;

  nlgeometry::ObjReader objr;
  globalMesh = new nlgeometry::Mesh( );
  int nMeshes = 0;

  auto startTime = std::chrono::system_clock::now( );
  for ( int i = 1; i < argc; i++ )
  {

    mesh = objr.readMesh( std::string( argv[i] ), true );

    if ( mesh->vertices( ).size( ) > 0  && mesh->triangles( ).size( ) > 0 )
    {
      nMeshes++;
      mesh->computeNormals( );
      mesh->computeBoundingBox( );

      nlgeometry::AxisAlignedBoundingBox meshAABB = mesh->aaBoundingBox( );

      if ( meshAABB.minimum( ).x( ) < aabb.minimum( ).x( ))
        aabb.minimum( ).x( ) = meshAABB.minimum( ).x();
      if ( meshAABB.minimum( ).y( ) < aabb.minimum( ).y( ))
        aabb.minimum( ).y( ) = meshAABB.minimum( ).y();
      if ( meshAABB.minimum( ).z( ) < aabb.minimum( ).z( ))
        aabb.minimum( ).z( ) = meshAABB.minimum( ).z();

      if ( meshAABB.maximum( ).x( ) > aabb.maximum( ).x( ))
        aabb.maximum( ).x( ) = meshAABB.maximum( ).x();
      if ( meshAABB.maximum( ).y( ) > aabb.maximum( ).y( ))
        aabb.maximum( ).y( ) = meshAABB.maximum( ).y();
      if ( meshAABB.maximum( ).z( ) > aabb.maximum( ).z( ))
        aabb.maximum( ).z( ) = meshAABB.maximum( ).z();

      auto mTriangles = mesh->triangles( );
      auto triangles = globalMesh->triangles( );
      triangles.insert( triangles.end( ), mTriangles.begin( ),
                        mTriangles.end( ));
      globalMesh->triangles( ) = triangles;

      auto mVertices = mesh->vertices( );
      auto vertices = globalMesh->vertices( );
      vertices.insert( vertices.end( ), mVertices.begin( ),
                        mVertices.end( ));
      globalMesh->vertices( ) = vertices;
    }
    else
      std::cout << "File " << argv[i] << " not loaded" << std::endl;
  }
  std::cout << "Number of meshes: " << nMeshes << std::endl;
  std::cout << "\tnumber of triangles: " << globalMesh->triangles( ).size( )
            << std::endl;
  std::cout << "\tnumber of vertices: " << globalMesh->vertices( ).size( )
            << std::endl;
  globalMesh->uploadGPU( format );
  globalMesh->clearCPUData( );

  auto endTime = std::chrono::system_clock::now( );
  auto loadDuration =
    (float)std::chrono::duration_cast< std::chrono::milliseconds >
    ( endTime - startTime ).count( );
  std::cout << "\tmeshes load time: " << loadDuration << " millisecond"
            << std::endl;

  // cController->position( aabb.center( ));
  // cController->radius( aabb.radius( ) / sin( 3.1416f * 0.25f ));

  // glutMainLoop( );
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
  glutKeyboardFunc( DemoCallbacks::keyboardFunc );
  glutMouseFunc( DemoCallbacks::mouseFunc );
  glutMotionFunc( DemoCallbacks::mouseMotionFunc );
  glutReshapeFunc( DemoCallbacks::resizeFunc );
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
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void renderFunc( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  program->use( );
  program->sendUniform4m("proj", camera->projectionMatrix( ));
  program->sendUniform4m("view", camera->viewMatrix( ));
  program->sendUniform4m("model", globalMesh->modelMatrixVectorized( ));
  globalMesh->renderTriangles( );
  // globalMesh->renderQuads( );
  
  glFlush( );
  glutSwapBuffers( );
}
