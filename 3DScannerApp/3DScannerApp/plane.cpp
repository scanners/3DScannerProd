#include "stdafx.h"

#include "plane.h"

Plane::Plane(Point3f ptOnPlane, Vec3f normal) {
	pointOnPlane = ptOnPlane;
	normalVector = normal;
}

float Plane::findIntersection(Point3f cameraOrigin, Vec3f imageVec) {
	return 0.0;
}

void Plane::convertToCameraCoordinates(Extrinsic extrinsic) {

}