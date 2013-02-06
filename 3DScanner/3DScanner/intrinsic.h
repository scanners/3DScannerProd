#ifndef INTRINSIC_H
#define INTRINSIC_H

using cv::Mat;

class Intrinsic {
private:
	Mat intrinsic;
	Mat distortion;
public:
	Intrinsic(Mat, Mat);
	Mat getIntrinsic();
	Mat getDistortion();
};

#endif INTRINSIC_H