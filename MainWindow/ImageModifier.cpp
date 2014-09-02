#include "ImageModifier.h"

ImageModifier::ImageModifier(QImage & image)
{
	quality = 2;
	part2 = NULL;
	zoom30 = NULL;
	zoom60 = NULL;
	zoom90 = NULL;
	zoom120 = NULL;
	zoom150 = NULL;
	retimg = NULL;
	setUp(image);
}

ImageModifier::~ImageModifier()
{
	delete[] part2;
	delete[] zoom30;
	delete[] zoom60;
	delete[] zoom90;
	delete[] zoom120;
	delete[] zoom150;
	delete retimg;
}

void ImageModifier::setUp(QImage & image)
{
	if (part2 != NULL) delete[] part2;
	if (zoom30 != NULL) delete[] zoom30;
	if (zoom60 != NULL) delete[] zoom60;
	if (zoom90 != NULL) delete[] zoom90;
	if (zoom120 != NULL) delete[] zoom120;
	if (zoom150 != NULL) delete[] zoom150;
	if (retimg != NULL) delete retimg;
	phi = 90;
	theta = 90;
	viewAngle = 90;
	int width = this->width = 1000;
	int height = this->height = 500;
	if (image.width() / 2 == image.height())
	{
		width = this->width = image.width();
		height = this->height = image.height();
	}
	zoom30 = new Point[quality*quality*width*height];
	zoom60 = new Point[quality*quality*width*height];
	zoom90 = new Point[quality*quality*width*height];
	zoom120 = new Point[quality*quality*width*height];
	zoom150 = new Point[quality*quality*width*height];
	part2 = new Point[quality*quality*width*height];
	part3 = zoom90;
	createMaps();
	retimg = new QImage(width, height, image.format());
	viewAngle = 90;
}

int ImageModifier::getHeight()
{
	return height;
}

int ImageModifier::getWidth()
{
	return width;
}

QImage * ImageModifier::getImage()
{
	return retimg;
}

void ImageModifier::createMaps()
{
	width *= quality;
	height *= quality;
	for(int x = 0; x<(int)width; x++)
	{
		for(int y=0; y<height; y++)
		{
			part2[(int)(int(y)*width + int(x))] = horizontalToFisheye(flip(fisheyeToHorizontal(Point(x, y))));
			Point r = scaleFlip(Point(x, y));
			viewAngle = 30;
			zoom30[(int)(int(y)*width + int(x))] = horizontalToNormal(r);
			viewAngle = 60;
			zoom60[(int)(int(y)*width + int(x))] = horizontalToNormal(r);
			viewAngle = 90;
			zoom90[(int)(int(y)*width + int(x))] = horizontalToNormal(r);
			viewAngle = 120;
			zoom120[(int)(int(y)*width + int(x))] = horizontalToNormal(r);
			viewAngle = 150;
			zoom150[(int)(int(y)*width + int(x))] = horizontalToNormal(r);

			viewAngle = 90;
		}
	}
	width /= quality;
	height /= quality;
}

QImage * ImageModifier::toNormalView(QImage & img, double phival, double thetaval, bool aa)
{
	if (width != img.width() || height != img.height())
		setUp(img);
	if (viewAngle == 360)
		return &img;
	phi = phival;
	theta = thetaval;
	width *= quality;
	height *= quality;
#pragma omp parallel for
	for (int x = 0; x<(int)width; x+=quality)
	{
		for (int y = 0; y<height; y+=quality)
		{
			/*Point v = Point(x, y);
			v = scaleFlip(v);
			v = horizontalToNormal(v);
			v.x = v.x + (phi - 90) / 360 * width;
			while (v.x>width)
				v.x -= width;
			v = fisheyeToHorizontal(v);
			v = flip(v);
			v = horizontalToFisheye(v);
			v.x = v.x + (theta+270) / 360 * width;
			while (v.x > width)
				v.x -= width;*/

			Point r = part3[(int)(round(y)*width + round(x))];
			r.x = (r.x + (phi - 90) / 360 * width);
			while (r.x > width-1)
				r.x -= width;
			Point v = part2[(int)(round(r.y)*width + round(r.x))];
			v.x = (v.x + (theta + 270) / 360 * width);
			while (v.x > width-1)
				v.x -= width;
			retimg->setPixel(x/quality, y/quality, img.pixel(v.x/quality, v.y/quality));
		}
	}
	width /= quality;
	height /= quality;
	return retimg;
}

