#ifndef OBJECTPOINT_H
#define OBJECTPOINT_H

using cv::Point3f;

class ObjectPoint {
private:
	Point3f worldCoordinate;
	int imageNumber;
public:
	ObjectPoint(Point3f worldCoord, int imageNumber);
	Point3f getWorldCoords();
};

#endif //OBJECTPOINT_H