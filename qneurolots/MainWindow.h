#include <QMainWindow>
#include "OpenGLWidget.h"

#include <QDockWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>

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

  void openBlueConfig( const std::string& fileName,
                       const std::string& target);
  void openXMLScene( const std::string& fileName );
  void openSWCFile( const std::string& fileName );

  void updateNeuronList( void );

public slots:

  void openBlueConfigThroughDialog( void );
  void openXMLSceneThroughDialog( void );
  void openSWCFileThroughDialog( void );

  void updateExtractMeshDock( void );
  void onListClicked( QListWidgetItem *item );

protected:

  QString _lastOpenedFileName;


private:

  void generateNeuritesLayout( void );

  Ui::MainWindow* _ui;
  OpenGLWidget* _openGLWidget;

  QDockWidget* _extractMeshDock;

  QListWidget* _neuronList;
  QSlider* _radiusSlider;
  QVBoxLayout* _neuritesLayout;
  QPushButton* _generateButton;
  QPushButton* _extractButton;
};
