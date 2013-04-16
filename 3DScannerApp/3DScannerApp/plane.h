#ifndef PLANE_H
#define PLANE_H

#include "extrinsic.h"

using cv::Point3d;
using cv::Vec3d;

class Plane {
private:
	Point3d pointOnPlane;
	Vec3d normalVector;
public:
	Plane(Point3d pointOnPlane, Vec3d normalVector);
	~Plane();
	Point3d getPointOnPlane();
	Vec3d getNormalVector();
};

#endif //PLANE_H