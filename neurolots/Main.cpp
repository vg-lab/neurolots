//OpenGL
#include <GL/glew.h>
#ifdef Darwin
#include <gl.h>
#include <glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

//Eigen
#include <Eigen/Dense>

//Neurolots
#include "Camera.h"
#include "Program.h"
#include "NeuronMesh.h"
#include "NeuronsCollection.h"


#include <neurolots/nlfem/version.h>
#include <neurolots/nlgenerator/version.h>

#include <iostream>

#ifdef NEUROLOTS_USE_DEFLECT
#include <deflect/deflect.h>
#endif


#define WINDOW_TITLE_PREFIX "NeuroLoTs"

using Eigen::Vector3d;
using namespace std;
using namespace neurolots;


bool rotation = false;
bool translation = false;

bool paintSoma = true;
bool paintNeurites = true;

int m_x, m_y;

bool mode = true;

unsigned int frameCount = 0;

Camera* camera;
NeuronsCollection* neuronsCollection;

bool atLeastTwo( bool a, bool b, bool c )
{
  return a ^ b ? c : a;
}

#ifdef NEUROLOTS_USE_DEFLECT

bool deflectConnect = false;
std::string deflectHost;

deflect::Stream* deflectStream;

bool deflectCompressImage = true;
unsigned int deflectCompressionQuality = 75;


void handleStreamingError(const char* errorMessage)
{
    std::cerr << errorMessage <<  std::endl;
}

void startStreaming(const char* deflectHostName = "NeuroLOTs",
                    const char* deflectHostAddress = "localhost")
{
  if ( deflectStream )
    return;

  deflectStream = new deflect::Stream( deflectHostName,
                                        deflectHostAddress);
  if (!deflectStream->isConnected())
  {
      delete deflectStream;
      deflectStream = nullptr;
      return;
  }
  deflectStream->registerForEvents();
}

void stopStreaming( void )
{
  delete deflectStream;
  deflectStream = nullptr;
}

void processDeflectEvents( void )
{
  if( !deflectStream )
    return;

  if( !deflectStream->isRegisteredForEvents( ))
    return;

  deflect::Event lastEvent;

  unsigned int windowWidth = glutGet( GLUT_WINDOW_WIDTH ),
               windowHeight = glutGet( GLUT_WINDOW_HEIGHT );

  while( deflectStream->hasEvent( ))
  {

    const deflect::Event& wallEvent = deflectStream->getEvent();

    float mouseX = wallEvent.mouseX * windowWidth;
    float mouseY = wallEvent.mouseY * windowHeight;

    switch (wallEvent.type)
    {
      case deflect::Event::EVT_PRESS:
        rotation = true;
        m_x = mouseX;
        m_y = mouseY;
        break;

      case deflect::Event::EVT_RELEASE:
        rotation = false;
        break;
      case deflect::Event::EVT_MOVE:
        if( rotation )
          {
            camera->LocalRotation( -( m_x - mouseX ) * 0.01,
                                   -( m_y - mouseY ) * 0.01 );
            m_x = mouseX;
            m_y = mouseY;
          }
        break;
      case deflect::Event::EVT_WHEEL:

        if (wallEvent.dy < 0)
          camera->Radius( camera->Radius( ) * 1.1f );
        else
          camera->Radius( camera->Radius( ) / 1.1f );
        break;

      default:
        break;
    }
  }
}


void sendDeflectFrameImage( bool compress = true,
                            unsigned int compressedQuality = 75 )
{

  if( !deflectStream )
    return;

  // Grab the frame from OpenGL
  const int windowWidth = glutGet( GLUT_WINDOW_WIDTH );
  const int windowHeight = glutGet( GLUT_WINDOW_HEIGHT );

  const size_t imageSize = windowWidth * windowHeight * 4;
  unsigned char* imageData = new unsigned char[imageSize];
  glReadPixels( 0, 0, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                (GLvoid*)imageData );

  // Send the frame through the stream
  deflect::ImageWrapper deflectImage( (const void*)imageData, windowWidth,
                                      windowHeight, deflect::RGBA );
  deflectImage.compressionPolicy = compress ?
                  deflect::COMPRESSION_ON : deflect::COMPRESSION_OFF;
  deflectImage.compressionQuality = compressedQuality;
  deflect::ImageWrapper::swapYAxis( (void*)imageData, windowWidth,
                                    windowHeight, 4 );
  const bool success = deflectStream->send( deflectImage );
  deflectStream->finishFrame();

  if ( !success )
  {
      handleStreamingError("Streaming failure, connection closed.");
      delete deflectStream;
      deflectStream = nullptr;
      return;
  }

  delete [] imageData;
}


