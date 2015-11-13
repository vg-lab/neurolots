#include "ui_mainwindow.h"
#include "MainWindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QGroupBox>
#include <QScrollArea>

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


  _extractMeshDock = new QDockWidget( );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                       _extractMeshDock, Qt::Vertical );
  _extractMeshDock->setSizePolicy(QSizePolicy::MinimumExpanding,
                             QSizePolicy::Expanding);

  _extractMeshDock->setFeatures(QDockWidget::DockWidgetClosable |
                           QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetFloatable);
  // _extractMeshDock->setWindowTitle( QString( "To define" ));
  _extractMeshDock->setMinimumSize( 200, 200 );

  _extractMeshDock->close( );

  QWidget* newWidget = new QWidget( );
  _extractMeshDock->setWidget( newWidget );

  QVBoxLayout* _dockLayout = new QVBoxLayout( );
  _dockLayout->setAlignment( Qt::AlignTop );
  newWidget->setLayout( _dockLayout );

  //Neurons group
  QGroupBox* _neuronsGroup = new QGroupBox( QString( "Select Neuron" ));
  QVBoxLayout* _neuronsLayout = new QVBoxLayout( );
  _neuronsGroup->setLayout( _neuronsLayout );
  _dockLayout->addWidget( _neuronsGroup );

  _neuronList = new QListWidget( );
  _neuronsLayout->addWidget( new QLabel( QString( "Neurons" )));
  _neuronsLayout->addWidget( _neuronList );

  // Soma reconstruction group
  QGroupBox* _somaGroup = new QGroupBox( QString( "Soma reconstruction" ));
  QVBoxLayout* _somaGroupLayout = new QVBoxLayout( );
  _somaGroup->setLayout( _somaGroupLayout );
  _dockLayout->addWidget( _somaGroup );

  _radiusSlider = new QSlider( Qt::Horizontal );
  _radiusSlider->setMinimum(0);
  _radiusSlider->setMaximum(100);
  _radiusSlider->setValue(100);

  _somaGroupLayout->addWidget( new QLabel( QString( "Alpha radius" )));
  _somaGroupLayout->addWidget( _radiusSlider );

  QScrollArea* _neuritesArea = new QScrollArea( );
  _neuritesArea->setSizePolicy( QSizePolicy::MinimumExpanding,
                            QSizePolicy::Expanding );
  _neuritesArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  _neuritesArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  _neuritesArea->setWidgetResizable( true );
  _neuritesArea->setFrameShape( QFrame::NoFrame );
  _somaGroupLayout->addWidget( _neuritesArea );
  QWidget* _neuritesWidget = new QWidget( );
  _neuritesArea->setWidget( _neuritesWidget );
  _neuritesLayout = new QVBoxLayout( );
  _neuritesWidget->setLayout( _neuritesLayout );

  _generateButton = new QPushButton( QString( "Generate" ));
  _somaGroupLayout->addWidget( _generateButton );

// Extraction group
  QGroupBox* _extractionGroup = new QGroupBox( QString( "Mesh extraction" ));
  QVBoxLayout* _extractionLayout = new QVBoxLayout( );
  _extractionGroup->setLayout( _extractionLayout );
  _dockLayout->addWidget( _extractionGroup );

  _extractButton = new QPushButton( QString( "Extract" ));
  _extractionLayout->addWidget( _extractButton );


  for( unsigned int i = 0; i < 100; i++ )
  {
    _neuronList->addItem( QString::number( i ));
  }

  for( unsigned int i = 0; i < 8; i++ )
  {
    QSlider* _neuriteSlider = new QSlider( Qt::Horizontal );
    _neuritesLayout->addWidget( new QLabel( QString( "Alpha neurite " ) +
                                            QString::number( i )));
    _neuritesLayout->addWidget( _neuriteSlider );
  }

  connect( _neuronList, SIGNAL( itemClicked( QListWidgetItem* )),
           this, SLOT( onListClicked( QListWidgetItem* )));

  connect( _extractMeshDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionExtractMesh, SLOT( setChecked( bool )));
  connect( _ui->actionExtractMesh, SIGNAL( triggered( )),
           this, SLOT( updateExtractMeshDock( )));
}

void MainWindow::init( const std::string& zeqUri )
{

  _openGLWidget = new OpenGLWidget( 0, 0, zeqUri );
  this->setCentralWidget( _openGLWidget );
  _openGLWidget->setMinimumSize( QSize( 100, 100 ));
  qDebug( ) << _openGLWidget->format( );

  _openGLWidget->idleUpdate( _ui->actionUpdateOnIdle->isChecked( ));

  _openGLWidget->createNeuronsCollection( );

  connect( _ui->actionHome, SIGNAL( triggered( )),
           _openGLWidget, SLOT( home( )));

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
  updateNeuronList( );
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
  updateNeuronList( );
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
  updateNeuronList( );
}

void MainWindow::updateNeuronList( void )
{
  _neuronList->clear( );
  std::vector< unsigned int > ids = _openGLWidget->neuronIDs( );

  for( auto id: ids )
  {
    _neuronList->addItem( QString::number( id ));
  }
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

void MainWindow::updateExtractMeshDock( void )
{
  if( _ui->actionExtractMesh->isChecked( ))
    _extractMeshDock->show( );
  else
    _extractMeshDock->close( );
}

void MainWindow::onListClicked( QListWidgetItem* item )
{
  int id = item->text( ).toInt( );
  std::cout << "List press " <<  id << std::endl;
  _openGLWidget->neuron( id );
}
