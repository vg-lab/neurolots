#include <QApplication>
#include "MainWindow.h"
#include <QDebug>
#include <QOpenGLWidget>

#include <qneurolots/version.h>

void setFormat( int ctxOpenGLMajor, int ctxOpenGLMinor,
                int ctxOpenGLSamples, int ctxOpenGLVSync );
void usageMessage(  char* progName );
void dumpVersion( void );
bool atLeastTwo( bool a, bool b, bool c );

int main( int argc, char** argv )
{
  QApplication application(argc,argv);

  std::string blueConfig;
  std::string swcFile;
  std::string sceneFile;
  std::string zeqUri;
  std::string target = std::string( "" );
  bool fullscreen = false, initWindowSize = false, initWindowMaximized = false;
  int initWindowWidth, initWindowHeight;


  int ctxOpenGLMajor = DEFAULT_CONTEXT_OPENGL_MAJOR;
  int ctxOpenGLMinor = DEFAULT_CONTEXT_OPENGL_MINOR;
  int ctxOpenGLSamples = 0;
  int ctxOpenGLVSync = 1;


  for( int i = 1; i < argc; i++ )
  {
    if ( std::strcmp( argv[i], "--help" ) == 0 ||
         std::strcmp( argv[i], "-h" ) == 0 )
    {
      usageMessage( argv[0] );
      return 0;
    }
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
        zeqUri = std::string( argv[ i ]);
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
    if ( strcmp( argv[i], "--fullscreen" ) == 0 ||
         strcmp( argv[i],"-fs") == 0 )
    {
      fullscreen = true;
    }
    if ( strcmp( argv[i], "--maximize-window" ) == 0 ||
         strcmp( argv[i],"-mw") == 0 )
    {
      initWindowMaximized = true;
    }
    if ( strcmp( argv[i], "--window-size" ) == 0 ||
         strcmp( argv[i],"-ws") == 0 )
    {
      initWindowSize = true;
      if ( i + 2 >= argc )
        usageMessage( argv[0] );
      initWindowWidth = atoi( argv[ ++i ] );
      initWindowHeight = atoi( argv[ ++i ] );

    }
    if ( strcmp( argv[i], "--context-version" ) == 0 ||
         strcmp( argv[i],"-cv") == 0 )
    {
      initWindowSize = true;
      if ( i + 2 >= argc )
        usageMessage( argv[0] );
      ctxOpenGLMajor = atoi( argv[ ++i ] );
      ctxOpenGLMinor = atoi( argv[ ++i ] );

    }
    if ( strcmp( argv[i], "--samples" ) == 0 ||
         strcmp( argv[i],"-s") == 0 )
    {
      if ( i + 1 >= argc )
        usageMessage( argv[0] );
      ctxOpenGLSamples = atoi( argv[ ++i ] );

    }
    if ( strcmp( argv[i], "--no-vsync" ) == 0 ||
         strcmp( argv[i],"-nvs") == 0 )
    {
      ctxOpenGLVSync = 0;
    }
  }


  setFormat( ctxOpenGLMajor, ctxOpenGLMinor,
             ctxOpenGLSamples, ctxOpenGLVSync );
  MainWindow mainWindow;
  mainWindow.setWindowTitle("QNeuroLOTs");

  if ( initWindowSize )
    mainWindow.resize( initWindowWidth, initWindowHeight );

  if ( initWindowMaximized )
    mainWindow.showMaximized( );

  if ( fullscreen )
    mainWindow.showFullScreen( );

  mainWindow.show( );
  mainWindow.init( zeqUri );

  if ( atLeastTwo( !blueConfig.empty( ),
                   !swcFile.empty( ),
                   !sceneFile.empty( )))
  {
    std::cerr << "Error: -swc, -xml and -bc options are exclusive" << std::endl;
    usageMessage( argv[0] );
  }

  if ( blueConfig != "" )
    mainWindow.openBlueConfig( blueConfig, target );

  if ( swcFile != "" )
    mainWindow.openSWCFile( swcFile );

  if ( sceneFile != "" )
    mainWindow.openXMLScene( sceneFile );


  return application.exec();

}

