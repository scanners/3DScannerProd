#ifndef LINE_H
#define LINE_H

using cv::Vec3f;
using cv::Point3f;
using cv::Vec6f;

class Line {
private:
	Vec3f vector;
	Point3f point;
public:
	Line(Vec6f);
	Point3f approximateIntersection(Line);
};

#endif //LINE_H