#include <QMainWindow>
#include "OpenGLWidget.h"

namespace Ui
{
class MainWindow;
}

class MainWindow
  : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow( QWidget* parent = 0,
                       bool updateOnIdle = false );
  ~MainWindow( void );

  void init( const std::string& zeqUri = "" );
  void showStatusBarMessage ( const QString& message );

public slots:

  void openBlueConfig( );
  void openXMLScene( );

protected:

  QString _lastOpenedFileName;


private:

    Ui::MainWindow* _ui;
    OpenGLWidget* _openGLWidget;


};
