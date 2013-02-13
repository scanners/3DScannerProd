#include "stdafx.h"

#include "plane.h"

Plane::Plane(Point3f pointInStage, Vec3f normal) {
	pointOnPlane = pointInStage;
	normalVector = normal;
}

float Plane::findIntersection(Point3f cameraOrigin, Vec3f imageVec) {
	return 0.0;
}

void Plane::convertToCameraCoordinates(Extrinsic extrinsic) {

}