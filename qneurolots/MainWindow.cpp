#include "ui_mainwindow.h"
#include "MainWindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>

MainWindow::MainWindow( QWidget* parent_,
                        bool updateOnIdle )
  : QMainWindow( parent_ )
  , _lastOpenedFileName( "" )
  , _ui( new Ui::MainWindow )
  , _openGLWidget( nullptr )
{
  _ui->setupUi( this );

  _ui->actionUpdateOnIdle->setChecked( updateOnIdle );

#ifdef NSOL_USE_BBPSDK
  _ui->actionOpenBlueConfig->setEnabled( true );
#else
  _ui->actionOpenBlueConfig->setEnabled( false );
#endif

#ifdef NSOL_USE_QT5CORE
  _ui->actionOpenXMLScene->setEnabled( true );
#else
  _ui->actionOpenXMLScene->setEnabled( false );
#endif

}

void MainWindow::init( const std::string& zeqUri )
{

  _openGLWidget = new OpenGLWidget( 0, 0, zeqUri );
  this->setCentralWidget( _openGLWidget );
  qDebug( ) << _openGLWidget->format( );

  _openGLWidget->idleUpdate( _ui->actionUpdateOnIdle->isChecked( ));

  _openGLWidget->createNeuronsCollection( );

  connect( _ui->actionUpdateOnIdle, SIGNAL( triggered( )),
           _openGLWidget, SLOT( toggleUpdateOnIdle( )));

  connect( _ui->actionBackgroundColor, SIGNAL( triggered( )),
           _openGLWidget, SLOT( changeClearColor( )));

  connect( _ui->actionOpenBlueConfig, SIGNAL( triggered( )),
           this, SLOT( openBlueConfig( )));

  connect( _ui->actionOpenXMLScene, SIGNAL( triggered( )),
           this, SLOT( openXMLScene( )));


}

MainWindow::~MainWindow( void )
{
    delete _ui;
}


void MainWindow::showStatusBarMessage ( const QString& message )
{
  _ui->statusbar->showMessage( message );
}


void MainWindow::openBlueConfig( void )
{
#ifdef NSOL_USE_BBPSDK

  QString path = QFileDialog::getOpenFileName(
    this, tr( "Open BlueConfig" ), _lastOpenedFileName,
    tr( "BlueConfig ( BlueConfig CircuitConfig);; All files (*)" ),
    nullptr, QFileDialog::DontUseNativeDialog );

  if (path != QString( "" ))
  {
    bool ok;
    QString text = QInputDialog::getText(
      this, tr( "Please select target" ),
      tr( "Cell target:" ), QLineEdit::Normal,
      "Column", &ok );
    if ( ok && !text.isEmpty( ))
    {

      std::string targetLabel = text.toStdString( );
      _lastOpenedFileName = QFileInfo(path).path( );
      std::string fileName = path.toStdString( );

      _openGLWidget->loadData( fileName,
                               OpenGLWidget::TDataFileType::BlueConfig,
                               targetLabel );
    }


  }
#endif

}


void MainWindow::openXMLScene( void )
{
#ifdef NSOL_USE_QT5CORE
  QString path = QFileDialog::getOpenFileName(
    this, tr( "Open XML Scene" ), _lastOpenedFileName,
    tr( "XML ( *.xml);; All files (*)" ), nullptr,
    QFileDialog::DontUseNativeDialog );

  if ( path != QString( "" ))
  {
    std::string fileName = path.toStdString( );
    _openGLWidget->loadData( fileName,
                             OpenGLWidget::TDataFileType::NsolScene );
  }
#endif

}
