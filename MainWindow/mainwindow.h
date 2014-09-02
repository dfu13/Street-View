#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QImage>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <QScrollArea>
#include <iostream>
#include "display.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void open();
	bool save();
	bool saveAs();

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	

	QString currFileName;
	QScrollArea * scrollArea;
	Display * display;

	QMenu *fileMenu;
	QAction *openAct;
	QAction *saveAct;

};

#endif // MAINWINDOW_H
