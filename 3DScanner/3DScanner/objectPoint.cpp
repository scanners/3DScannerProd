#include "stdafx.h"

#include "objectPoint.h"

ObjectPoint::ObjectPoint(Point3f point, int frame) {
	worldCoord = point;
	frameNumber = frame;
}

Point3f ObjectPoint::getWorldCoords() {
	return worldCoord;
}