Point ImageModifier::toNormalView(Point p)
{
	if(viewAngle==360)
		return p;
	p.x *= quality;
	p.y *= quality;
	width *= quality;
	height *= quality;
	double x = p.x;
	double y = p.y;
	Point r = part3[(int)(round(y)*width+round(x))];
	r.x = (int)(r.x+(phi-90)/360*width)%(int)width;
	Point v = part2[(int)(round(r.y)*width+round(r.x))];
	v.x = (int)(v.x+(theta+270)/360*width)%(int)width;
	width /= quality;
	height /= quality;
	v.x /= quality;
	v.y /= quality;
	return v;
}

Point ImageModifier::toNormalView(Point p, double phival, double thetaval)
{
	if(viewAngle==360)
		return p;
	p.x *= quality;
	p.y *= quality;
	width *= quality;
	height *= quality;
	phi = phival;
	theta = thetaval;
	double x = p.x;
	double y = p.y;
	Point r = part3[(int)(round(y)*width+round(x))];
	r.x = (int)(r.x+(phi-90)/360*width)%(int)width;
	Point v = part2[(int)(round(r.y)*width+round(r.x))];
	v.x = (int)(v.x+(360-theta+270)/360*width)%(int)width;
	width /= quality;
	height /= quality;
	v.x /= quality;
	v.y /= quality;
	return v;
}

Point ImageModifier::fisheyeToHorizontal(Point p) const
{
	double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
	if (p.x == height/2 || p.x == height*3/2)
		return p;
	if(p.x<width/2)
	{
		double r = height/2;
		double x = p.x-r;
		double y = r-p.y;
		double cx = (x*x-r*r)/2/x;
		double cy = 0;
		double rad = (x*x+r*r)/2/x;
		double halfdeg = atan(r/cx)/PI*180;
		double deg = 180-halfdeg+p.y/height*halfdeg*2;
		p.x = cx - rad*cos(deg/180*PI);
		p.y = cy - rad*sin(deg/180*PI);
		p.x = p.x+r;
		p.y = r-p.y;
		
	}
	else
	{
		p.x = p.x - width/2;
		double r = height/2;
		double x = p.x-r;
		double y = r-p.y;
		double cx = (x*x-r*r)/2/x;
		double cy = 0;
		double rad = (x*x+r*r)/2/x;
		double halfdeg = atan(r/cx)/PI*180;
		double deg = 180-halfdeg+p.y/height*halfdeg*2;
		p.x = cx - rad*cos(deg/180*PI);
		p.y = cy - rad*sin(deg/180*PI);
		p.x = p.x+r;
		p.x = p.x+width/2;
		p.y = r-p.y;
	}
	if (int(p.x)>int(width - 1))
		p.x = width - 1;
	if (int(p.x)<0)
		p.x = 0;
	if (int(p.y)>int(height - 1))
		p.y = height - 1;
	if (int(p.y)<0)
		p.y = 0;
	return p;
}

Point ImageModifier::horizontalToFisheye(Point p) const
{
	if (p.x < width / 2)
	{
		double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
		double r = height / 2;
		double x = p.x - r;
		double y = r - p.y;
		double cx = (x*x + y*y - r*r) / 2 / x;
		double cy = 0;
		double rad = sqrt(cx*cx + r*r);
		if (x<0)
			rad = 0 - rad;
		double halfdeg = atan(r / cx) / PI * 180;
		double deg = atan(y / (cx - x)) / PI * 180;
		//cout << halfdeg << ", " << deg << endl;
		p.x = cx + rad;
		p.y = (halfdeg - deg)*height / halfdeg / 2;//(deg + halfdeg - 180)*height / halfdeg / 2;
		p.x = p.x + r;
		//p.y = r - p.y;
		if (int(p.x)>int(width - 1))
			p.x = width - 1;
		if (int(p.x)<0)
			p.x = 0;
		if (int(p.y)>int(height - 1))
			p.y = height - 1;
		if (int(p.y) < 0)
			p.y = 0;
	}
	return p;
}

