#ifndef OBJECTPOINT_H
#define OBJECTPOINT_H

using cv::Point3f;

class ObjectPoint {
private:
	Point3f worldCoord;
	int frameNumber;
public:
	ObjectPoint(Point3f, int);
	Point3f getWorldCoords();
};

#endif //OBJECTPOINT_H