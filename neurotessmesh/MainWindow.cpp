/**
 * @file    MainWindow.cpp
 * @brief
 * @author  Juan José García <juanjose.garcia@urjc.es>,
 * Pablo Toharia <pablo.toharia@urjc.es>
 * @date    2015
 * @remarks Copyright (c) 2015 GMRV/URJC. All rights reserved.
 * Do not distribute without further notice.
 */

#include "ui_mainwindow.h"
#include "MainWindow.h"
#include <neurotessmesh/version.h>
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

  connect( _ui->actionAbout, SIGNAL(triggered( )),
           this, SLOT( showAbout( )));

  _initExtractionDock( );
  _initConfigurationDock( );
}

MainWindow::~MainWindow( void )
{
    delete _ui;
}

// PUBLIC

void MainWindow::init( const std::string& zeqUri )
{

  _openGLWidget = new OpenGLWidget( 0, 0, zeqUri );
  this->setCentralWidget( _openGLWidget );
  _openGLWidget->setMinimumSize( QSize( 100, 100 ));
  qDebug( ) << _openGLWidget->format( );

  _openGLWidget->idleUpdate( _ui->actionUpdateOnIdle->isChecked( ));

  _openGLWidget->createNeuronsCollection( );

  connect( _ui->actionHome, SIGNAL( triggered( )),
           this, SLOT( home( )));

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

  connect( _radiusSlider, SIGNAL( valueChanged( int )),
           this, SLOT( onActionGenerate( int )));

  connect( _extractButton, SIGNAL( clicked( )),
           _openGLWidget, SLOT( extractMesh( )));

  connect( _lotSlider, SIGNAL( valueChanged( int )),
           _openGLWidget, SLOT( onLotValueChanged( int )));

  _lotSlider->valueChanged( _lotSlider->value( ));

  connect( _distanceSlider, SIGNAL( valueChanged( int )),
           _openGLWidget, SLOT( onDistanceValueChanged( int )));

  _distanceSlider->valueChanged( _distanceSlider->value( ));

  connect( _tangSlider, SIGNAL( valueChanged( int )),
           _openGLWidget, SLOT( onTangValueChanged( int )));

  _tangSlider->valueChanged( _tangSlider->value( ));

  connect( _radioHomogeneous, SIGNAL( clicked( )),
           _openGLWidget, SLOT( onHomogeneousClicked( )));

  connect( _radioLinear, SIGNAL( clicked( )),
           _openGLWidget, SLOT( onLinearClicked( )));
  _radioLinear->clicked( );
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

void MainWindow::openXMLScene( const std::string& fileName )
{
  _openGLWidget->loadData( fileName,
    OpenGLWidget::TDataFileType::NsolScene );
  updateNeuronList( );
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
  const std::vector< unsigned int >& ids = _openGLWidget->neuronIDs( );

  for( const auto& id: ids )
  {
    _neuronList->addItem( QString::number( id ));
  }
}

// PUBLIC SLOTS

void MainWindow::home( void )
{
  _openGLWidget->home( );
  _generateNeuritesLayout( );
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
    this, tr( "About " ) + tr( "NeuroTessMesh" ),
    tr( "<p><BIG><b>" ) + tr( "NeuroTessMesh" ) + tr( "</b></BIG><br><br>" ) +
    tr( "version " ) +
    tr( neurotessmesh::Version::getString( ).c_str( )) +
    tr( " (" ) +
    tr( std::to_string( neurotessmesh::Version::getRevision( )).c_str( )) +
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
       "")
    );
}

void MainWindow::updateExtractMeshDock( void )
{
  if( _ui->actionExtractMesh->isChecked( ))
    _extractMeshDock->show( );
  else
    _extractMeshDock->close( );
}

void MainWindow::updateConfigurationDock( void )
{
  if( _ui->actionConfiguration->isChecked( ))
    _configurationDock->show( );
  else
    _configurationDock->close( );
}

void MainWindow::onListClicked( QListWidgetItem* item )
{
  int id = item->text( ).toInt( );
  _openGLWidget->neuron( id );
  _generateNeuritesLayout( );
}

void MainWindow::onActionGenerate( int /*value_*/ )
{

  float alphaRadius = ( float )_radiusSlider->value( ) / 100.0f;
  std::vector< float > alphaNeurites;

  for ( unsigned int i = 0; i < _neuriteSliders.size( ); i++ )
  {
    alphaNeurites.push_back(( float ) _neuriteSliders[i]->value( ) / 100.0f );
  }

  _openGLWidget->regenerateNeuron( alphaRadius, alphaNeurites );
}

// PRIVATE

void MainWindow::_generateNeuritesLayout( void )
{
  unsigned int numDendrites = _openGLWidget->numNeurites( );

  _neuriteSliders.clear( );

  QLayoutItem* child;
  while(( child = _neuritesLayout->takeAt( 0 )) != 0 )
  {
    delete child->widget( );
  }

  QSlider* _neuriteSlider;
  for( unsigned int i = 0; i < numDendrites; i++ )
  {
    _neuriteSlider = new QSlider( Qt::Horizontal );
    _neuriteSlider->setMinimum(0);
    _neuriteSlider->setMaximum(200);
    _neuriteSlider->setValue(100);
    _neuritesLayout->addWidget( new QLabel( QString( "Neurite " ) +
                                            QString::number( i ) +
                                            QString( " factor" )));

    _neuritesLayout->addWidget( _neuriteSlider );
    _neuriteSliders.push_back( _neuriteSlider );
    // connect( _neuriteSlider, SIGNAL( sliderReleased( )),
    //        this, SLOT( onActionGenerate( )));
    connect( _neuriteSlider, SIGNAL( valueChanged( int )),
           this, SLOT( onActionGenerate( int )));
  }
  _radiusSlider->setValue(100);
}

