#include "ui_mainwindow.h"
#include "MainWindow.h"
#include <qneurolots/version.h>
#ifdef NEUROLOTS_USE_GMRVZEQ
#include <gmrvzeq/version.h>
#endif
#ifdef NEUROLOTS_USE_DEFLECT
#include <deflect/version.h>
#endif


#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow( QWidget* parent_,
                        bool updateOnIdle )
  : QMainWindow( parent_ )
  , _lastOpenedFileName( "" )
  , _ui( new Ui::MainWindow )
  , _openGLWidget( nullptr )
{
  _ui->setupUi( this );

  _ui->actionUpdateOnIdle->setChecked( updateOnIdle );
  _ui->actionShowFPSOnIdleUpdate->setChecked( false );

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


  connect( _ui->actionQuit, SIGNAL( triggered( )),
           QApplication::instance(), SLOT( quit( )));

  connect( _ui->actionAbout, SIGNAL(triggered( )),
           this, SLOT( showAbout( )));


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

  connect( _ui->actionShowFPSOnIdleUpdate, SIGNAL( triggered( )),
           _openGLWidget, SLOT( toggleShowFPS( )));

  connect( _ui->actionWireframe, SIGNAL( triggered( )),
           _openGLWidget, SLOT( toggleWireframe( )));

  connect( _ui->actionOpenBlueConfig, SIGNAL( triggered( )),
           this, SLOT( openBlueConfigThroughDialog( )));

  connect( _ui->actionOpenXMLScene, SIGNAL( triggered( )),
           this, SLOT( openXMLSceneThroughDialog( )));

  connect( _ui->actionOpenSWCFile, SIGNAL( triggered( )),
           this, SLOT( openSWCFileThroughDialog( )));

}

MainWindow::~MainWindow( void )
{
    delete _ui;
}


void MainWindow::showStatusBarMessage ( const QString& message )
{
  _ui->statusbar->showMessage( message );
}

void MainWindow::openBlueConfig( const std::string& fileName,
                                 const std::string& targetLabel )
{
  _openGLWidget->loadData( fileName,
                           OpenGLWidget::TDataFileType::BlueConfig,
                           targetLabel );

}

void MainWindow::openBlueConfigThroughDialog( void )
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
      openBlueConfig( fileName, targetLabel );
    }


  }
#endif

}

void MainWindow::openXMLScene( const std::string& fileName )
{
  _openGLWidget->loadData( fileName,
    OpenGLWidget::TDataFileType::NsolScene );
}


void MainWindow::openXMLSceneThroughDialog( void )
{
#ifdef NSOL_USE_QT5CORE
  QString path = QFileDialog::getOpenFileName(
    this, tr( "Open XML Scene" ), _lastOpenedFileName,
    tr( "XML ( *.xml);; All files (*)" ), nullptr,
    QFileDialog::DontUseNativeDialog );

  if ( path != QString( "" ))
  {
    std::string fileName = path.toStdString( );
    openXMLScene( fileName );
  }
#endif

}


void MainWindow::openSWCFile( const std::string& fileName )
{
  _openGLWidget->loadData( fileName,
    OpenGLWidget::TDataFileType::SWC );
}


void MainWindow::openSWCFileThroughDialog( void )
{
  QString path = QFileDialog::getOpenFileName(
    this, tr( "Open Swc File" ), _lastOpenedFileName,
    tr( "swc ( *.swc);; All files (*)" ), nullptr,
    QFileDialog::DontUseNativeDialog );

  if ( path != QString( "" ))
  {
    std::string fileName = path.toStdString( );
    openSWCFile( fileName );
  }

}


void MainWindow::showAbout( void )
{

  QMessageBox::about(
    this, tr( "About " ) + tr( "QNeurolots" ),
    tr( "<p><BIG><b>" ) + tr( "QNeurolots" ) + tr( "</b></BIG><br><br>" ) +
    tr( "version " ) +
    tr( qneurolots::Version::getString( ).c_str( )) +
    tr( " (" ) +
    tr( std::to_string( qneurolots::Version::getRevision( )).c_str( )) +
    tr( ")" ) +
    tr( "<br><br>Using: " ) +
    tr( "<ul>") +
    tr( "<li>nsol " ) +
    tr( nsol::Version::getString( ).c_str( )) +
    tr( " (" ) +
    tr( std::to_string( nsol::Version::getRevision( )).c_str( )) +
    tr( ")</li> " ) +

    tr( "<li>BBPSDK " ) +
#ifdef NSOL_USE_BBPSDK
    tr( bbp::Version::getString( ).c_str( )) +
    tr( " (" ) +
    tr( std::to_string( bbp::Version::getRevision( )).c_str( )) +
    tr( ")" ) +
#else
    tr( "not built-in " ) +
#endif
    tr ( "</li> " ) +

    tr( "<li>ZEQ " ) +
#ifdef NEUROLOTS_USE_ZEQ
    tr( zeq::Version::getString( ).c_str( )) +
    tr( " (" ) +
    tr( std::to_string( zeq::Version::getRevision( )).c_str( )) +
    tr( ")" ) +
#else
    tr( "not built-in " ) +
#endif
    tr ( "</li> " ) +

    tr( "<li>gmrvzeq " ) +
#ifdef NEUROLOTS_USE_GMRVZEQ
    tr( gmrvzeq::Version::getString( ).c_str( )) +
    tr( " (" ) +
    tr( std::to_string( gmrvzeq::Version::getRevision( )).c_str( )) +
    tr( ")" ) +
#else
    tr( "not built-in " ) +
#endif
    tr ( "</li> " ) +

    tr( "<li>Deflect " ) +
#ifdef NEUROLOTS_USE_DEFLECT
    tr( deflect::Version::getString( ).c_str( )) +
    tr( " (" ) +
    tr( std::to_string( deflect::Version::getRevision( )).c_str( )) +
    tr( ")" ) +
#else
    tr( "not built-in " ) +
#endif
    tr ( "</li> " ) +

    tr ( "</ul>" ) +
    tr( "<br>GMRV - Universidad Rey Juan Carlos<br>"
       "<a href=www.gmrv.es>www.gmrv.es</a><br>"
       "<a href='mailto:gmrv@gmrv.es'>gmrv@gmrv.es</a><br><br>"
       "<br>(C) 2015. Universidad Rey Juan Carlos<br><br>"
       "<img src=':/icons/gmrv_grande.png' >&nbsp;&nbsp;&nbsp;&nbsp;"
       "<img src=':/icons/logoURJC.png' ><br><br> "
       "</p>"
       ""));

}
