#ifndef DISPLAY_H
#define DISPLAY_H

#include <qwidget.h>
#include <qimage.h>
#include <qpainter.h>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include "ImageModifier.h"

class Display : public QWidget
{
	Q_OBJECT
public:
	Display(QWidget * parent = 0);
	~Display();
	void setImage(QString filename);
	QImage * getImage();
	QImage * getTransformedImage();
	QImage * img;

protected:
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void wheelEvent(QWheelEvent * event);

private:
	void paintEvent(QPaintEvent *);
	ImageModifier * imgmod;
	int lastX;
	int lastY;
	double theta;
	double phi;
	

};
#endif