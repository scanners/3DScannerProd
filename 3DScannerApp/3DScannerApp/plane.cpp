#include "stdafx.h"

#include "plane.h"

Plane::Plane(Point3d ptOnPlane, Vec3d normal) {
	pointOnPlane = ptOnPlane;
	normalVector = normal;
}

Point3d Plane::getPointOnPlane() {
	return pointOnPlane;
}

Vec3d Plane::getNormalVector() {
	return normalVector;
}

Plane::~Plane()
{
	// this class doesn't really need this because
	// it isn't dynamically allocating memory
	// but i'll keep it here for good measure
}