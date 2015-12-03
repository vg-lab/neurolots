#include "OpenGLWidget.h"
#include <QOpenGLContext>
#include <QMouseEvent>
#include <QColorDialog>
#include <sstream>
#include <string>
#include <iostream>

#include "MainWindow.h"
#include "../nlrender/Config.h"

OpenGLWidget::OpenGLWidget( QWidget* parent_,
                            Qt::WindowFlags windowsFlags_,
                            const std::string&
#ifdef NEUROLOTS_USE_ZEQ
                            zeqUri
#endif
  )
  : QOpenGLWidget( parent_, windowsFlags_ )
  , _fpsLabel( this )
  , _showFps( false )
  , _wireframe( false )
  , _neuronsCollection( nullptr )
  , _frameCount( 0 )
  , _mouseX( 0 )
  , _mouseY( 0 )
  , _rotation( false )
  , _translation( false )
  , _idleUpdate( true )
  , _paint( false )
  , _currentClearColor( 20, 20, 20, 0 )
  , _neuron( nullptr )
{
#ifdef NEUROLOTS_USE_ZEQ
  if ( zeqUri != "" )
  {
    std::cout << zeqUri << std::endl;
    _camera = new neurolots::Camera( zeqUri );
  }
  else
#endif
    _camera = new neurolots::Camera( );

  _cameraTimer = new QTimer( );
  _cameraTimer->start(( 1.0f / 60.f ) * 100 );
  connect( _cameraTimer, SIGNAL( timeout( )), this, SLOT( timerUpdate( )));

  _fpsLabel.setStyleSheet(
    "QLabel { background-color : #333;"
    "color : white;"
    "padding: 3px;"
    "margin: 10px;"
    " border-radius: 10px;}" );

  // This is needed to get key evends
  this->setFocusPolicy( Qt::WheelFocus );

}


OpenGLWidget::~OpenGLWidget( void )
{
  delete _camera;
}

void OpenGLWidget::createNeuronsCollection( void )
{
  makeCurrent( );
  neurolots::nlrender::Config::init( );
  _neuronsCollection = new neurolots::NeuronsCollection( _camera );
}

void OpenGLWidget::loadData( const std::string& fileName,
                             const TDataFileType fileType,
                             const std::string& target )
{

  makeCurrent( );

  switch( fileType )
  {
  case TDataFileType::BlueConfig:
    _neuronsCollection->loadBlueConfig( fileName, target );
    break;

  case TDataFileType::SWC:
    _neuronsCollection->loadSwc( fileName );
    break;

  case TDataFileType::NsolScene:
    _neuronsCollection->loadScene( fileName );
    break;

  default:
    throw std::runtime_error( "Data file type not supported" );

  }

  this->_paint = true;
  update( );

  return;
}


void OpenGLWidget::initializeGL( void )
{
  initializeOpenGLFunctions( );

  glEnable( GL_DEPTH_TEST );
  glClearColor( float( _currentClearColor.red( )) / 255.0f,
                float( _currentClearColor.green( )) / 255.0f,
                float( _currentClearColor.blue( )) / 255.0f,
                float( _currentClearColor.alpha( )) / 255.0f );
  glPolygonMode( GL_FRONT_AND_BACK , GL_FILL );
  glEnable( GL_CULL_FACE );

  glLineWidth( 1.5 );

  _then = std::chrono::system_clock::now( );


  QOpenGLWidget::initializeGL( );

}

void OpenGLWidget::paintGL( void )
{

  _frameCount++;
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  if ( _paint )
  {
    if ( _neuron )
      //_neuronsCollection->extractMesh( _neuron );
      _neuronsCollection->PaintNeuron( _neuron );
    else if ( _neuronsCollection )
      _neuronsCollection->Paint( );

    glUseProgram( 0 );
    glFlush( );

  }

  #define FRAMES_PAINTED_TO_MEASURE_FPS 10
  if ( _frameCount % FRAMES_PAINTED_TO_MEASURE_FPS  == 0 )
  {

    std::chrono::time_point< std::chrono::system_clock > now =
      std::chrono::system_clock::now( );

    auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>( now - _then );
    _then = now;

    MainWindow* mainWindow = dynamic_cast< MainWindow* >( parent( ));
    if ( mainWindow )
    {

      unsigned int ellapsedMiliseconds = duration.count( );

      unsigned int fps = roundf( 1000.0f *
                                 float( FRAMES_PAINTED_TO_MEASURE_FPS ) /
                                 float( ellapsedMiliseconds ));

      // mainWindow->showStatusBarMessage(
      //   QString::number( fps ) + QString( " FPS" ));
      if ( _showFps )
      {
        _fpsLabel.setVisible( true );
        _fpsLabel.setText( QString::number( fps ) + QString( " FPS" ));
        _fpsLabel.adjustSize( );
      }
      else
        _fpsLabel.setVisible( false );
    }

  }

  if ( _idleUpdate )
  {
    // std::cout << _frameCount << std::endl;
    update( );
  }
  else
  {
    _fpsLabel.setVisible( false );
  }

}