void updateStreaming( void )
{
  processDeflectEvents( );
  sendDeflectFrameImage( );
}

#endif

void usageMessage( char* programName )
{
  std::cerr << std::endl
            << "Usage: "
            << programName << "\t"
            << "( -bc file_name [ -target target_name ] | " << std::endl
            << "\t\t\t  -swc swc_file | -xml scene_file ) " << std::endl
            << "\t\t\t[ -zeq uri ] "
            << "[ -pw host ]"
            << std::endl << std::endl;
  exit(-1);
}

void dumpVersion( void )
{

  std::cerr << std::endl
            << "neurolots "
            << nlgenerator::Version::getMajor( ) << "."
            << nlgenerator::Version::getMinor( ) << "."
            << nlgenerator::Version::getPatch( )
            << " (" << nlgenerator::Version::getRevision( ) << ")"
            << std::endl << std::endl;

  std::cerr << "BBPSDK support built-in: ";
  #ifdef NSOL_USE_BBPSDK
  std::cerr << "\tyes";
  #else
  std::cerr << "\tno";
  #endif
  std::cerr << std::endl;

  std::cerr << "zeq support built-in: ";
  #ifdef NEUROLOTS_USE_ZEQ
  std::cerr << "\t\tyes";
  #else
  std::cerr << "\t\tno";
  #endif
  std::cerr << std::endl;

  std::cerr << "GmrvZeq support built-in: ";
  #ifdef NEUROLOTS_USE_GMRVZEQ
  std::cerr << "\tyes";
  #else
  std::cerr << "\tno";
  #endif
  std::cerr << std::endl;

  std::cerr << "Deflect support built-in: ";
  #ifdef NEUROLOTS_USE_DEFLECT
  std::cerr << "\tyes";
  #else
  std::cerr << "\tno";
  #endif
  std::cerr << std::endl;

  std::cerr << std::endl;

}


void sceneInit( void )
{
  glEnable( GL_DEPTH_TEST );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glPolygonMode( GL_FRONT_AND_BACK , GL_FILL );
  glEnable( GL_CULL_FACE );

  glLineWidth( 1.5 );

}

void paintFunc(void)
{
  frameCount ++;
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  camera->Anim( );
  neuronsCollection->Paint( );

#ifdef NEUROLOTS_USE_DEFLECT
  if( deflectConnect )
    updateStreaming();
#endif

  glUseProgram( 0 );
  glFlush( );
  glutSwapBuffers( );
}

void keyboardFunc( unsigned char key, int /* _x */, int /* _y */ )
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
      camera->Pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
      camera->Radius( 1000.0f );
      camera->Rotation( 0.0f, 0.0f );
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
#ifdef NEUROLOTS_USE_DEFLECT
    case 'p':
      if ( deflectConnect && !deflectStream )
        startStreaming("NeuroLOTs", deflectHost.c_str());
      break;
#endif
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
    camera->Radius( camera->Radius( ) / 1.1f );
  }
  if ( boton == 4 && state == GLUT_DOWN )
  {
    camera->Radius( camera->Radius( ) * 1.1f );
  }
}

