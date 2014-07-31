#include "MainWindow.h"
#include "DebugShapes.h"
#include <QtGui\qmenu.h>
#include <QtGui\qmenubar.h>
#include <QtGui\qstatusbar.h>
#include <QtGui\qfiledialog.h>
#include <glm\gtx\transform.hpp>
#include <QtGui\qlabel.h>
#include <QtGui\qhboxlayout>
#include <QtGui\qcheckbox.h>
#include <QtGui\qlabel.h>
#include "Slider.h"

MainWindow::MainWindow(const QMap<QString, QSize> &customSizeHints, QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{		
	setObjectName("MainWindow");
	setWindowTitle("Scene");

	center = new QTextEdit(this);
	center->setReadOnly(true);
	//->setMinimumSize(1200,750);
	setCentralWidget(center);
	SetupMenuBar();
	CreateDockWindows();

	DebugShapeManager.CreateRender();

	setCentralWidget(DebugShapeManager.renderer);

	connect(DebugShapeManager.renderer, SIGNAL(renderInitialized()), this, SLOT(InitializeShapes()));
	connect(&Timer, SIGNAL(timeout()), this, SLOT(Update()));
	Timer.start(0);
}

MainWindow::~MainWindow()
{
}

void MainWindow::SetupMenuBar()
{	
    QMenu *menu = menuBar()->addMenu(tr("&File"));

	menu->addSeparator();
    menu->addAction(tr("&Quit"), this, SLOT(close()));
}

void MainWindow::CreateDockWindows()
{
	QDockWidget* dock = new QDockWidget(tr("Debug Menu"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	QWidget* Debug = new QWidget;	
	Debug->setMinimumHeight(200);
	layout = new QVBoxLayout; 	

	QHBoxLayout* sliderLayout;
	layout->addLayout(sliderLayout = new QHBoxLayout);
	sliderLayout->addWidget(new QLabel("Light Position"));
	layout->addLayout(sliderLayout = new QHBoxLayout);
	sliderLayout->addWidget(LightPositionX = new Neumont::Slider(-10,10));	
	LightPositionX->setValue(2.5f);
	connect(LightPositionX, SIGNAL(valueChanged(float)),this,SLOT(Update()));
	sliderLayout->addWidget(LightPositionY = new Neumont::Slider(-10,10));
	LightPositionY->setValue(2.5f);
	connect(LightPositionY, SIGNAL(valueChanged(float)),this,SLOT(Update()));
	sliderLayout->addWidget(LightPositionZ = new Neumont::Slider(-10,10));
	LightPositionZ->setValue(-1);
	connect(LightPositionZ, SIGNAL(valueChanged(float)),this,SLOT(Update()));
		
	layout->addLayout(sliderLayout = new QHBoxLayout);
	sliderLayout->addWidget(new QLabel("Melting Sliders"));
	layout->addLayout(sliderLayout = new QHBoxLayout);
	sliderLayout->addWidget(lowThreshold = new Neumont::Slider(0,1));
	connect(lowThreshold, SIGNAL(valueChanged(float)),this,SLOT(Update()));
	sliderLayout->addWidget(highThreshold = new Neumont::Slider(0,1));
	highThreshold->setValue(1);
	connect(highThreshold, SIGNAL(valueChanged(float)),this,SLOT(Update()));

	layout->addLayout(sliderLayout = new QHBoxLayout);
	sliderLayout->addWidget(new QLabel("Number of waves and velocity Sliders for vertex displacement"));
	layout->addLayout(sliderLayout = new QHBoxLayout);
	sliderLayout->addWidget(k = new Neumont::Slider(0,3));
	k->setValue(1);
	connect(lowThreshold, SIGNAL(valueChanged(float)),this,SLOT(Update()));
	sliderLayout->addWidget(velocity = new Neumont::Slider(0,3));
	velocity->setValue(1);
	connect(highThreshold, SIGNAL(valueChanged(float)),this,SLOT(Update()));
	
	QCheckBox* Switchlights;
	layout->addWidget(Switchlights = new QCheckBox("enable/Disable Lights"));
	connect(Switchlights, SIGNAL(toggled(bool)), this, SLOT(ToggleLights()));
	
	QCheckBox* SwitchAlpha;
	layout->addWidget(SwitchAlpha = new QCheckBox("enable/Disable Alpha Mode"));
	connect(SwitchAlpha, SIGNAL(toggled(bool)), this, SLOT(ToggleAlpha()));
	
	QCheckBox* SwitchShadows;
	layout->addWidget(SwitchShadows = new QCheckBox("enable/Disable Shadows"));
	connect(SwitchShadows, SIGNAL(toggled(bool)), this, SLOT(ToggleShadows()));
		
	QCheckBox* SwitchNormals;
	layout->addWidget(SwitchNormals = new QCheckBox("enable/Disable Normals"));
	connect(SwitchNormals, SIGNAL(toggled(bool)), this, SLOT(ToggleNormals()));
	
	Debug->setLayout(layout);
	dock->setWidget(Debug);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void MainWindow::InitializeShapes()
{
	DebugShapeManager.LoadShadows();
	DebugShapeManager.initialize();
	
	glm::mat4 pos = glm::mat4();
	DebugShapeManager.AddLightSource(pos);		
	
	pos = glm::scale(glm::vec3(20,20,20));
	DebugShapeManager.AddSkyBox(pos);	

	pos = glm::translate(glm::vec3(-0.5f,0.3f,0)) * glm::rotate(glm::mat4(), -90.0f, glm::vec3(1,0,0)) * glm::scale(glm::vec3(0.15f,0.15f,0.15f));
	DebugShapeManager.AddTeapot(pos,0 , true, false);
	
	pos = glm::translate(glm::vec3(-3,1,0)) * glm::scale(glm::vec3(0.2f,2,2));
	DebugShapeManager.AddCube(pos, 5, 0, true, false);

	pos = glm::translate(glm::vec3(0,0.2,0)) * glm::scale(glm::vec3(0.18f,0.18f,0.18f));
	DebugShapeManager.AddTable(pos, true, false);
			
	pos = glm::translate(glm::vec3(0,-1,0)) * glm::scale(glm::vec3(0.8f,1,0.4f));
	DebugShapeManager.AddPlane(pos, 0, true, false);
		
	pos = glm::translate(glm::vec3(-2.7,0.9,1.4)) * glm::rotate(glm::mat4(), -90.0f, glm::vec3(0,0,1)) * glm::rotate(glm::mat4(), 90.0f, glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.08f,1,0.28f));
	DebugShapeManager.AddVetexPlane(pos, 0, true, false);
}

void MainWindow::Update()
{
	DebugShapeManager.renderer->LightPosition = glm::vec3(LightPositionX->value(),LightPositionY->value(),LightPositionZ->value());
	DebugShapeManager.LightSource->where = glm::translate(glm::vec3(LightPositionX->value(),LightPositionY->value(),LightPositionZ->value())) * glm::scale(glm::vec3(0.2f,0.2f,0.2f));	
	DebugShapeManager.renderer->LowThreshold = lowThreshold->value();
	DebugShapeManager.renderer->HighThreshold = highThreshold->value();
	DebugShapeManager.renderer->K = k->value();
	DebugShapeManager.renderer->Velocity = velocity->value();
	DebugShapeManager.renderer->repaint();
	clockManager.newFrame();
}

void MainWindow::ToggleShadows()
{
	if(DebugShapeManager.renderer->ShadowsOn)
		DebugShapeManager.renderer->ShadowsOn = false;
	else
		DebugShapeManager.renderer->ShadowsOn= true;

	DebugShapeManager.renderer->repaint();
}

void MainWindow::ToggleNormals()
{	
	if(DebugShapeManager.renderer->NormalsOn)
		DebugShapeManager.renderer->NormalsOn = false;
	else
		DebugShapeManager.renderer->NormalsOn= true;

	DebugShapeManager.renderer->repaint();
}

void MainWindow::ToggleNoiseTextures()
{		
	DebugShapeManager.renderer->repaint();
}

void MainWindow::ToggleLights()
{	
	if(DebugShapeManager.renderer->LightsOn)
		DebugShapeManager.renderer->LightsOn = false;
	else
		DebugShapeManager.renderer->LightsOn= true;

	DebugShapeManager.renderer->repaint();
}

void MainWindow::ToggleAlpha()
{		
	if(DebugShapeManager.Object->Blend)
		DebugShapeManager.Object->Blend = false;
	else
		DebugShapeManager.Object->Blend = true;

	DebugShapeManager.renderer->repaint();
}