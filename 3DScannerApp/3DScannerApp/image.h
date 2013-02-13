#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
using std::vector;

#include "objectPoint.h"
#include "plane.h"
#include "line.h"
#include "extrinsic.h"
#include "intrinsic.h"

using cv::Mat;

class Image {
private:
	vector<ObjectPoint> pointCloud;
	Plane *back;
	Plane *ground;
	Plane *laser;
	Line *bestFit;
public:
	Image(Mat, int);
	void getRedComponent(Image, int);
	void process(Intrinsic, Extrinsic, Extrinsic);
};

#endif //IMAGE_H