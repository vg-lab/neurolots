
#include <iostream>
#include <boost/filesystem.hpp>

#include <nlrender/nlrender.h>
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

#include "Shaders.h"

reto::Camera* camera;
reto::AbstractCameraController* cController;
nlrender::Renderer* renderer;

//Read
nlgeometry::VDMapPtr vdmap;
Eigen::Matrix4f model;
unsigned int textureSize;

//Render
nlgeometry::Meshes meshes;
reto::ShaderProgram* program;

void renderFunc( void );
void initContext( int argc, char* argv[ ]);
void initOGL( void );

int main ( int argc, char* argv[])
{
  if(argc < 3)
  {
          std::cerr << "Error: Usage: " << argv[0] << "vdmap_displacements[.tiff] "
            << "vdmap_normals[.tiff]  -out file[.obj|.off] "
            << " -lod levelOfDetail[float]"<< std::endl;
  return 1;
  }

  //Las texturas, leidas como strings
  // porque arg[1], [2]?
  std::string displaPath( argv[1]);
  std::string normalPath (argv[2]);
  std::string outFile( "out.obj");
  float lod=64.0f;

  for(int i=3; i < argc;i++)
  {
      std::string option( argv[i]);
      try
      {
          //No tengo muy claro que hace aquí pero realmente me importa bien poco
          if(option.compare("-out")==0)
          {
              i++;
              outFile = std::string(argv[i]);
          }
          else if( option.compare("-lod"))
          {
              i++;
              lod= atof( argv[i]);
          }
      }
      catch(...)
      {
        std::cerr << "Error: Usage: " << argv[0] << "vdmap_displacements[.tiff] "
                    << "vdmap_normals[.tiff]  -out file[.obj|.off] "
                    << " -lod levelOfDetail[float]"<< std::endl;
        return 1;        
      }        
  }
  initContext(argc, argv);
  initOGL();

  camera= new reto::Camera();
  cController = new reto::OrbitalCameraController( camera );
  DemoCallbacks::camera(cController);

  renderer = new nlrender::Renderer();
  renderer->lod()= lod;

  vdmap= nlgeometry::VDMapReader::readVDMap(displaPath, normalPath);
  
  Eigen::Matrix4f projection(camera->projectionMatrix());
  renderer->projectionMatrix()=projection;
  Eigen::Matrix4f view(camera->viewMatrix());
  renderer->viewMatrix()=view;

  nlgeometry::MeshPtr mesh=renderer->extract(vdmap);

  nlgeometry::AxisAlignedBoundingBox aabb;
  nlgeometry::AttribsFormat format( 2 );
  format[0] = nlgeometry::TAttribType::POSITION;
  format[1] = nlgeometry::TAttribType::NORMAL;

  //for ( int i = 1; i < argc; i++ )
  //{


    if ( mesh->vertices( ).size( ) > 0  && mesh->triangles( ).size( ) > 0 )
    {
      std::cout << "Loaded " << argv[0] << " with: "
                << mesh->vertices( ).size( ) << " vertices, "
                << mesh->triangles( ).size( ) << " triangles and "
                << mesh->quads( ).size( ) << " quads" << std::endl;
      mesh->computeNormals( );
      mesh->uploadGPU( format );
      mesh->computeBoundingBox( );
      mesh->clearCPUData( );

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

      meshes.push_back( mesh );
    }
    else
      std::cout << "File " << argv[0] << " not loaded" << std::endl;
  //}

  cController->position( aabb.center( ));
  cController->radius( aabb.radius( ) / sin( 3.1416f * 0.25f ));

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
  glutCreateWindow( "Neurolots example: Displacement with PCA" );

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

  for ( auto mesh: meshes )
  {
    program->sendUniform4m("model", mesh->modelMatrixVectorized( ));
    mesh->renderTriangles( );
    mesh->renderQuads( );
  }

  glFlush( );
  glutSwapBuffers( );
}