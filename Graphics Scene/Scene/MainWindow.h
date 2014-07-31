#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui\qmainwindow.h>
#include <QtGui\qtextedit.h>
#include <QtGui\qdockwidget.h>
#include <QtGui\qvboxlayout>
#include <Qt\qtimer.h>

QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSignalMapper)

namespace Neumont{ class Slider; }

class MainWindow : public QMainWindow
{
private:
	Q_OBJECT
		
		Neumont::Slider* LightPositionX;
		Neumont::Slider* LightPositionY;
		Neumont::Slider* LightPositionZ;
		
		Neumont::Slider* lowThreshold;
		Neumont::Slider* highThreshold;

		
		Neumont::Slider* k;
		Neumont::Slider* velocity;

		QTimer Timer;
		void SetupMenuBar();
		void CreateDockWindows();

		QTextEdit *center;
		QMenu *dockWidgetMenu;
		QMenu *mainWindowMenu;
		QSignalMapper *mapper;
		QVBoxLayout* layout;

public:
	MainWindow(const QMap<QString, QSize> &customSizeHints, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MainWindow();

	public slots:
	void InitializeShapes(); 
	void Update();
	void ToggleShadows();
	void ToggleNormals();
	void ToggleNoiseTextures();
	void ToggleLights();
	void ToggleAlpha();

};

#endif