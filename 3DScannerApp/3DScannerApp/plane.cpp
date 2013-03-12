#include "stdafx.h"

#include "plane.h"

Plane::Plane(Point3f ptOnPlane, Vec3f normal) {
	pointOnPlane = ptOnPlane;
	normalVector = normal;
}

Point3f Plane::getPointOnPlane() {
	return pointOnPlane;
}

Vec3f Plane::getNormalVector() {
	return normalVector;
}