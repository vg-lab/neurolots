/**
 * @file    MainWindow.h
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>,
 * Pablo Toharia <pablo.toharia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include <QMainWindow>
#include "OpenGLWidget.h"

#include <QDockWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>

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

  void home( void );
  void openBlueConfigThroughDialog( void );
  void openXMLSceneThroughDialog( void );
  void openSWCFileThroughDialog( void );
  void showAbout( void );

  void updateExtractMeshDock( void );
  void updateConfigurationDock( void );
  void onListClicked( QListWidgetItem *item );
  void onActionGenerate( int value_ );

protected:

  QString _lastOpenedFileName;


private:

  void _generateNeuritesLayout( void );
  void _initExtractionDock( void );
  void _initConfigurationDock( void );

  Ui::MainWindow* _ui;
  OpenGLWidget* _openGLWidget;

  QDockWidget* _extractMeshDock;
  QDockWidget* _configurationDock;

  QListWidget* _neuronList;
  QSlider* _radiusSlider;
  QVBoxLayout* _neuritesLayout;
  std::vector< QSlider* > _neuriteSliders;

  QPushButton* _extractButton;

  QSlider* _lotSlider;
  QSlider* _distanceSlider;
  QSlider* _tangSlider;

  QGroupBox* _somaGroup;

  QRadioButton* _radioHomogeneous;
  QRadioButton* _radioLinear;

};
