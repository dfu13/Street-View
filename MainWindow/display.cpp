#include "display.h"

Display::Display(QWidget * parent)
	: QWidget(parent)
{
	theta = 90;
	phi = 90;
	img = new QImage(0,0, QImage::Format_ARGB6666_Premultiplied);
	imgmod = new ImageModifier(*img);
	this->setCursor(Qt::OpenHandCursor);
}

Display::~Display()
{
	delete img;
}

void Display::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	QRect target(0, 0, img->width(), img->height());
	QRect source(0, 0, img->width(), img->height());
	QImage newImg = *imgmod->toNormalView(*img, phi, theta, false);
	painter.drawImage(target, newImg, source);
}

void Display::setImage(QString filename)
{
	img->load(filename);
	setGeometry(0,0,img->width(), img->height());
	repaint();
	phi = 90;
	theta = 90;
}

QImage * Display::getImage()
{
	return img;
}

QImage * Display::getTransformedImage()
{
	return imgmod->getImage();
}

void Display::mousePressEvent(QMouseEvent * event)
{
	lastX = event->x();
	lastY = event->y();
	this->setCursor(Qt::ClosedHandCursor);
}

void Display::mouseMoveEvent(QMouseEvent * event)
{
	double dtheta = ((double)lastX - event->x()) / img->width()*imgmod->getViewAngle();
	theta += dtheta;
	while (theta < 0)
		theta += 360;
	theta = (int(theta) % 360) + (theta - int(theta));
	lastX = event->x();

	double dphi = ((double)lastY - event->y()) / img->height()*imgmod->getViewAngle();
	phi += dphi;
	if (phi < imgmod->getViewAngle() / 2)
		phi = imgmod->getViewAngle() / 2;
	if (phi>180 - imgmod->getViewAngle() / 2)
		phi = 180 - imgmod->getViewAngle() / 2;
	lastY = event->y();
	repaint();
}

void Display::mouseReleaseEvent(QMouseEvent * event)
{
	this->setCursor(Qt::OpenHandCursor);
	repaint();
}

void Display::wheelEvent(QWheelEvent * event)
{
	if (event->delta()>0)
	{
		if (imgmod->getViewAngle() == 360)
		{
			int y = lastY;
			if (y > height()) phi = 180;
			else phi = y * float(180) / float(height());

			int x = lastX;
			if (x > width()) theta = 360;
			else theta = (x * float(360) / float(width()));
		}
		else if (imgmod->getViewAngle() != 30)
		{
			int y = lastY;
			if (y > height()) phi = 180;
			else y = y * float(180) / float(height());
			y = y - 90;
			//y = y*imgmod->getViewAngle()/360;
			//phi = phi + y;
			phi += y * 30 / 180;

			int x = lastX;
			if (x > width()) theta = 360;
			else x = x * float(360) / float(width());
			x = x - 180;
			//x = x*imgmod->getViewAngle()/720;
			//theta = theta + x;
			theta += x * 30 / 360;
		}
		imgmod->zoomIn();
	}
	else
	{

		if (imgmod->getViewAngle() != 360)
		{
			int y = lastY;
			if (y > height()) phi = 180;
			else y = y * float(180) / float(height());
			y = y - 90;
			//y = y*imgmod->getViewAngle()/360;
			//phi = phi + y;
			phi -= y * 30 / 180;

			int x = lastX;
			if (x > width()) theta = 360;
			else x = x * float(360) / float(width());
			x = x - 180;
			//x = x*imgmod->getViewAngle()/720;
			//theta = theta + x;
			theta -= x * 30 / 360;
		}
		imgmod->zoomOut();
	}
	if (phi<imgmod->getViewAngle() / 2)
		phi = imgmod->getViewAngle() / 2;
	if (phi>180 - imgmod->getViewAngle() / 2)
		phi = 180 - imgmod->getViewAngle() / 2; 
	repaint();
}