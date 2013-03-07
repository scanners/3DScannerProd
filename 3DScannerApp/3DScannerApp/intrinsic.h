#ifndef INTRINSIC_H
#define INTRINSIC_H

using cv::Mat;

class Intrinsic {
private:
	Mat intrinsicMatrix;
	Mat distortionCoefficients;
public:
	Intrinsic(Mat intrinsicMatrix, Mat distortionCoefficients);
	Mat getIntrinsicMatrix();
	Mat getDistortionCoefficients();
};

#endif INTRINSIC_H