void mouseMoveFunc( int _x, int _y )
{
  if( rotation )
  {
    camera->LocalRotation( -( m_x - _x ) * 0.01, -( m_y - _y ) * 0.01 );
    m_x = _x;
    m_y = _y;
  }
  if( translation )
  {
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
  camera->Ratio((( double ) w ) / h );
  glViewport( 0, 0, w, h );
}

void timerFunc( int value )
{
  if( 0 != value )
  {
    char* tmpString = new char[ 512 + strlen( WINDOW_TITLE_PREFIX )];

    sprintf( tmpString, "%s: %d FPS", WINDOW_TITLE_PREFIX, frameCount );

    glutSetWindowTitle( tmpString );
    free( tmpString );
  }

  frameCount = 0;
  glutTimerFunc( 1000, timerFunc, 1 );
}

int main( int argc, char* argv[ ])
{
  if( argc < 2 )
  {
    usageMessage( argv[0] );
  }

  glutInit( &argc, argv );
  glutInitContextVersion( 4, 4 );

  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
  glutInitWindowPosition( 0, 0 );
  glutInitWindowSize( 600, 600 );
  glutCreateWindow( WINDOW_TITLE_PREFIX );

  glutDisplayFunc( paintFunc );
  glutKeyboardFunc( keyboardFunc );
  glutMouseFunc( mouseFunc );
  glutMotionFunc( mouseMoveFunc );
  glutIdleFunc( idleFunc );
  glutReshapeFunc( resizeFunc );
  glutTimerFunc( 0, timerFunc, 0);

  glewExperimental = GL_TRUE;
  glewInit( );

  camera = nullptr;
  neuronsCollection = nullptr;

  std::string blueConfig;
  std::string swcFile;
  std::string sceneFile;
  std::string uri;
  std::string target = std::string( "" );



  for( int i = 1; i < argc; i++ )
  {
    if ( std::strcmp( argv[i], "--version" ) == 0 )
    {
      dumpVersion( );
      return 0;
    }

    if( std::strcmp( argv[ i ], "-zeq" ) == 0 )
    {
#ifdef NEUROLOTS_USE_ZEQ
      if( ++i < argc )
      {
        uri = std::string( argv[ i ]);
      }
#else
      std::cerr << "Zeq not supported " << std::endl;
      return -1;
#endif
    }
    if( std::strcmp( argv[ i ], "-bc" ) == 0 )
    {
      if( ++i < argc )
      {
        blueConfig = std::string( argv[ i ]);
      }
      else
        usageMessage( argv[0] );

    }
    if( std::strcmp( argv[ i ], "-swc" ) == 0 )
    {
      if( ++i < argc )
      {
        swcFile = std::string( argv[ i ]);
      }
      else
        usageMessage( argv[0] );

    }
    if( std::strcmp( argv[ i ], "-xml" ) == 0 )
    {
      if( ++i < argc )
      {
        sceneFile = std::string( argv[ i ]);
      }
      else
        usageMessage( argv[0] );

    }
    if( std::strcmp( argv[ i ], "-target" ) == 0 )
    {
      if(++i < argc )
      {
        target = std::string( argv[ i ]);
      }
      else
        usageMessage( argv[0] );

    }
    if( std::strcmp( argv[ i ], "-pw" ) == 0 )
    {
#ifdef NEUROLOTS_USE_DEFLECT
      if( ++i < argc )
      {
        deflectHost = std::string( argv[ i ]);
        deflectConnect = true;
        startStreaming("NeuroLOTs", deflectHost.c_str());
      }
      else
        usageMessage( argv[0] );

#else
      std::cerr << "Deflect not supported " << std::endl;
      return -1;
#endif
    }
  }

#ifdef NEUROLOTS_USE_ZEQ
  if ( !uri.empty( ))
    camera = new Camera( uri );
  else
    camera = new Camera( );
#else
  camera = new Camera( );
#endif
  neuronsCollection = new NeuronsCollection( camera );

  if( !uri.empty( ))
    neuronsCollection->setZeqUri( uri );

  if ( atLeastTwo( !blueConfig.empty( ),
                   !swcFile.empty( ),
                   !sceneFile.empty( )))
  {
    std::cerr << "Error: -swc, -xml and -bc options are exclusive" << std::endl;
    usageMessage( argv[0] );
  }

  if ( blueConfig.empty( ) & swcFile.empty( ) & sceneFile.empty( ))
  {
    std::cerr << "Error: no -swc, -xml and -bc options selected" << std::endl;
    usageMessage( argv[0] );
  }


  if( !blueConfig.empty( ))
    neuronsCollection->loadBlueConfig( blueConfig, target );
  else if( !swcFile.empty( ))
    neuronsCollection->loadSwc( swcFile );
  else if ( !sceneFile.empty( ))
    neuronsCollection->loadScene( sceneFile );

  sceneInit( );

  glutMainLoop( );

  return 0;
}
