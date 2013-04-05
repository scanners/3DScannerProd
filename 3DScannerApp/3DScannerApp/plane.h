#ifndef PLANE_H
#define PLANE_H

#include "extrinsic.h"

using cv::Point3f;
using cv::Vec3f;

class Plane {
private:
	Point3f pointOnPlane;
	Vec3f normalVector;
public:
	Plane(Point3f pointOnPlane, Vec3f normalVector);
	~Plane();
	Point3f getPointOnPlane();
	Vec3f getNormalVector();
};

#endif //PLANE_H