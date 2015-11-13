#ifndef __QNEUROLOTS__OPENGLWIDGET__
#define __QNEUROLOTS__OPENGLWIDGET__

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QLabel>
#include <chrono>

#define NEUROLOTS_SKIP_GLEW_INCLUDE 1
#include "../nlrender/Camera.h"
#include "../nlrender/NeuronsCollection.h"
#include "../nlrender/Neuron.h"

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

  std::vector< unsigned int > neuronIDs( void )
  {
    return _neuronsCollection->neuronIDs( );
  }

  void neuron( int id_ )
  {
    if( id_ > -1 )
    {
      _neuron = _neuronsCollection->neuronById( id_ );
      _neuronsCollection->focusOnNeuron( _neuron );
    }
    else
    {
      _neuron = nullptr;
      _neuronsCollection->focusAll( );
    }
  }

public slots:

  void home( void );
  void changeClearColor( void );
  void toggleUpdateOnIdle( void );
  void toggleShowFPS( void );
  void toggleWireframe( void );


protected:

  virtual void initializeGL( void );
  virtual void paintGL( void );
  virtual void resizeGL( int w, int h );

  virtual void mousePressEvent( QMouseEvent* event );
  virtual void mouseReleaseEvent( QMouseEvent* event );
  virtual void wheelEvent( QWheelEvent* event );
  virtual void mouseMoveEvent( QMouseEvent* event );
  virtual void keyPressEvent( QKeyEvent* event );


  QLabel _fpsLabel;
  bool _showFps;

  bool _wireframe;

  neurolots::Camera* _camera;
  neurolots::NeuronsCollection* _neuronsCollection;
  unsigned int _frameCount;

  int _mouseX, _mouseY;
  bool _rotation, _translation;

  bool _idleUpdate;
  bool _paint;

  QColor _currentClearColor;

  neurolots::NeuronPtr _neuron;

  std::chrono::time_point< std::chrono::system_clock > _then;

};

#endif // __QNEUROLOTS__OPENGLWIDGET__
