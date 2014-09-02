#ifndef IMAGEMODIFIER_H
#define IMAGEMODIFIER_H

#include <QImage>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "point.h"

using namespace std;

class ImageModifier
{
public:
	ImageModifier(QImage & image);
	~ImageModifier();
	QImage * toNormalView(QImage & img, double phival, double thetaval, bool aa); //transforms a whole image from panoramic to normal view using given phi/theta, option antialiasing
	Point toNormalView(Point p, double phival, double thetaval); //transforms a point from panoramic to normal view using given phi/theta
	Point toNormalView(Point p); //transforms a point from panoramic to normal view using previous phi/theta
	int getWidth(); //returns the width of the image
	int getHeight(); //returns the height of the image
	QImage * getImage(); //returns the previously transformed image
	void changeViewAngle(int a); //changes viewing angle / zoom
	int getViewAngle(); //returns the current viewing angle / zoom
	void zoomIn(); //sets the view angle to be smaller
	void zoomOut(); //sets the view angle  to be bigger

private:
	void setUp(QImage & image);
	void createMaps(); //creates all the maps upon construction
	Point fisheyeToHorizontal(Point p) const; //returns the point going from fisheye view to panoramic view
	Point horizontalToFisheye(Point p) const;
	Point horizontalToNormal(Point p) const; //returns the point going from panoramic view to normal view
	Point flip(Point p) const; //flips x and y
	Point scaleFlip(Point p) const;
	QImage antialias(QImage & img); //blur/smooth filter to make image look better
	double width; //width of image
	double height; //height of image
	double phi; //polar angle
	double theta; //azimuthal angle
	double cot(double angle) const; //math cotangent
	QImage * retimg; //image being returned
	Point * part2; //combined map of second part of transformation
	Point * part3; //points to zoomXX
	Point * zoom30; //combined map of third part of transformation with a set view angle
	Point * zoom60;
	Point * zoom90;
	Point * zoom120;
	Point * zoom150;
	int round(double d); //rewrote this to just concatenation
	int viewAngle; //current view angle
	int quality;
};
#endif