Point ImageModifier::horizontalToNormal(Point p) const
{
	double y = p.y;
	double x = p.x;
	double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
	double startphi = 90 - viewAngle / 2;
	double starttheta = 90 - viewAngle / 2;
	double phi1 = startphi + viewAngle*y / height;
	double theta1 = starttheta + viewAngle*x / width;
	double newx = theta1 / 360 * width;
	double newy = atan(tan(phi1*PI/180) / sin(theta1*PI / 180)) / PI*height;
	if ((int)newx >= width)
		newx = (int)newx % (int)width;
	if ((int)newy >= height)
		newy = height - 1;
	while ((int)newx<0)
		newx += width;
	if ((int)newy<0)
		newy = newy + height;
	p.x = newx;
	p.y = newy;
	return p;
}

Point ImageModifier::flip(Point p) const
{
	if (p.x < height)
		return Point(p.y, p.x);
	else
		return Point(p.x, p.y);
}

Point ImageModifier::scaleFlip(Point p) const
{
	double x = p.x;
	double y = p.y;
	return Point(y / height*width, x / width*height);
}

QImage ImageModifier::antialias(QImage & img)
{
	QImage retimg(width, height, img.format());
#pragma omp parallel for
	for (int x = 1; x<(int)width - 1; x++)
	{
		for (int y = 1; y<height - 1; y++)
		{
			QRgb around[8];
			around[0] = img.pixel(x - 1, y - 1);
			around[1] = img.pixel(x, y - 1);
			around[2] = img.pixel(x + 1, y - 1);
			around[3] = img.pixel(x - 1, y);
			around[4] = img.pixel(x + 1, y);
			around[5] = img.pixel(x - 1, y + 1);
			around[6] = img.pixel(x, y + 1);
			around[7] = img.pixel(x + 1, y + 1);
			int r = 0;
			int g = 0;
			int b = 0;
			for (int i = 0; i<8; i++)
			{
				r += qRed(around[i]);
				g += qGreen(around[i]);
				b += qBlue(around[i]);
			}
			r += qRed(img.pixel(x, y));
			g += qGreen(img.pixel(x, y));
			b += qBlue(img.pixel(x, y));
			r = r / 9;
			g = g / 9;
			b = b / 9;
			retimg.setPixel(x, y, (r << 16) + (g << 8) + b);
		}
	}
	return retimg;
}

double ImageModifier::cot(double angle) const
{
	return cos(angle) / sin(angle);
}

int ImageModifier::round(double d)
{
	return int(d + .5);
}

int ImageModifier::getViewAngle()
{
	return viewAngle;
}

void ImageModifier::changeViewAngle(int a)
{
	viewAngle = a;
}

void ImageModifier::zoomIn()
{
	if (viewAngle == 30)
	{
		;
	}
	else if (viewAngle == 60)
	{
		viewAngle = 30;
		part3 = zoom30;
	}
	else if (viewAngle == 90)
	{
		viewAngle = 60;
		part3 = zoom60;
	}
	else if (viewAngle == 120)
	{
		viewAngle = 90;
		part3 = zoom90;
	}
	else if (viewAngle == 150)
	{
		viewAngle = 120;
		part3 = zoom120;
	}
	else if (viewAngle == 360)
	{
		viewAngle = 150;
		part3 = zoom150;
	}
}

void ImageModifier::zoomOut()
{
	if (viewAngle == 30)
	{
		viewAngle = 60;
		part3 = zoom60;
	}
	else if (viewAngle == 60)
	{
		viewAngle = 90;
		part3 = zoom90;
	}
	else if (viewAngle == 90)
	{
		viewAngle = 120;
		part3 = zoom120;
	}
	else if (viewAngle == 120)
	{
		viewAngle = 150;
		part3 = zoom150;
	}
	else if (viewAngle == 150)
	{
		viewAngle = 360;
	}
	else if (viewAngle == 360)
	{
		;
	}
}