void usageMessage( char* progName )
{
  std::cerr << std::endl
            << "Usage: "
            << progName << std::endl
            << "\t[ -bc blue_config_path | -swc swc_file_list "
            << " | -xml scene_xml ] "
            << std::endl
            << "\t[ -target target_label ] "
            << std::endl
            << "\t[ -zeq schema* ]"
            << std::endl
            << "\t[ -ws | --window-size ] width height ]"
            << std::endl
            << "\t[ -fs | --fullscreen ] "
            << std::endl
            << "\t[ -mw | --maximize-window ]"
            << std::endl
            << "\t[ -s | --samples ] num_samples (1)"
            << std::endl
            << "\t[ -nvs | --no-vsync ] (2)"
            << std::endl
            << "\t[ -vs | --context-version ] major minor (3)"
            << std::endl
            << "\t[ --version ]"
            << std::endl
            << "\t[ --help | -h ]"
            << std::endl << std::endl
            << "\t(1) overwritten by env var CONTEXT_OPENGL_SAMPLES"
            << std::endl
            << "\t(2) overwritten by env var CONTEXT_OPENGL_VSYNC"
            << std::endl
            << "\t(3) overwritten by env var CONTEXT_OPENGL_MAJOR"
            << std::endl
            << "\t    overwritten by env var CONTEXT_OPENGL_MINOR"
            << std::endl
            << "\t(4) schema: for example hbp://"
            << std::endl << std::endl;
  exit(-1);
}

void dumpVersion( void )
{

  std::cerr << std::endl
            << "qneurolots "
            << qneurolots::Version::getMajor( ) << "."
            << qneurolots::Version::getMinor( ) << "."
            << qneurolots::Version::getPatch( )
            << " (" << qneurolots::Version::getRevision( ) << ")"
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


void setFormat( int ctxOpenGLMajor,
                int ctxOpenGLMinor,
                int ctxOpenGLSamples,
                int ctxOpenGLVSync )
{

  if ( std::getenv("CONTEXT_OPENGL_MAJOR"))
    ctxOpenGLMajor = std::stoi( std::getenv("CONTEXT_OPENGL_MAJOR"));

  if ( std::getenv("CONTEXT_OPENGL_MINOR"))
    ctxOpenGLMinor = std::stoi( std::getenv("CONTEXT_OPENGL_MINOR"));

  if ( std::getenv("CONTEXT_OPENGL_SAMPLES"))
    ctxOpenGLSamples = std::stoi( std::getenv("CONTEXT_OPENGL_SAMPLES"));

  if ( std::getenv("CONTEXT_OPENGL_VSYNC"))
    ctxOpenGLVSync = std::stoi( std::getenv("CONTEXT_OPENGL_VSYNC"));

  std::cerr << "Setting OpenGL context to "
            << ctxOpenGLMajor << "." << ctxOpenGLMinor << std::endl;

  QSurfaceFormat format;
  format.setVersion( ctxOpenGLMajor, ctxOpenGLMinor);
  format.setProfile( QSurfaceFormat::CoreProfile );

  if ( ctxOpenGLSamples != 0 )
    format.setSamples( ctxOpenGLSamples );

  format.setSwapInterval( ctxOpenGLVSync );


  QSurfaceFormat::setDefaultFormat( format );
  if ( std::getenv("CONTEXT_OPENGL_COMPATIBILITY_PROFILE"))
    format.setProfile( QSurfaceFormat::CompatibilityProfile );
  else
    format.setProfile( QSurfaceFormat::CoreProfile );

}

bool atLeastTwo( bool a, bool b, bool c )
{
  return a ^ b ? c : a;
}