void OpenGLWidget::resizeGL( int w , int h )
{
  _camera->Ratio((( double ) w ) / h );
  glViewport( 0, 0, w, h );


}


void OpenGLWidget::mousePressEvent( QMouseEvent* event_ )
{

  if ( event_->button( ) == Qt::LeftButton )
  {
    _rotation = true;
    _mouseX = event_->x( );
    _mouseY = event_->y( );
  }

  if ( event_->button( ) ==  Qt::MidButton )
  {
    _translation = true;
    _mouseX = event_->x( );
    _mouseY = event_->y( );
  }

  update( );

}

void OpenGLWidget::mouseReleaseEvent( QMouseEvent* event_ )
{
  if ( event_->button( ) == Qt::LeftButton)
  {
    _rotation = false;
  }

  if ( event_->button( ) ==  Qt::MidButton )
  {
    _translation = false;
  }

  update( );

}

void OpenGLWidget::mouseMoveEvent( QMouseEvent* event_ )
{
  if( _rotation )
  {
    _camera->LocalRotation( -( _mouseX - event_->x( )) * 0.01,
                            -( _mouseY - event_->y( )) * 0.01 );
    _mouseX = event_->x( );
    _mouseY = event_->y( );
  }
  if( _translation )
  {
    _mouseX = event_->x( );
    _mouseY = event_->y( );
  }

  this->update( );
}


void OpenGLWidget::wheelEvent( QWheelEvent* event_ )
{

  int delta = event_->angleDelta( ).y( );

  if ( delta > 0 )
    _camera->Radius( _camera->Radius( ) / 1.1f );
  else
    _camera->Radius( _camera->Radius( ) * 1.1f );

  update( );

}



void OpenGLWidget::keyPressEvent( QKeyEvent* event_ )
{
  makeCurrent( );

  switch ( event_->key( ))
  {
  case Qt::Key_C:
    _camera->Pivot( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ));
    _camera->Radius( 1000.0f );
    _camera->Rotation( 0.0f, 0.0f );
    update( );
    break;
  }
}


void OpenGLWidget::changeClearColor( void )
{
  QColor color =
    QColorDialog::getColor( _currentClearColor, parentWidget( ),
                            "Choose new background color",
                            QColorDialog::DontUseNativeDialog);

  if ( color.isValid( ))
  {
    _currentClearColor = color;

    makeCurrent( );
    glClearColor( float( _currentClearColor.red( )) / 255.0f,
                  float( _currentClearColor.green( )) / 255.0f,
                  float( _currentClearColor.blue( )) / 255.0f,
                  float( _currentClearColor.alpha( )) / 255.0f );
    update( );
  }
}


void OpenGLWidget::toggleUpdateOnIdle( void )
{
  _idleUpdate = !_idleUpdate;
  if ( _idleUpdate )
    update( );
}

void OpenGLWidget::toggleShowFPS( void )
{
  _showFps = !_showFps;
  if ( _idleUpdate )
    update( );
}

void OpenGLWidget::toggleWireframe( void )
{
  makeCurrent( );
  _wireframe = !_wireframe;

  if ( _wireframe )
  {
    glEnable( GL_POLYGON_OFFSET_LINE );
    glPolygonOffset( -1, -1 );
    glLineWidth( 1.5 );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }
  else
  {
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glDisable( GL_POLYGON_OFFSET_LINE );
  }

  update( );
}

void OpenGLWidget::timerUpdate( void )
{
  if( _camera->Anim( ))
    this->update( );
}

void OpenGLWidget::extractMesh( void )
{
  if( _neuron )
  {
    this->makeCurrent( );
    _neuronsCollection->extractMesh( _neuron );
    glUseProgram( 0 );
  }
}

void OpenGLWidget::onLotValueChanged( int value_ )
{
  _neuronsCollection->lod( ( float ) value_ );
  update( );
}

void OpenGLWidget::onDistanceValueChanged( int value_ )
{
  _neuronsCollection->maxDist(( float ) value_ / 1000.0f );
  update( );
}

void OpenGLWidget::onTangValueChanged( int value_ )
{
  _neuronsCollection->tng(( float ) value_ / 50.0f );
  update( );
}

void OpenGLWidget::onHomogeneousClicked( void )
{
  _neuronsCollection->tessMethod( neurolots::NeuronsCollection::HOMOGENEOUS );
  update( );
}

void OpenGLWidget::onLinearClicked( void )
{
  _neuronsCollection->tessMethod( neurolots::NeuronsCollection::LINEAR );
  update( );
}
