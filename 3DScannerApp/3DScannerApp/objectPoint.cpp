#include "stdafx.h"

#include "objectPoint.h"

ObjectPoint::ObjectPoint(Point3f worldCoord, int imageNum) {
	worldCoordinate = worldCoord;
	imageNumber = imageNum;
}

Point3f ObjectPoint::getWorldCoords() {
	return worldCoordinate;
}