void MainWindow::_initExtractionDock( void )
{
  _extractMeshDock = new QDockWidget( );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                       _extractMeshDock, Qt::Vertical );
  _extractMeshDock->setSizePolicy(QSizePolicy::MinimumExpanding,
                             QSizePolicy::Expanding);

  _extractMeshDock->setFeatures(QDockWidget::DockWidgetClosable |
                           QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetFloatable);
  _extractMeshDock->setWindowTitle( QString( "Reconstruction" ));
  _extractMeshDock->setMinimumSize( 200, 200 );

  _extractMeshDock->close( );

  QWidget* newWidget = new QWidget( );
  _extractMeshDock->setWidget( newWidget );

  QVBoxLayout* _meshDockLayout = new QVBoxLayout( );
  _meshDockLayout->setAlignment( Qt::AlignTop );
  newWidget->setLayout( _meshDockLayout );

  //Neurons group
  QGroupBox* _neuronsGroup = new QGroupBox( QString( "Select Neuron" ));
  QVBoxLayout* _neuronsLayout = new QVBoxLayout( );
  _neuronsGroup->setLayout( _neuronsLayout );
  _meshDockLayout->addWidget( _neuronsGroup );

  _neuronList = new QListWidget( );
  _neuronsLayout->addWidget( new QLabel( QString( "Neurons" )));
  _neuronsLayout->addWidget( _neuronList );

  // Soma reconstruction group
  QGroupBox* _somaGroup = new QGroupBox( QString( "Soma reconstruction" ));
  QVBoxLayout* _somaGroupLayout = new QVBoxLayout( );
  _somaGroup->setLayout( _somaGroupLayout );
  _meshDockLayout->addWidget( _somaGroup );

  _radiusSlider = new QSlider( Qt::Horizontal );
  _radiusSlider->setMinimum( 25 );
  _radiusSlider->setMaximum( 100 );
  _radiusSlider->setValue( 100 );

  _somaGroupLayout->addWidget( new QLabel( QString( "Radius factor" )));
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

// Extraction group
  QGroupBox* _extractionGroup = new QGroupBox( QString( "Mesh extraction" ));
  QVBoxLayout* _extractionLayout = new QVBoxLayout( );
  _extractionGroup->setLayout( _extractionLayout );
  _meshDockLayout->addWidget( _extractionGroup );

  _extractButton = new QPushButton( QString( "Extract" ));
  _extractionLayout->addWidget( _extractButton );

  connect( _neuronList, SIGNAL( itemClicked( QListWidgetItem* )),
           this, SLOT( onListClicked( QListWidgetItem* )));

  connect( _extractMeshDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionExtractMesh, SLOT( setChecked( bool )));
  connect( _ui->actionExtractMesh, SIGNAL( triggered( )),
           this, SLOT( updateExtractMeshDock( )));

}

void MainWindow::_initConfigurationDock( void )
{
  _configurationDock = new QDockWidget( );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::LeftDockWidgetArea,
                       _configurationDock, Qt::Vertical );
  _configurationDock->setSizePolicy(QSizePolicy::MinimumExpanding,
                             QSizePolicy::Expanding);

  _configurationDock->setFeatures(QDockWidget::DockWidgetClosable |
                           QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetFloatable);
  _configurationDock->setWindowTitle( QString( "Configuration" ));
  _configurationDock->setMinimumSize( 200, 200 );

  _configurationDock->close( );

  QWidget* newWidget = new QWidget( );
  _configurationDock->setWidget( newWidget );

  QVBoxLayout* _configDockLayout = new QVBoxLayout( );
  _configDockLayout->setAlignment( Qt::AlignTop );
  newWidget->setLayout( _configDockLayout );

  QGroupBox* tessParamsGroup = new QGroupBox( QString( "Tessellation params" ));
  _configDockLayout->addWidget( tessParamsGroup );
  QVBoxLayout* vbox = new QVBoxLayout;
  tessParamsGroup->setLayout( vbox );

  _lotSlider = new QSlider( Qt::Horizontal );
  _lotSlider->setMinimum( 1 );
  _lotSlider->setMaximum( 30 );
  _lotSlider->setValue( 4 );
  vbox->addWidget(
    new QLabel( QString( "Level" )));
  vbox->addWidget( _lotSlider );

  _distanceSlider = new QSlider( Qt::Horizontal );
  _distanceSlider->setMinimum( 0 );
  _distanceSlider->setMaximum( 1000 );
  _distanceSlider->setValue( 10 );
  vbox->addWidget(
    new QLabel( QString( "Maximum Distance" )));
  vbox->addWidget( _distanceSlider );

  _tangSlider = new QSlider( Qt::Horizontal );
  _tangSlider->setMinimum( 0 );
  _tangSlider->setMaximum( 50 );
  _tangSlider->setValue( 10 );
  vbox->addWidget(
    new QLabel( QString( "Tangent smoothing" )));
  vbox->addWidget( _tangSlider );

  // Radio Buttons Group

  QGroupBox* tessMethodGroup = new QGroupBox( QString( "Tessellation method" ));
  _configDockLayout->addWidget( tessMethodGroup );
  vbox = new QVBoxLayout;
  tessMethodGroup->setLayout( vbox );

  _radioHomogeneous = new QRadioButton( QString( "Homogeneous" ));
  vbox->addWidget( _radioHomogeneous );
  _radioLinear = new QRadioButton( QString( "Linear" ));
  vbox->addWidget( _radioLinear );

  _radioLinear->setChecked( true );

  connect( _configurationDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionConfiguration, SLOT( setChecked( bool )));
  connect( _ui->actionConfiguration, SIGNAL( triggered( )),
           this, SLOT( updateConfigurationDock( )));

}
