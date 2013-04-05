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

Plane::~Plane()
{
	// this class doesn't really need this because
	// it isn't dynamically allocating memory
	// but i'll keep it here for good measure
}