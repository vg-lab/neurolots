#ifndef __QNEUROLOTS__OPENGLWIDGET__
#define __QNEUROLOTS__OPENGLWIDGET__

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QLabel>
#include <chrono>

#define NEUROLOTS_SKIP_GLEW_INCLUDE 1
#include "../nlrender/Camera.h"
#include "../nlrender/NeuronsCollection.h"

class OpenGLWidget
  : public QOpenGLWidget
  , public QOpenGLFunctions
{

  Q_OBJECT;

public:

  typedef enum
  {
    BlueConfig,
    SWC,
    NsolScene
  } TDataFileType;

  OpenGLWidget( QWidget* parent = 0,
                Qt::WindowFlags windowFlags = 0,
                const std::string& zeqUri = "" );
  ~OpenGLWidget( void );

  void createNeuronsCollection( void );

  void loadData( const std::string& fileName,
                 const TDataFileType fileType = TDataFileType::BlueConfig,
                 const std::string& target = std::string( "" ));

  void idleUpdate( bool idleUpdate_ = true )
  {
    _idleUpdate = idleUpdate_;
  }

public slots:

  void toggleUpdateOnIdle( void )
  {
    _idleUpdate = !_idleUpdate;
    if ( _idleUpdate )
      update( );
  }

  void toggleShowFPS( void )
  {
    _showFps = !_showFps;
    if ( _idleUpdate )
      update( );
  }

  void changeClearColor( void );


protected:

  virtual void initializeGL( void );
  virtual void paintGL( void );
  virtual void resizeGL( int w, int h );

  virtual void mousePressEvent( QMouseEvent* event );
  virtual void mouseReleaseEvent( QMouseEvent* event );
  virtual void wheelEvent( QWheelEvent* event );
  virtual void mouseMoveEvent( QMouseEvent* event );

  QLabel _fpsLabel;
  bool _showFps;

  neurolots::Camera* _camera;
  neurolots::NeuronsCollection* _neuronsCollection;
  unsigned int _frameCount;

  int _mouseX, _mouseY;
  bool _rotation, _translation;

  bool _idleUpdate;
  bool _paint;

  QColor _currentClearColor;

  std::chrono::time_point< std::chrono::system_clock > _then;

};

#endif // __QNEUROLOTS__OPENGLWIDGET__
