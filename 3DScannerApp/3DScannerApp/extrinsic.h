#ifndef EXTRINSIC_H
#define EXTRINSIC_H

using cv::Mat;

class Extrinsic {
private:
	Mat rotationMatrix;
	Mat translationMatrix;
public:
	Extrinsic(Mat RotationMatrix, Mat translationMatrix);
	Mat getRotationMatrix();
	Mat getTranslationMatrix();
};

#endif //EXTRINSIC_H