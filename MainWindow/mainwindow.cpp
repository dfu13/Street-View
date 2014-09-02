#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	display = new Display();

	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Midlight);
	scrollArea->setWidget(display);
	scrollArea->setAlignment(Qt::AlignCenter);

	setCentralWidget(scrollArea);

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	currFileName = "";

	resize(1280, 720);
	setWindowTitle(tr("Sphere View"));
}

MainWindow::~MainWindow()
{
	delete display;
	delete scrollArea;

	delete fileMenu;
	delete openAct;
	delete saveAct;
}

void MainWindow::createActions()
{
	openAct = new QAction(tr("&Open"), this);
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	saveAct = new QAction(tr("&Save"), this);
	connect(saveAct, SIGNAL(triggered()), this, SLOT(saveAs()));

}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
}

void MainWindow::createToolBars()
{

}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::open()
{
	QString filename = QFileDialog::getOpenFileName(this);
	if (!filename.isEmpty())
	{
		statusBar()->showMessage(tr("Loading..."));
		display->setImage(filename);
		currFileName = filename;
		scrollArea->setAlignment(Qt::AlignCenter);
		statusBar()->showMessage(tr("Ready"));
	}
}

bool MainWindow::save()
{
	if (!currFileName.isEmpty())
	{
		display->getTransformedImage()->save(currFileName);
		return true;
	}
	else
		return saveAs();
}

bool MainWindow::saveAs()
{
	QString filename = QFileDialog::getSaveFileName(this);
	if (filename.isEmpty())
	{
		statusBar()->showMessage(tr("Please enter a file name."));
		return false;
	}
	else
	{
		display->getTransformedImage()->save(filename);
		currFileName = filename;
		statusBar()->showMessage(tr("Image saved."));
		return true;
	}
}